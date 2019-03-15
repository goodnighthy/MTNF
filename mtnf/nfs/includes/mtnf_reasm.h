#ifndef _MTNF_reasm_H_
#define _MTNF_reasm_H_

#include <stdint.h>
#include <stdbool.h>

#include <rte_mbuf.h>
#include <time.h>

#include "mtnf_help.h"

#define BIG_PRIME 10019
#define BUCKET_SIZE 10
#define PLANNED_SEQ_SIZE 10
#define BUFFERED_SEGS_SIZE 32

struct Segment {
    uint32_t seq;
    uint32_t pktbuf_key;  
};

struct data_entry {
    uint32_t next_expected_seq;
    uint32_t next_generated_seq;
    uint32_t planned_seqs[PLANNED_SEQ_SIZE];
    struct Segment buffered_segs[BUFFERED_SEGS_SIZE];
};

struct flow_key {
    uint32_t src_ip, dst_ip;
    uint16_t src_port, dst_port;
    uint8_t proto;
};

struct reasm_statistics {
    struct flow_key flowkey_map[BIG_PRIME][BUCKET_SIZE];
    struct data_entry dataentry_map[BIG_PRIME][BUCKET_SIZE];
    uint16_t bucket_cnt[BIG_PRIME];
};

/* register tenant state */
uint32_t
mtnf_reasm_register(void);

/* init tenant state */
void
mtnf_reasm_init(void *state);

/* handle tenant packets */
uint16_t
mtnf_reasm_handler(struct rte_mbuf *pkt[], uint16_t num, void *state);

#endif