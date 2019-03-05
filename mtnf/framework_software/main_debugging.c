#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

#include <rte_mbuf.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_launch.h>
#include <rte_log.h>
#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>
#include <rte_prefetch.h>
#include <rte_branch_prediction.h>

#include "mtnf_queue.h"
#include "mtnf_help.h"

#include "mtnf_common.h"
#include "mtnf_args.h"
#include "mtnf_tenant.h"
#include "mtnf_init.h"
#include "mtnf_stats.h"

#define RTE_LOGTYPE_MTNF          RTE_LOGTYPE_USER1
#define MAX_PKT_BURST 32
#define MAX_PKT_BUFFER 32
#define BUFFER_SIZE 128

/* test time latency */
/*
#include <sys/time.h>
#define TIMESLOT_NUM 10000
static unsigned long process_time_slot[TIMESLOT_NUM];
static unsigned long buffer_time_slot[TIMESLOT_NUM];
static unsigned long start_usec, end_usec;
static int buffer_odd_cnt;
*/

#define BIG_PRIME 10019
static volatile bool keep_running = 1;
static uint16_t shift_8 = 1UL << 8;
static uint32_t shift_16 = 1UL << 16;
static uint64_t shift_32 = 1UL << 32;
static uint8_t hash_table[BIG_PRIME];

static void
handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        keep_running = 0;
    }
}

struct ipv4_key {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t  proto;
};

struct mtnf_classifier {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t  proto;
    uint8_t tenant_id;
};

/* calc hash value */
static uint32_t
mtnf_hash_val(struct mtnf_classifier* tmp_turple) {
    uint64_t ret = 0;
    ret = tmp_turple->src_addr % BIG_PRIME;
    ret = (ret * shift_32 + tmp_turple->dst_addr) % BIG_PRIME;
    ret = (ret * shift_16 + tmp_turple->src_port) % BIG_PRIME;
    ret = (ret * shift_16 + tmp_turple->dst_port) % BIG_PRIME;
    ret = (ret * shift_8 + tmp_turple->proto) % BIG_PRIME;
    return (uint32_t)ret;
}

static int
dispatcher_thread(void *arg) {
    uint8_t i, port_id, tid, wid;
    uint16_t j, k, nb_rx, nb_tx, hash_index;
    uint32_t min_load, min_queue;
    uint64_t timeslot;
    struct rte_mbuf *pkts[PACKET_READ_SIZE];
    struct timeval cur_time;
    struct ipv4_hdr *ipv4_hdr;
    struct tcp_hdr *tcp_hdr;
    struct udp_hdr *udp_hdr;
    struct rte_ring *worker_queue;
    struct ipv4_key key;
    /*
    struct mtnf_classifier classifier_table[MAX_TENANTS] = {{3232235521, 3232235777, 1234, 5678, 6, 1},
                                                            {3232235522, 3232235777, 1234, 5678, 6, 2},
                                                            {3232235523, 3232235777, 1234, 5678, 6, 3},
                                                            {3232235524, 3232235777, 1234, 5678, 6, 4},
                                                            {3232235525, 3232235777, 1234, 5678, 6, 5},
                                                            {3232235526, 3232235777, 1234, 5678, 6, 6},
                                                            {3232235527, 3232235777, 1234, 5678, 6, 7},
                                                            {3232235528, 3232235777, 1234, 5678, 6, 8}};
    */
    struct mtnf_classifier classifier_table[MAX_TENANTS] = {{16820416, 16885952, 1234, 5678, 6, 1},
                                                            {33597632, 16885952, 1234, 5678, 6, 2},
                                                            {50374848, 16885952, 1234, 5678, 6, 3},
                                                            {67152064, 16885952, 1234, 5678, 6, 4},
                                                            {83929280, 16885952, 1234, 5678, 6, 5},
                                                            {100706496, 16885952, 1234, 5678, 6, 6},
                                                            {117483712, 16885952, 1234, 5678, 6, 7},
                                                            {134260928, 16885952, 1234, 5678, 6, 8}};
    uint32_t tenants_table[MAX_TENANTS][4] = {{0, 0, 0, 2000000},
                                              {0, 0, 1, 2000000},
                                              {0, 0, 2, 2000000},
                                              {0, 0, 3, 2000000}};
    uint32_t workers_table[QUEUE_NUM][5] = {{0, 0, 0, 0, 10000000},
                                            {0, 0, 0, 0, 10000000},
                                            {0, 0, 0, 0, 10000000},
                                            {0, 0, 0, 0, 10000000}}; 
    
    /* create hashtable, if two tenant falls into the same bucket, fails */
    memset(hash_table, 0, sizeof(hash_table));
    for (i = 0; i < MAX_TENANTS; i ++) {
        hash_index = mtnf_hash_val(&classifier_table[i]);
//        printf("%d\n", hash_index);
        hash_table[hash_index] = i + 1;
    }

    struct dispatcher_info *dispatcher_info = (struct dispatcher_info *)arg;
    RTE_LOG(INFO, MTNF, "Core %d: Running dispatcher thread\n", rte_lcore_id());

    for (; keep_running;) {
        for (i = 0; i < dispatcher_info->ports_num; i++) {
            port_id = ports->id[i];
            nb_rx = rte_eth_rx_burst(port_id, 0, pkts, MAX_PKT_BURST);
            ports->stats[port_id].rx += nb_rx;

            for (j = 0; j < nb_rx; j++) {
                ipv4_hdr = mtnf_pkt_ipv4_hdr(pkts[j]);
                memset(&key, 0, sizeof(struct ipv4_key));
                key.proto  = ipv4_hdr->next_proto_id;
                key.src_addr = ipv4_hdr->src_addr;
                key.dst_addr = ipv4_hdr->dst_addr;
                if (key.proto == IP_PROTOCOL_TCP) {
                        tcp_hdr = mtnf_pkt_tcp_hdr(pkts[j]);
                        key.src_port = rte_be_to_cpu_16(tcp_hdr->src_port);
                        key.dst_port = rte_be_to_cpu_16(tcp_hdr->dst_port);
                } else if (key.proto == IP_PROTOCOL_UDP) {
                        udp_hdr = mtnf_pkt_udp_hdr(pkts[j]);
                        key.src_port = rte_be_to_cpu_16(udp_hdr->src_port);
                        key.dst_port = rte_be_to_cpu_16(udp_hdr->dst_port);
                } else {
                        key.src_port = 0;
                        key.dst_port = 0;
                }
                tid = 0;
                for (k = 0; k < MAX_TENANTS; k++) {
                    if (key.src_addr == classifier_table[k].src_addr &&
                        key.dst_addr == classifier_table[k].dst_addr &&
                        key.src_port == classifier_table[k].src_port &&
                        key.dst_port == classifier_table[k].dst_port &&
                        key.proto == classifier_table[k].proto) {
                        tid = classifier_table[k].tenant_id;
                        break;
                    }
                }
//                printf("src ip%u, dst ip%u\n", (int)key.src_addr, (int)key.dst_addr);
//                printf("src addr:%u; dst addr%u; src port:%u; dst port:%u; proto:%u\n", key.src_addr, key.dst_addr, key.src_port, key.dst_port, key.proto);
                ipv4_hdr->type_of_service = tid;
                gettimeofday(&cur_time, NULL);
                timeslot = (uint64_t)(cur_time.tv_sec * 1e6 + cur_time.tv_usec);
                timeslot = (timeslot >> 20);
                wid = tenants_table[tid][2];
                if (timeslot == tenants_table[tid][0]) {
                    tenants_table[tid][1] += 1;
                    if (tenants_table[tid][1] > tenants_table[tid][3])
                        rte_pktmbuf_free(pkts[j]);
                } else {
                    tenants_table[tid][0] = timeslot;
                    tenants_table[tid][1] = 1;
                    if (timeslot != workers_table[wid][0]) {
                        workers_table[wid][0] = timeslot;
                        workers_table[wid][2] = workers_table[wid][1];
                        workers_table[wid][1] = 0;
                    }
                    if (workers_table[wid][2] < workers_table[wid][4]) {
                        workers_table[wid][3] = 0;
                    } else {
                        workers_table[wid][3]++;
                        min_load = 0xffffffff;
                        min_queue = 0;
                        for (k = 0; k < QUEUE_NUM; k++) {
                            if (min_load > workers_table[k][2]) {
                                min_load = workers_table[k][2];
                                min_queue = k;
                            }
                        }
                        min_load = min_load + tenants_table[tid][1];
                        if (min_load < workers_table[min_queue][4]) {
                            tenants_table[tid][2] = min_queue;
                            workers_table[min_queue][2] = min_load;
                            workers_table[wid][2] -= tenants_table[tid][1];
                        }
                    }
                }
                workers_table[wid][1]++;
                q_buffers[wid].buffer_slot[q_buffers[wid].num++] = pkts[j];
//                printf("adding %d, it has %d now\n", (int)wid, (int)q_buffers[wid].num);
                if (q_buffers[wid].num == MAX_PKT_BUFFER) {
                    worker_queue = queue_lookup(get_queue_name(wid));
                    nb_tx = rte_ring_enqueue_burst(worker_queue, (void **)q_buffers[wid].buffer_slot, MAX_PKT_BURST, NULL);
                    if (unlikely(nb_tx < MAX_PKT_BUFFER)) {
                        pktmbuf_free_bulk(&q_buffers[wid].buffer_slot[nb_tx], MAX_PKT_BURST - nb_tx);
                    }
                    q_buffers[wid].num = 0;
                }
            }
        }
    }
    RTE_LOG(INFO, MTNF, "Core %d: dispatcher thread done\n", rte_lcore_id());

    return 0;
}

static int
worker_thread(void *arg) {
    uint8_t port_id, tenant_id;
	uint16_t i, nb_rx, nb_tx, nb_handler;
	struct rte_mbuf *pkts[PACKET_READ_SIZE];
    struct rte_ring *worker_queue;
	struct worker_info *worker_info = (struct worker_info *)arg;

    /* test time latency */
    /*
    struct timeval start[2], end_process;
    buffer_odd_cnt = -1;
    */
    
    port_id = ports->id[worker_info->id];
    worker_queue = queue_lookup(get_queue_name(worker_info->id));

	RTE_LOG(INFO, MTNF, "Core %d: Running worker thread\n", rte_lcore_id());

	for (; keep_running;) {
        nb_rx = rte_ring_dequeue_burst(worker_queue, (void **)pkts, MAX_PKT_BURST, NULL);
/*        if (nb_rx > 0) {
            printf("%d receiving %d\n", port_id, nb_rx);
        }
*/
        for (i = 0; i < nb_rx; i++) {
            rte_prefetch0(rte_pktmbuf_mtod(pkts[i], void *));
            
//            mtnf_pkt_print(pkts[i]);

            tenant_id = get_tenant_id(pkts[i]);
            t_buffers[tenant_id].buffer_slot[t_buffers[tenant_id].num++] = pkts[i];

            if (t_buffers[tenant_id].num == MAX_PKT_BUFFER) {
                /* test time latenct */
                /* get buffer time */
                /*
                if (buffer_odd_cnt == -1) {
                    gettimeofday(&start[1], NULL);
                } else {
                    gettimeofday(&start[buffer_odd_cnt % 2], NULL);
                    start_usec = start[(buffer_odd_cnt + 1) % 2].tv_usec;
                    end_usec = start[buffer_odd_cnt % 2].tv_usec;
                    if (start_usec > end_usec)
                        end_usec += 1000000;
                    buffer_time_slot[buffer_odd_cnt] = (end_usec - start_usec);
                }
                buffer_odd_cnt = (buffer_odd_cnt + 1) % TIMESLOT_NUM;
                */

                tenants[tenant_id].stats.rx += t_buffers[tenant_id].num;

                /* handle by network function */
                nb_handler = mtnf_packets_handler(&t_buffers[tenant_id], tenant_id);

                /* get the time of processing */
                /*
                gettimeofday(&end_process, NULL);
                end_usec = end_process.tv_usec;
                start_usec = start[(buffer_odd_cnt + 1) % 2].tv_usec;
                if (start_usec > end_usec)
                    end_usec += 1000000;
                process_time_slot[buffer_odd_cnt] = (end_usec - start_usec);
                */

                tenants[tenant_id].stats.tx += nb_handler;

//                pktmbuf_free_bulk(&t_buffers[tenant_id].buffer_slot[0], nb_handler);
                nb_tx = rte_eth_tx_burst(port_id, 0, t_buffers[tenant_id].buffer_slot, nb_handler);
                if (unlikely(nb_tx < nb_handler)) {
                    pktmbuf_free_bulk(&t_buffers[tenant_id].buffer_slot[nb_tx], nb_handler - nb_tx);
                    ports->stats[port_id].tx_drop += (nb_handler - nb_tx);
                }
                ports->stats[port_id].tx += nb_tx;
            }
        }

		
	}

	RTE_LOG(INFO, MTNF, "Core %d: worker thread done\n", rte_lcore_id());

	return 0;
}

static int
master_thread(void) {
	const uint64_t sleeptime = timer_period;

	RTE_LOG(INFO, MTNF, "Core %d: Running master thread\n", rte_lcore_id());

	/* Initial pause so above printf is seen */
    sleep(3);

    /* Loop forever: sleep always returns 0 or <= param */
    while (keep_running && sleep(sleeptime) <= sleeptime) {
        mtnf_stats_display_all(sleeptime, ports, tenants);
        /* test time latency */
        /*
        int i;
        uint64_t buffer_sum = 0, process_sum = 0;
        for (i = 0; i < buffer_odd_cnt; i ++) {
            buffer_sum += buffer_time_slot[i];
            process_sum += process_time_slot[i];
        }
        printf("buffer latency: %f, process latency: %f", \
            (double)buffer_sum / buffer_odd_cnt, (double)process_sum / buffer_odd_cnt);
        */
    }

    RTE_LOG(INFO, MTNF, "Core %d: Master thread done\n", rte_lcore_id());

    return 0;
}

int
main(int argc, char *argv[]) {
	unsigned cur_lcore, worker_lcores;
	unsigned i;

	/* init the system */
	if (init(argc, argv) < 0)
		return -1;
	RTE_LOG(INFO, MTNF, "Finished Process Init.\n");

	cur_lcore = rte_lcore_id();
    worker_lcores = rte_lcore_count() - 2;

    RTE_LOG(INFO, MTNF, "%d cores available in total\n", rte_lcore_count());
    RTE_LOG(INFO, MTNF, "%d cores available for worker\n", worker_lcores);
    RTE_LOG(INFO, MTNF, "%d cores available for master\n", 1);

	/* Listen for ^C and docker stop so we can exit gracefully */
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    /* Launch dispatcher thread on core */
    struct dispatcher_info *dispatcher_info = calloc(1, sizeof(struct dispatcher_info));
    dispatcher_info->ports_num = ports->num_ports;
    cur_lcore = rte_get_next_lcore(cur_lcore, 1, 1);
    if (rte_eal_remote_launch(dispatcher_thread, (void*)dispatcher_info,  cur_lcore) == -EBUSY) {
            RTE_LOG(ERR, MTNF, "Core %d is already busy, can't use for dispatcher\n", cur_lcore);
            return -1;
    }

    /* Launch each worker thread on cores */
    for (i = 0; i < worker_lcores; i++) {
            struct worker_info *worker_info = calloc(1, sizeof(struct worker_info));
            worker_info->id = i;
            cur_lcore = rte_get_next_lcore(cur_lcore, 1, 1);
            if (rte_eal_remote_launch(worker_thread, (void*)worker_info,  cur_lcore) == -EBUSY) {
                    RTE_LOG(ERR, MTNF, "Core %d is already busy, can't use for worker %d\n", cur_lcore, worker_info->id);
                    return -1;
            }
    }

    /* Master thread handles statistics and NF management */
    master_thread();

    return 0;
}