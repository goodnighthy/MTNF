#ifndef _MTNF_MEMPOOL_H_
#define _MTNF_MEMPOOL_H_

#include <stdint.h>
#include <stdio.h>

#include <rte_mempool.h>
#include <rte_malloc.h>
#include <rte_mbuf.h>
#include <rte_common.h>
#include <rte_errno.h>

#define MBUF_POOL_CACHE_SIZE 256

/* init the mempool for pktmbuf */
struct rte_mempool *
init_pktmbuf_pool(const char *pktmbuf_pool_name, uint32_t num_mbufs);


void *
init_pktmbuf_buffer(const char *pktmbuf_buffer_name, uint32_t length_buf, uint32_t num_buf);

/* free a bulk of pktmbuf */
void 
pktmbuf_free_bulk(struct rte_mbuf *pktmbuf[], unsigned n);

#endif