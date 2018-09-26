#ifndef _MTNF_PORT_H_
#define _MTNF_PORT_H_

#include <getopt.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <rte_ethdev.h>

#include "mtnf_common.h"
#include "mtnf_init.h"

/********************************Global variables*********************************/
/* postmask of the devices */
uint32_t tenant_number = MAX_TENANTS;

/* time period of print statistics */
uint64_t timer_period = TIMER_PERIOD; 

/* parse the portmask */
int
mtnf_parse_args(int argc, char **argv);

#endif