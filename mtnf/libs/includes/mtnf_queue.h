#ifndef _MTNF_QUEUE_H_
#define _MTNF_QUEUE_H_

#include <rte_ring.h>

/* init a queue with queue_size packets  */
struct rte_ring *
queue_init(const char *queue_name, uint32_t queue_size);

/* lookup queue through queue_name */
struct rte_ring *
queue_lookup(const char *queue_name);

#endif