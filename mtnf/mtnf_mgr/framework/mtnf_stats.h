#ifndef _MTNF_STATS_H_
#define _MTNF_STATS_H_

/* display the statistics of ports and tenants */
void
mtnf_stats_display_all(unsigned difftime, struct ports_info *portsinfo, struct tenant_info *tenantsinfo);

/* clear the statistics of all tenants */
void
mtnf_stats_clear_all_tenants(struct tenant_info *tenantsinfo);

/* clear the statistics of tenant id */
void
mtnf_stats_clear_tenant(uint16_t id, struct tenant_info *tenantsinfo);

#endif