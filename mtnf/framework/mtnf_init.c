#include "mtnf_init.h"

struct tenants_info *tenants;
struct ports_info *ports;
struct rte_mempool *pktmbuf_pool;
struct tenants_buffer *buffers;

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
    pktmbuf_pool = init_pktmbuf_pool(PKTMBUF_POOL_NAME, ports->num_ports * MBUFS_PER_PORT + tenant_number * MBUFS_PER_TENANT);

    /* initialise mbuf buffers */
    buffers = init_pktmbuf_buffer(PKTMBUF_BUFFER_NAME, sizeof(*buffers), tenant_number);

    mtnf_states_init();

    /* init ports and set up ports info */
    retval = init_all_ports(port_mask, pktmbuf_pool);
    if (retval < 0)
        return -1;

    check_all_ports_link_status(port_mask);

    return 0;
}