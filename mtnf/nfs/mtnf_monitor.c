#include "mtnf_monitor.h"

/* register tenant state */
uint32_t
mtnf_monitor_register(void) {
	return sizeof(struct monitor_statistics);
}

/* handle tenant packets */
uint16_t
mtnf_monitor_handler(struct rte_mbuf *pkt[], uint16_t num, void *state) {
	struct monitor_statistics *stats;
	uint16_t i, num_out;

	stats = (struct monitor_statistics *)state;
	num_out = num;
	for (i = 0; i < num; i++) {
		if (mtnf_pkt_is_tcp(pkt[i]) == true)
			stats->tcp_num++;
		if (mtnf_pkt_is_udp(pkt[i]) == true)
			stats->udp_num++;
	}

	return num_out;
}