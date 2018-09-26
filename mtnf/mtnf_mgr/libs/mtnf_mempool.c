#include <stdint.h>
#include <stdio.h>

#include <rte_mempool.h>
#include <rte_mbuf.h>

#include "mtnf_mempool.h"

/* init the mempool for pktmbuf */
struct rte_mempool *
init_pktmbuf_pool(const char *pktmbuf_pool_name,uint32_t num_mbufs) {
	struct rte_mempool *pktmbuf_pool;


    pktmbuf_pool = rte_pktmbuf_pool_create(pktmbuf_pool_name, num_mbufs,
                                        MBUF_POOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
                                        rte_socket_id());
    printf("init pktmbuf_pool %s on socket %d\n", pktmbuf_pool_name, rte_socket_id());

    if (pktmbuf_pool == NULL) {
        rte_exit(EXIT_FAILURE, "%s\n", rte_strerror(rte_errno));
    }

    return pktmbuf_pool;
}

/* free a bulk of pktmbuf */
void 
pktmbuf_free_bulk(struct rte_mbuf *pktmbuf[], unsigned n) {
    unsigned int i;

    for (i = 0; i < n; i++)
        rte_pktmbuf_free(pktmbuf[i]);
}