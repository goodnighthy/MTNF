#ifndef _MTNF_PORT_H_
#define _MTNF_PORT_H_

#include <stdint.h>
#include <rte_mempool.h>

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
void
init_all_ports(const char* ports_info_name, uint32_t port_mask, struct rte_mempool *mbuf_pool);

/* Check the link status of all ports in up to 9s, and print them finally */
void 
check_all_ports_link_status(uint32_t port_mask);

/* display the statistics of all ports */
void 
display_ports(unsigned difftime, struct ports_info *portsinfo);

#endif