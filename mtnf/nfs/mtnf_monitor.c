#include "mtnf_monitor.h"

/* register tenant state */
uint32_t
mtnf_monitor_register(void) {
	return sizeof(struct monitor_statistics);
}

/* init tenant state */
void
mtnf_monitor_init(void *state) {
	struct monitor_statistics *stats;

	stats = (struct monitor_statistics *)state;
	stats->tcp_num = 0;
	stats->udp_num = 0;
	for (int i = 0; i < 16; i ++)
		stats->len_cnt[i] = 0;
}

/* handle tenant packets */
uint16_t
mtnf_monitor_handler(struct rte_mbuf *pkt[], uint16_t num, void *state) {
	struct monitor_statistics *stats;
	uint16_t i, num_out, len;
	struct ipv4_hdr* ipv4;

	stats = (struct monitor_statistics *)state;
	num_out = num;
	uint64_t sum, tmpi;
	for (i = 0; i < num; i++) {
		ipv4 = mtnf_pkt_ipv4_hdr(pkt[i]);
		if (mtnf_pkt_is_tcp(pkt[i]) == true)
			stats->tcp_num++;
		if (mtnf_pkt_is_udp(pkt[i]) == true)
			stats->udp_num++;
		len = rte_be_to_cpu_16(ipv4->total_length);
		sum = 0;
		tmpi = 0;
		while (tmpi < len) {
			tmpi += 8;
			sum = stats->len_cnt[len % 17];
			if (sum % 2 == 1)
				sum = sum * 11315137 % 141331;
			else
				sum = sum * 13515113 % 111343;
			stats->len_cnt[len % 17] = sum;
		}
	}

	return num_out;
}