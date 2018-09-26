#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "mtnf_port.h"

/*******************************Internal Function*********************************/

/*
 * Function clearing the terminal and moving back the cursor to the top left.
 *
 */
static void
mtnf_stats_clear_terminal(void) {
    const char clr[] = { 27, '[', '2', 'J', '\0' };
    const char topLeft[] = { 27, '[', '1', ';', '1', 'H', '\0' };

    fprintf(stats_out, "%s%s", clr, topLeft);
}

/*
 * Function displaying statistics for all tenants
 *
 */
static void
mtnf_stats_display_tenants(unsigned difftime, struct tenant_info *tenantsinfo) {
	unsigned i;
    /* Arrays to store last TX/RX count to calculate rate */
    static uint64_t rx_last[MAX_TENANTS];
    static uint64_t tx_last[MAX_TENANTS];
    static uint64_t tx_drop_last[MAX_TENANTS];

    printf("TENANTS\n");
    printf("-----\n");

    for (i = 0; i < tenant_number; i++) {
        printf("Tenant %u - rx: %9"PRIu64"  (%9"PRIu64" pps)\t"
	                        "tx: %9"PRIu64"  (%9"PRIu64" pps)\t"
	                        "tx_drop: %9"PRIu64"  (%9"PRIu64" pps)\n\n",
	                        i,
	                        tenantsinfo[i].stats.rx,
	                        (tenantsinfo[i].stats.rx - rx_last[i])
	                                /difftime,
	                        tenantsinfo[i].stats.tx,
	                        (tenantsinfo[i].stats.tx - tx_last[i])
	                                /difftime,
	                         tenantsinfo[i].stats.tx_drop,
	                        (tenantsinfo[i].stats.tx_drop - tx_drop_last[i])
	                                /difftime);

        rx_last[i] = tenantsinfo[i].stats.rx;
        tx_last[i] = tenantsinfo[i].stats.tx;
        tx_drop_last[i] = tenantsinfo[i].stats.tx_drop;
    }
}

/*******************************Interfaces***************************************/

/* display the statistics of ports and tenants */
void
mtnf_stats_display_all(unsigned difftime, struct ports_info *portsinfo, struct tenant_info *tenantsinfo) {
    mtnf_stats_clear_terminal();

    mtnf_stats_display_ports(difftime, portsinfo);
    mtnf_stats_display_nfs(difftime, tenantsinfo);
}

/* clear the statistics of all tenants */
void
mtnf_stats_clear_all_tenants(struct tenant_info *tenantsinfo) {
    unsigned i;

    for (i = 0; i < MAX_TENANTS; i++) {
        tenantsinfo[i].stats.rx = 0;
        tenantsinfo[i].stats.tx = 0;
        tenantsinfo[i].stats.tx_drop = 0;
    }
}

/* clear the statistics of tenant id */
void
mtnf_stats_clear_tenant(uint16_t id, struct tenant_info *tenantsinfo) {
    tenantsinfo[id].stats.rx = 0;
    tenantsinfo[id].stats.tx = 0;
    tenantsinfo[id].stats.tx_drop = 0;
}