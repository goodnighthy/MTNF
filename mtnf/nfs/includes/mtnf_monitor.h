#ifndef _MTNF_MONITOR_H_
#define _MTNF_MONITOR_H_

#include <stdint.h>
#include <stdbool.h>

#include <rte_mbuf.h>

#include "mtnf_help.h"

struct monitor_statistics
{
	uint64_t tcp_num;
	uint64_t udp_num;
	uint16_t len_cnt[17];
};

/* register tenant state */
uint32_t
mtnf_monitor_register(void);

/* init tenant state */
void
mtnf_monitor_init(void *state);

/* handle tenant packets */
uint16_t
mtnf_monitor_handler(struct rte_mbuf *pkt[], uint16_t num, void *state);

#endif