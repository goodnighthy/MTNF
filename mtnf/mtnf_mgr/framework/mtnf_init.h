#ifndef _MTNF_INIT_H_
#define _MTNF_INIT_H_

#include "mtnf_args.h"
#include "mtnf_help.h"
#include "mtnf_mempool.h"
#include "mtnf_memzone.h"
#include "mtnf_port.h"


/*************************External global variables***************************/
/* postmask of the devices */
extern uint32_t port_mask;
extern uint32_t port_number;
extern uint32_t tenant_number;


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