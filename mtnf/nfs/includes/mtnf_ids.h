#ifndef _MTNF_IDS_H_
#define _MTNF_IDS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdbool.h>
#include <string.h>

#include <rte_mbuf.h>

#include "mtnf_help.h"

#define IDS_RULE_NUM 3

struct ids_statistics {
	char str[3][50];
	int next[3][50];
};

/* register tenant state */
uint32_t
mtnf_ids_register(void);

/* init tenant state */
void
mtnf_ids_init(void *state);

/* handle tenant packets */
uint16_t
mtnf_ids_handler(struct rte_mbuf *pkt[], uint16_t num, void *state);

#endif