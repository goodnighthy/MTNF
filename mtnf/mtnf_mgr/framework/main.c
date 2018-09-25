#include <stdint.h>
#include <stdio.h>

#include <rte_mbuf.h>
#include <rte_lcore.h>

#include "mtnf_args.h"
#include "mtnf_help.h"
#include "mtnf_mempool.h"
#include "mtnf_memzone.h"
#include "mtnf_port.h"

#define RTE_LOGTYPE_MTNF          RTE_LOGTYPE_USER1