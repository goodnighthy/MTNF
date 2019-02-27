#include "mtnf_init.h"

struct tenants_info *tenants;
struct ports_info *ports;
struct rte_mempool *pktmbuf_pool;
struct tenants_buffer *t_buffers;
struct queues_buffer *q_buffers;

int
init(int argc, char *argv[]) {
	int retval;

	/* init EAL, parsing EAL args */
    retval = rte_eal_init(argc, argv);
    if (retval < 0)
        return -1;
    argc -= retval;
    argv += retval;

    /* set up array for tenant info */
    tenants = memzone_reserve(MTNF_TENANT_INFO, sizeof(*tenants), MAX_TENANTS);

    /* set up ports info */
    ports = memzone_reserve(MTNF_PORT_INFO, sizeof(*ports), 1);

    retval = mtnf_parse_args(argc, argv);
    if (retval < 0)
    	return -1; 

    /* initialise mbuf pools */
    pktmbuf_pool = init_pktmbuf_pool(PKTMBUF_POOL_NAME, ports->num_ports * MBUFS_PER_PORT + QUEUE_NUM * MBUFS_PER_QUEUE + tenant_number * MBUFS_PER_TENANT);

    /* initialise mbuf buffers */
    t_buffers = init_pktmbuf_buffer(PKTMBUF_T_BUFFER_NAME, sizeof(*t_buffers), tenant_number);
    q_buffers = init_pktmbuf_buffer(PKTMBUF_Q_BUFFER_NAME, sizeof(*q_buffers), QUEUE_NUM);

    mtnf_states_init();

    /* init ports and set up ports info */
    retval = init_all_ports(port_mask, pktmbuf_pool);
    if (retval < 0)
        return -1;

    check_all_ports_link_status(port_mask);

    for (int i = 0; i < QUEUE_NUM; i ++) {
        queue_init(get_queue_name(i), MBUFS_PER_QUEUE);
    }

    return 0;
}