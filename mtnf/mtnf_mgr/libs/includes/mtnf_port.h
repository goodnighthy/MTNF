#ifndef _MTNF_PORT_H_
#define _MTNF_PORT_H_

#include <stdint.h>
#include <rte_mempool.h>
#include <rte_ether.h>

#define RX_QUEUE 1
#define TX_QUEUE 1

#define RX_DESC_PER_QUEUE 1024
#define TX_DESC_PER_QUEUE 1024

#define HW_RXCSUM 0
#define HW_TXCSUM 0

#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */

struct ports_info {
        uint8_t num_ports;
        uint8_t id[RTE_MAX_ETHPORTS];	/* the virtual port id to physical port id */
        struct ether_addr mac[RTE_MAX_ETHPORTS];
        volatile struct {
        	uint64_t rx;
        	uint64_t tx;
        	uint64_t tx_drop;
        } ports[RTE_MAX_ETHPORTS] __rte_cache_aligned;
};

/* Init all ports */
struct ports_info *
init_all_ports(const char *ports_info_name, uint32_t port_mask, struct rte_mempool *mbuf_pool);

/* Check the link status of all ports in up to 9s, and print them finally */
void 
check_all_ports_link_status(uint32_t port_mask);

/* display the statistics of all ports */
void 
display_ports(unsigned difftime, struct ports_info *portsinfo);

#endif