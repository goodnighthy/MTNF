#ifndef _MTNF_AES_ENCRYPT_H_
#define _MTNF_AES_ENCRYPT_H_

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <rte_common.h>
#include <rte_mbuf.h>
#include <rte_ip.h>
#include <rte_udp.h>
#include <rte_ether.h>

#include "mtnf_help.h"
#include "aes.h"

struct aes_encrypt_statistics {
    WORD key_schedule[60]; //word Schedule
};

/* register tenant state */
uint32_t
mtnf_aes_encrypt_register(void);

/* init tenant state */
void
mtnf_aes_encrypt_init(void *state);

/* handle tenant packets */
uint16_t
mtnf_aes_encrypt_handler(struct rte_mbuf *pkt[], uint16_t num, void *state);

#endif