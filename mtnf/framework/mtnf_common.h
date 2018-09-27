#ifndef _MTNF_COMMON_H_
#define _MTNF_COMMON_H_

#include <stdint.h>

#include <rte_mbuf.h>
#include <rte_ether.h>

#define MAX_TENANTS 16

#define MBUFS_PER_PORT 2048
#define MBUFS_PER_TENANT 256

/* default period is 1 seconds */
#define TIMER_PERIOD 1

#define PACKET_READ_SIZE 32

/* define common names for structures */
#define PKTMBUF_POOL_NAME "mtnf_pktmbuf_pool"
#define PKTMBUF_BUFFER_NAME "mtnf_pktmbuf_buffer"
#define MTNF_PORT_INFO "mtnf_port_info"
#define MTNF_TENANT_INFO "mtnf_tenant_info"
#define MTNF_TENANT_STATE "mtnf_tenant_state"

/* define a structure to describe a worker */
struct worker_info
{
	uint8_t id;
};

/* define a structure to buffer packets for each tenant */
struct tenants_buffer
{
	struct rte_mbuf *buffer_slot[MBUFS_PER_TENANT];
	uint16_t num;
};

/* define a structure to describe a tenant */
struct tenants_info
{
	uint8_t id;
	volatile struct {
        uint64_t rx;
        uint64_t tx;
        uint64_t tx_drop;
    } stats __rte_cache_aligned;
};

struct ports_info {
        uint8_t num_ports;
        uint8_t id[RTE_MAX_ETHPORTS];	/* the virtual port id to physical port id */
        struct ether_addr mac[RTE_MAX_ETHPORTS];
        volatile struct {
        	uint64_t rx;
        	uint64_t tx;
        	uint64_t tx_drop;
        } stats[RTE_MAX_ETHPORTS] __rte_cache_aligned;
};

#endif