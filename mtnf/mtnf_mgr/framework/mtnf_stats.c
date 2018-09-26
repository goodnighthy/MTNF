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

/* display MAC of port */
static const char *
print_MAC(uint8_t port) {
    static const char err_address[] = "00:00:00:00:00:00";
    static char addresses[RTE_MAX_ETHPORTS][sizeof(err_address)];

    if (unlikely(port >= RTE_MAX_ETHPORTS))
        return err_address;
    if (unlikely(addresses[port][0] == '\0')) {
        struct ether_addr mac;
        rte_eth_macaddr_get(port, &mac);
        snprintf(addresses[port], sizeof(addresses[port]),
                        "%02x:%02x:%02x:%02x:%02x:%02x\n",
                        mac.addr_bytes[0], mac.addr_bytes[1],
                        mac.addr_bytes[2], mac.addr_bytes[3],
                        mac.addr_bytes[4], mac.addr_bytes[5]);
    }
    return addresses[port];
}

/* display the statistics of all ports */
void 
mtnf_stats_display_ports(unsigned difftime, struct ports_info *portsinfo) {
        unsigned i;
        /* Arrays to store last TX/RX count to calculate rate */
        static uint64_t rx_last[RTE_MAX_ETHPORTS];
        static uint64_t tx_last[RTE_MAX_ETHPORTS];
        static uint64_t tx_drop_last[RTE_MAX_ETHPORTS];

        printf("PORTS\n");
        printf("-----\n");
        for (i = 0; i < portsinfo->num_ports; i++)
                printf("Port %u: '%s'\t", (unsigned)portsinfo->id[i],
                                print_MAC(portsinfo->id[i]));
        printf("\n\n");
        for (i = 0; i < portsinfo->num_ports; i++) {
                printf("Port %u - rx: %9"PRIu64"  (%9"PRIu64" pps)\t"
                                "tx: %9"PRIu64"  (%9"PRIu64" pps)\t"
                                "tx_drop: %9"PRIu64"  (%9"PRIu64" pps)\n\n",
                                (unsigned)portsinfo->id[i],
                                portsinfo->stats[portsinfo->id[i]].rx,
                                (portsinfo->stats[portsinfo->id[i]].rx - rx_last[i])
                                        /difftime,
                                portsinfo->stats[portsinfo->id[i]].tx,
                                (portsinfo->stats[portsinfo->id[i]].tx - tx_last[i])
                                        /difftime,
                                portsinfo->stats[portsinfo->id[i]].tx_drop,
                                (portsinfo->stats[portsinfo->id[i]].tx_drop - tx_drop_last[i])
                                        /difftime);

                rx_last[i] = portsinfo->stats[portsinfo->id[i]].rx;
                tx_last[i] = portsinfo->stats[portsinfo->id[i]].tx;
                tx_drop_last[i] = portsinfo->stats[portsinfo->id[i]].tx_drop;
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