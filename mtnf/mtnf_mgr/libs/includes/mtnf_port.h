#ifndef _MTNF_PORT_H_
#define _MTNF_PORT_H_

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_memzone.h>
#include <rte_ether.h>
#include <rte_cycles.h>


#define RX_QUEUE 1
#define TX_QUEUE 1

#define RX_DESC_PER_QUEUE 1024
#define TX_DESC_PER_QUEUE 1024

#define HW_RXCSUM 0
#define HW_TXCSUM 0

#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */

uint8_t rss_symmetric_key[40] = {    0x6d, 0x5a, 0x6d, 0x5a,
                                     0x6d, 0x5a, 0x6d, 0x5a,
                                     0x6d, 0x5a, 0x6d, 0x5a,
                                     0x6d, 0x5a, 0x6d, 0x5a,
                                     0x6d, 0x5a, 0x6d, 0x5a,
                                     0x6d, 0x5a, 0x6d, 0x5a,
                                     0x6d, 0x5a, 0x6d, 0x5a,
                                     0x6d, 0x5a, 0x6d, 0x5a,
                                     0x6d, 0x5a, 0x6d, 0x5a,
                                     0x6d, 0x5a, 0x6d, 0x5a,};

/* Init all ports */
int
init_all_ports(uint32_t port_mask, struct rte_mempool *mbuf_pool);

/* Check the link status of all ports in up to 9s, and print them finally */
void 
check_all_ports_link_status(uint32_t port_mask);

#endif