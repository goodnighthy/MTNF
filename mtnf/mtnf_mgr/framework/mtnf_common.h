#ifndef _MTNF_COMMON_H_
#define _MTNF_COMMON_H_

#include <stdint.h>

#include <rte_mbuf.h>

#define MAX_TENANTS 32

#define MBUFS_PER_PORT 2048
#define MBUFS_PER_TENANT 256

/* default period is 3 seconds */
#define TIMER_PERIOD 3

#define PACKET_READ_SIZE 32

/* define common names for structures */
#define PKTMBUF_POOL_NAME "mtnf_pktmbuf_pool"
#define MTNF_PORT_INFO "mtnf_port_info"
#define MTNF_TENANT_INFO "mtnf_tenant_info"

/* define a structure to describe a worker */
struct worker_info
{
	uint8_t id;
};

/* define a structure to describe a tenant */
struct tenant_info
{
	uint8_t id;
	struct tenant_buffer tenant_buffer;
	volatile struct {
        uint64_t rx;
        uint64_t tx;
        uint64_t tx_drop;
    } stats;
};

struct tenant_buffer
{
	struct rte_mbuf *buffer[MBUFS_PER_TENANT];
	uint16_t count;
}

#endif