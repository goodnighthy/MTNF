#include "mtnf_queue.h"

/* init a queue with queue_size packets  */
struct rte_ring *
queue_init(const char *queue_name, uint32_t queue_size) {
	struct rte_ring *queue;

	queue = rte_ring_create(queue_name, queue_size,
                                rte_socket_id(), RING_F_SC_DEQ);
    if (queue == NULL)
        rte_exit(EXIT_FAILURE, "Cannot init queue for %s\n", queue_name);

    return queue;
}

/* lookup queue through queue_name */
struct rte_ring *
queue_lookup(const char *queue_name) {
	struct rte_ring *queue;

	queue = rte_ring_lookup(queue_name);

	if (queue == NULL)
		rte_exit(EXIT_FAILURE, "Cannot get queue for %s\n", queue_name);

	return queue;
}