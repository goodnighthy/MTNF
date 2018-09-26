#include <stdint.h>
#include <stdio.h>

#include "mtnf_init.h"
#include "mtnf_common.h"

struct port_info *ports = NULL;
struct rte_mempool *pktmbuf_pool;
struct tenant_info *tenants;


int
init(int argc, char *argv[]) {
	int retval;

	/* init EAL, parsing EAL args */
    retval = rte_eal_init(argc, argv);
    if (retval < 0)
        return -1;
    argc -= retval;
    argv += retval;

    retval = mtnf_parse_args(int argc, char **argv);
    if (retval < 0)
    	return -1;

    /* set up array for tenant info */
    tenants = memzone_reserve(MTNF_TENANT_INFO, sizeof(*tenants), tenant_number);

    /* initialise mbuf pools */
    pktmbuf_pool = init_pktmbuf_pool(PKTMBUF_POOL_NAME, port_number * MBUFS_PER_PORT + tenant_number * MBUFS_PER_TENANT);

    /* init ports and set up ports info */
    ports = init_all_ports(MTNF_PORT_INFO, port_mask, pktmbuf_pool);

    return 0
}

