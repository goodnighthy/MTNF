#ifndef _MTNF_IDS_H_
#define _MTNF_IDS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdbool.h>
#include <string.h>

#include <rte_mbuf.h>

#include "mtnf_help.h"

#define IDS_RULE_NUM 6
#define STR_LEN 30

struct ids_statistics {
	char str[IDS_RULE_NUM][STR_LEN];
	int next[IDS_RULE_NUM][STR_LEN];
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