#include "mtnf_reasm.h"

#define PKTBUF_SIZE 32 * 16
#define PKTBUF_CAP 1518 // ??? not sure why use this, just copy S6's setting
static double reorder_prob[5] = {0.128, 0.032, 0.008, 0.002, 0.0005};                                                   0.0005};
static char global_pkt_buf[PKTBUF_SIZE][PKTBUF_CAP];
static bool flag_pkt_buf[PKTBUF_SIZE][PKTBUF_CAP];

/* register tenant state */
uint32_t
mtnf_reasm_register(void) {
	return sizeof(struct reasm_statistics);
}

/* init tenant state */
void
mtnf_reasm_init(void *state) {
	struct reasm_statistics *stats;

	stats = (struct reasm_statistics *)state;
    
    memset(stats->flowkey_map, 0, sizeof(struct flow_key) * BIG_PRIME * BUCKET_SIZE);
    memset(stats->dataentry_map, 0, sizeof(struct data_entry) * BIG_PRIME * BUCKET_SIZE);
    memset(stats->bucket_cnt, 0, sizeof(uint16_t) * BIG_PRIME);
    memset(stat->buffered_segs, 0, sizeof(struct Segment) * BUFFERED_SEGS_SIZE);
    memset(global_pkt_buf, 0, sizeof(char) * PKTBUF_SIZE * PKTBUF_CAP);
    memset(flag_pkt_buf, 0, sizeof(char) * PKTBUF_SIZE * PKTBUF_CAP);
}

static void fill_fkey(struct ipv4_hdr *ipv4, struct flow_key *f_key) {
    struct tcp_hdr *tcp_hdr;
    struct udp_hdr *udp_hdr;
    f_key->proto = ipv4->next_proto_id;
    f_key->src_ip = ipv4->src_addr;
    f_key->dst_ip = ipv4->dst_addr;
    if (f_key->proto == IP_PROTOCOL_TCP) {
        tcp_hdr = (struct tcp_hdr *)((uint8_t*)ipv4 + sizeof(struct ipv4_hdr));
        f_key->src_port = tcp_hdr->src_port;
        f_key->dst_port = tcp_hdr->dst_port;
    } else if (f_key->proto == IP_PROTOCOL_UDP) {
        udp_hdr = (struct udp_hdr *)((uint8_t*)ipv4 + sizeof(struct ipv4_hdr));
        f_key->src_port = udp_hdr->src_port;
        f_key->dst_port = udp_hdr->dst_port;
    } else {
        f_key->src_port = 0;
        f_key->dst_port = 0;
    }
}

static uint32_t hash_flowkey(struct flow_key *key) {
    uint64_t hash_val = 0;
    hash_val = key->src_ip % BIG_PRIME;
    hash_val = (hash_val * shift_32 + key->dst_ip) % BIG_PRIME;
    hash_val = (hash_val * shift_16 + key->src_port) % BIG_PRIME;
    hash_val = (hash_val * shift_16 + key->dst_port) % BIG_PRIME;
    hash_val = (hash_val * shift_8 + key->proto) % BIG_PRIME;

    return (uint32_t)hash_val;
}

static void 
get_data_entry(void *state, struct flow_key *key, uint32_t &hash_index, \
            uint16_t &bucket_index, struct data_entry *de) {
    struct  reasm_statistics *stats = (struct reasm_statistics *)state;
    struct flow_key &tmp_key;
    uint16_t bucket_cap;
    int i;

    hash_index = hash_flowkey(key);
    bucket_cap = stats->bucket_cnt[hash_index];
    
    for (bucket_index = 0; bucket_index < bucket_cap; bucket_index ++) {
        tmp_key = stats->flowkey_map[hash_index][bucket_index];
        if (tmp_key.src_ip == key->src_ip && tmp_key.dst_ip == key->dst_ip && \
            tmp_key.src_port == key->src_port && tmp_key.dst_port == key->dst_port && \
            tmp_key.proto  == key->proto) {
            break;
        }
    }

    if (bucket_index == bucket_cap) { /* didn't find */
        de = NULL;
        if (bucket_index == BUCKET_SIZE) { // bucket is full!
            bucket_index = -1;
        }
    } else
        de = &(stats->dataentry_map[hash_index][bucket_index]);
}

static uint32_t get_pkt_seq(struct data_entry *de) {
    uint32_t ret = de->planned_seqs[0];
    int i;

    // Shift elements to the left by one slot
    for (i = 0; i < PLANNED_SEQ_SIZE - 1; i ++) {
        de->planned_seqs[i] = de->planned_seqs[i + 1];
    }
    de->planned_seqs[PLANNED_SEQ_SIZE - 1] = de->next_generated_seq;
    de->next_generated_seq ++;

    double dice = drand48();
    size_t diff;
    for (diff = 5; diff > 0; diff --) {
        if (dice < reorder_prob[diff - 1]) {
            uint32_t tmp_swap;
            int swap_index = rand() % (10 - diff);
            tmp_swap = de->planned_seqs[swap_index];
            de->planned_seqs[swap_index] = de->planned_seqs[swap_index + diff];
            de->planned_seqs[swap_index + diff] = tmp_swap;
            break;
        }
    }

    printf("ret: %u, new seq >>>>> ", ret);
    for (i = 0; i < PLANNED_SEQ_SIZE; i ++)
        printf("%u, ", de->planned_seqs[i]);
    printf("\n");

    return ret;
}

int get_pktbuf_index() {
    int i;
    for (i = 0; i < PKTBUF_SIZE; i ++)
        if (flag_pkt_buf[i] == false)
            return i;
    return -1;
}

/* handle tenant packets */
uint16_t
mtnf_reasm_handler(struct rte_mbuf *pkt[], uint16_t num, void *state) {
	struct reasm_statistics *stats;
	uint16_t i, num_out, bucket_index;
	struct ipv4_hdr* ipv4;
    struct flow_key *fkey;
    uint32_t hash_index;
    struct data_entry *dataentry;
    srand(time(NULL));

	stats = (struct reasm_statistics *)state;
	num_out = num;
	for (i = 0; i < num; i++) {
		ipv4 = mtnf_pkt_ipv4_hdr(pkt[i]);
        printf("packet arrived >>>>>>>>>>>>>>>>>>>>>>>>\n");

        hash_index = 0;
        bucket_index = 0;
        fill_fkey(ipv4, fkey);
        get_data_entry(stats, fkey, hash_index, bucket_index, dataentry);

        uint32_t seq;
        /* if new flow and bucket not full, create new data entry */
        if (dataentry == NULL && bucket_index >= 0) {
            if (bucket_index >= 0)  {
                printf("create new data entry>>>>>>>\n");
                /* create new flow key*/
                struct flow_key &tmp_key;
                tmp_key = stats->flowkey_map[hash_index][bucket_index];
                tmp_key.src_ip = fkey->src_ip;
                tmp_key.dst_ip = fkey->dst_ip;
                tmp_key.src_port = fkey->src_port;
                tmp_key.dst_port = fkey->dst_port;
                tmp_key.proto = fkey->proto;

                /* create new dataentry */
                struct data_entry &tmp_de;
                tmp_de = stats->dataentry_map[hash_index][bucket_index];
                tmp_de.next_expected_seq = 1;
                tmp_de.next_generated_seq = 1;
                uint16_t j;
                for (j = 0; j < PLANNED_SEQ_SIZE; j ++) {
                    tmp_de.planned_seqs[j] = tmp_de.next_generated_seq;
                    tmp_de.next_generated_seq ++;
                }

                dataentry = &(tmp_de);
                seq = get_pkt_seq(dataentry);
            } else {
                seq = 0;
                printf("bucket is full!!!!!!!\n");                
            }
        } else {
            seq = get_pkt_seq(dataentry);
        }


        printf("seq: %u\n", seq);
        if (seq != 0) {
            if (seq == dataentry->next_expected_seq) {
                printf("sequence equal! pulling-----\n");
                bool found_match = true;
                while (found_match) {
                    int tmp_index;
                    found_match = false;
                    dataentry->next_expected_seq ++;
                    for (tmp_index = 0; tmp_index < BUFFERED_SEGS_SIZE; tmp_index ++) {
                        if (dataentry->next_expected_seq == stats->buffered_segs[tmp_index].seq) {
                            printf("%u ", dataentry->next_expected_seq);
                            found_match = true;
                            stats->buffered_segs[tmp_index].seq = 0;
                            flag_pkt_buf[stats->buffered_segs[tmp_index].pktbuf_key] = false;
                            break;
                        }
                    }
                }
                printf("\n");
            } else { /* save it in buffer */
                struct udp_hdr *udp;
                struct tcp_hdr *tcp;
                uint16_t plen, hlen;
                uint8_t *pkt_data, *eth, seg_index;
                /* Check if we have a valid UDP packet */
                udp = mtnf_pkt_udp_hdr(pkt[i]);
                tcp = mtnf_pkt_tcp_hdr(pkt[i]);
                if (udp != NULL) {
                    /* Get at the payload */
                    pkt_data = ((uint8_t *) udp) + sizeof(struct udp_hdr);
                    /* Calculate length */
                    eth = rte_pktmbuf_mtod(pkt[i], uint8_t *);
                    hlen = pkt_data - eth;
                    plen = pkt[i]->pkt_len - hlen;
                }
                /* Check if we have a valid TCP packet */
                if (tcp != NULL) {
                    /* Get at the payload */
                    pkt_data = ((uint8_t *) tcp) + sizeof(struct tcp_hdr);
                    /* Calculate length */
                    eth = rte_pktmbuf_mtod(pkt[i], uint8_t *);
                    hlen = pkt_data - eth;
                    plen = pkt[i]->pkt_len - hlen;
                }

                for (seg_index = 0; seg_index < BUFFERED_SEGS_SIZE; seg_index ++) {
                    if (stats->buffered_segs[seg_index].seq == 0) {
                        break;
                    }
                }

                int pktbuf_index =  get_pktbuf_index();
                printf("pktbuf_index: %u, seg_index: %u, plen: %u\n", \
                    pktbuf_index, seg_index, plen);
                if (plen <= PKTBUF_CAP && pktbuf_index > 0 && \
                    seg_index < BUFFERED_SEGS_SIZE) { 
                    /* if the packet is smaller than PKTBUFF_CAP 
                       and there is free buf 
                    */
                    memcpy(global_pkt_buf[pktbuf_index], pkt_data, plen);
                    flag_pkt_buf[pktbuf_index] = true;
                    stats->buffered_segs[seg_index].seq = seq;
                    stats->buffered_segs[seg_index].pktbuf_key = pktbuf_index;
                }
            }
        }
	}

	return num_out;
}