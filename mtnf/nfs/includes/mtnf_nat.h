#ifndef _MTNF_MONITOR_H_
#define _MTNF_MONITOR_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include <rte_mbuf.h>

#include "mtnf_help.h"

#define BIG_PRIME 10019
#define BUCKET_SIZE 10

struct nat_entry {
    uint32_t new_ip;
    uint16_t new_port;
    bool is_forward;
};

struct flow_key {
    uint32_t src_ip, dst_ip;
    uint16_t src_port, dst_port;
    uint8_t proto;
};

struct nat_statistics {
    struct flow_key flowkey_map[BIG_PRIME][BUCKET_SIZE];
    struct nat_entry natentry_map[BIG_PRIME][BUCKET_SIZE];
    uint16_t bucket_cnt[BIG_PRIME];
    bool port_used[65536];
};

/* register tenant state */
uint32_t
mtnf_nat_register(void);

/* init tenant state */
void
mtnf_nat_init(void *state);

/* handle tenant packets */
uint16_t
mtnf_nat_handler(struct rte_mbuf *pkt[], uint16_t num, void *state);

#endif