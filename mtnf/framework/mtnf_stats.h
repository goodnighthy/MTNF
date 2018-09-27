#ifndef _MTNF_STATS_H_
#define _MTNF_STATS_H_

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include <rte_ether.h>

#include "mtnf_common.h"
#include "mtnf_args.h"
#include "mtnf_init.h"

/* display the statistics of ports and tenants */
void
mtnf_stats_display_all(unsigned difftime, struct ports_info *portsinfo, struct tenants_info *tenantsinfo);

/* clear the statistics of all tenants */
void
mtnf_stats_clear_all_tenants(struct tenants_info *tenantsinfo);

/* clear the statistics of tenant id */
void
mtnf_stats_clear_tenant(uint16_t id, struct tenants_info *tenantsinfo);

#endif