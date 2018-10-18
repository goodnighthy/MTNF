#ifndef _MTNF_TENANT_H_
#define _MTNF_TENANT_H_

#include <stdint.h>

#include "mtnf_monitor.h"
#include "mtnf_aes_encrypt.h"
#include "mtnf_aes_decrypt.h"
#include "mtnf_firewall.h"

#include "mtnf_common.h"
#include "mtnf_args.h"

/* init tenants state */
void
mtnf_states_init(void);

/* packet handler for tenant id */
uint16_t
mtnf_packets_handler(struct tenants_buffer *buffer, uint8_t id);

#endif