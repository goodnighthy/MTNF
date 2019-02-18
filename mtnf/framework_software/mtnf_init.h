#ifndef _MTNF_INIT_H_
#define _MTNF_INIT_H_

#include <stdint.h>
#include <stdio.h>

#include "mtnf_mempool.h"
#include "mtnf_memzone.h"
#include "mtnf_port.h"
#include "mtnf_queue.h"

#include "mtnf_common.h"
#include "mtnf_args.h"
#include "mtnf_tenant.h"


/*************************External global variables***************************/
extern struct tenants_info *tenants;
extern struct ports_info *ports;
extern struct rte_mempool *pktmbuf_pool;
extern struct tenants_buffer *buffers;


/**********************************Functions**********************************/

/*
 * Function that initialize all data structures, memory mapping and global
 * variables.
 *
 * Input  : the number of arguments (following C conventions)
 *          an array of the arguments as strings
 * Output : an error code
 *
 */
int init(int argc, char *argv[]);

#endif