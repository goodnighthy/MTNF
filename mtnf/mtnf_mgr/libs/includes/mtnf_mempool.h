#ifndef _MTNF_MEMPOOL_H_
#define _MTNF_MEMPOOL_H_

#include <stdint.h>
#include <rte_mempool.h>

/* init the mempool for pktmbuf */
void
init_pktmbuf_pool(const char* pktmbuf_pool_name, uint32_t num_mbufs);

/* free a bulk of pktmbuf */
void 
pktmbuf_free_bulk(struct rte_mbuf *pktmbuf[], unsigned n)

#endif