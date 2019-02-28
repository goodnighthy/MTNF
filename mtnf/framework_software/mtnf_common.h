#ifndef _MTNF_COMMON_H_
#define _MTNF_COMMON_H_

#include <stdint.h>

#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_ip.h>

#define MAX_TENANTS 16
#define QUEUE_NUM 4

#define MBUFS_PER_PORT 2048
#define MBUFS_PER_QUEUE 2048
#define MBUFS_PER_TENANT 256

/* default period is 1 seconds */
#define TIMER_PERIOD 1

#define PACKET_READ_SIZE 32

/* define common names for structures */
#define PKTMBUF_POOL_NAME "mtnf_pktmbuf_pool"
#define PKTMBUF_T_BUFFER_NAME "mtnf_pktmbuf_t_buffer"
#define PKTMBUF_Q_BUFFER_NAME "mtnf_pktmbuf_q_buffer"
#define MTNF_QUEUE_NAME "mtnf_queue_name_%u"
#define MTNF_PORT_INFO "mtnf_port_info"
#define MTNF_TENANT_INFO "mtnf_tenant_info"
#define MTNF_TENANT_STATE "mtnf_tenant_state"

/* define a structure to describe a dispatcher */
struct dispatcher_info
{
    uint8_t ports_num;
};

/* define a structure to describe a worker */
struct worker_info
{
	uint8_t id;
};

struct queues_buffer
{
    struct rte_mbuf *buffer_slot[MBUFS_PER_QUEUE];
    uint16_t num;
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

static inline const char *
get_queue_name(unsigned id) {
        static char buffer[sizeof(MTNF_QUEUE_NAME) + 2];

        snprintf(buffer, sizeof(buffer) - 1, MTNF_QUEUE_NAME, id);
        return buffer;
}

static inline uint8_t
get_tenant_id(struct rte_mbuf *pkt) {
    struct ipv4_hdr *ipv4;

    ipv4 = (struct ipv4_hdr *)(rte_pktmbuf_mtod(pkt, uint8_t *) + sizeof(struct ether_hdr));

    return (ipv4->type_of_service) % MAX_TENANTS;
}

#endif