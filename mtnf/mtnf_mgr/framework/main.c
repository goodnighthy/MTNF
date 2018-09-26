#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

#include <rte_mbuf.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_launch.h>
#include <rte_log.h>
#include <rte_branch_prediction.h>

#include "mtnf_common.h"
#include "mtnf_args.h"
#include "mtnf_init.h"
#include "mtnf_stats.h"

#define RTE_LOGTYPE_MTNF          RTE_LOGTYPE_USER1
#define MAX_PKT_BURST 32
#define BUFFER_SIZE 128

static volatile bool keep_running = 1;

static void
handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        keep_running = 0;
    }
}

static int
worker_thread(void *arg) {
	uint8_t port_id, nb_rx, nb_tx;
	struct rte_mbuf *pkts[PACKET_READ_SIZE];
	struct worker_info *worker_info = (struct worker_info *)arg;

	port_id = ports->id[worker_info->id];

	RTE_LOG(INFO, MTNF, "Core %d: Running worker thread\n", rte_lcore_id());

	for (; keep_running;) {
		nb_rx = rte_eth_rx_burst(port_id, 0, pkts, MAX_PKT_BURST);
		ports->stats[port_id].rx += nb_rx;

		nb_tx = rte_eth_tx_burst(port_id, 0, pkts, nb_rx);
		if (unlikely(nb_tx < nb_rx)) {
			pktmbuf_free_bulk(&pkts[nb_tx], nb_rx - nb_tx);
			ports->stats[port_id].tx_drop += (nb_rx - nb_tx);
		}
		ports->stats[port_id].tx += nb_tx;
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
    worker_lcores = rte_lcore_count() - 1;

    RTE_LOG(INFO, MTNF, "%d cores available in total\n", rte_lcore_count());
    RTE_LOG(INFO, MTNF, "%d cores available for worker\n", worker_lcores);
    RTE_LOG(INFO, MTNF, "%d cores available for master\n", 1);

	/* Listen for ^C and docker stop so we can exit gracefully */
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

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