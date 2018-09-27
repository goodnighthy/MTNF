#ifndef _MTNF_ARGS_H_
#define _MTNF_ARGS_H_

#include <getopt.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <rte_ethdev.h>

#include "mtnf_common.h"
#include "mtnf_init.h"


/********************************Global variables*********************************/
/* postmask of the devices */
extern uint32_t port_mask;

/* number of the tenants */
extern uint32_t tenant_number;

/* time period of print statistics */
extern uint64_t timer_period; 

/* parse the portmask */
int
mtnf_parse_args(int argc, char **argv);

#endif