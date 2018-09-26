#include <stdint.h>
#include <stdio.h>

#include "mtnf_init.h"
#include "mtnf_common.h"

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
    ports = memzone_reserve(ports_info_name, sizeof(*ports), 1);

    retval = mtnf_parse_args(int argc, char **argv);
    if (retval < 0)
    	return -1; 

    /* initialise mbuf pools */
    pktmbuf_pool = init_pktmbuf_pool(PKTMBUF_POOL_NAME, ports->num_ports * MBUFS_PER_PORT + tenant_number * MBUFS_PER_TENANT);

    /* init ports and set up ports info */
    ports = init_all_ports(port_mask, pktmbuf_pool);

    return 0
}