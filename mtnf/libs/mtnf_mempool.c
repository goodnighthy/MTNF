#include "mtnf_mempool.h"

/* init the mempool for pktmbuf */
struct rte_mempool *
init_pktmbuf_pool(const char *pktmbuf_pool_name, uint32_t num_mbufs) {
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

/* init the membuffer for pktmbuf */
void *
init_pktmbuf_buffer(const char *pktmbuf_buffer_name, uint32_t length_buf, uint32_t num_bufs) {
    void *pktmbuf_buffer;

    pktmbuf_buffer = rte_calloc(pktmbuf_buffer_name, length_buf, num_bufs, 0);

    if (pktmbuf_buffer == NULL) {
        rte_exit(EXIT_FAILURE, "%s\n", rte_strerror(rte_errno));
    }

    return pktmbuf_buffer;
}

/* free a bulk of pktmbuf */
void 
pktmbuf_free_bulk(struct rte_mbuf *pktmbuf[], unsigned n) {
    unsigned int i;

    for (i = 0; i < n; i++)
        rte_pktmbuf_free(pktmbuf[i]);
}