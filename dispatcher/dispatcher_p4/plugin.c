#include <stdint.h>

#include <nfp/me.h>
#include <nfp/mem_atomic.h>
#include <pif_common.h>
#include <pif_plugin.h>

//=============================================================================================================
/* max number of tenants supported */
#define MAX_TENANTS 16 

/* each entry represents the timeslot, number, token and port */
__export __mem static uint32_t tenants[MAX_TENANTS][4] = {{0, 0, 20000},
														  {0, 0, 20000},
														  {0, 0, 20000},
														  {0, 0, 20000}}; 

/* data structure for latency data per port */
struct tsopt_data {
    uint64_t max_latency;
    uint64_t min_latency;
    uint64_t count;
    uint64_t total_latency;
};

/* declare latency data with one extra slot for bad port#
 * this memory is exported so we can get to it from the host
 */
__export __mem struct tsopt_data tsopt_data;

//=============================================================================================================
int pif_plugin_primitive_parsetime(EXTRACTED_HEADERS_T *headers, MATCH_DATA_T *match_data) {
	PIF_PLUGIN_ipv4_T *ipv4;
    __xread struct tsopt_data in_xfer;
    __gpr struct tsopt_data out_reg;
    __xwrite struct tsopt_data out_xfer;
    uint32_t ctime, ptime;
    uint32_t latency;

	ipv4 = pif_plugin_hdr_get_ipv4(headers);
    ptime = ipv4->timestamp;

    mem_read32(&in_xfer, &tsopt_data, sizeof(in_xfer));

    out_reg = in_xfer;

	ctime = pif_plugin_meta_get__intrinsic_metadata__ingress_global_timestamp(headers);
    latency = ctime - ptime;


    if (latency > out_reg.max_latency)
        out_reg.max_latency = latency;

    if (latency < out_reg.min_latency || out_reg.min_latency == 0)
        out_reg.min_latency = latency;

    out_reg.count += 1;
    out_reg.total_latency += latency;

    out_xfer = out_reg;
    mem_write32(&out_xfer, &tsopt_data, sizeof(out_xfer));

    return PIF_PLUGIN_RETURN_FORWARD;
}

//=============================================================================================================
int pif_plugin_primitive_dispatch(EXTRACTED_HEADERS_T *headers, MATCH_DATA_T *match_data) {
	__xread uint32_t in_xfer_timeslot, in_xfer_number, in_xfer_token, in_xfer_port;
	__gpr uint32_t out_reg_number;
	__xwrite uint32_t out_xfer_timeslot, out_xfer_number;
	uint32_t timestamp, timeslot, tid;
	PIF_PLUGIN_ipv4_T *ipv4;

	timestamp = pif_plugin_meta_get__intrinsic_metadata__ingress_global_timestamp(headers);
	timeslot = (timestamp >> 21); //about 1.5m
	ipv4 = pif_plugin_hdr_get_ipv4(headers);
	tid = ipv4->diffserv;

	mem_read32(&in_xfer_timeslot, &tenants[tid][0], sizeof(uint32_t));
	mem_read32(&in_xfer_number, &tenants[tid][1], sizeof(uint32_t));
	mem_read32(&in_xfer_token, &tenants[tid][2], sizeof(uint32_t));
	out_reg_number = in_xfer_number;

	if (timeslot != in_xfer_timeslot) {
		out_xfer_timeslot = timeslot;
		out_xfer_number = 1;
		mem_write32(&out_xfer_timeslot, &tenants[tid][0], sizeof(uint32_t));
		mem_write32(&out_xfer_number, &tenants[tid][1], sizeof(uint32_t));
	} else {
		out_reg_number += 1;
		out_xfer_number = out_reg_number;
		mem_write32(&out_xfer_number, &tenants[tid][1], sizeof(uint32_t));
	}

//	if (out_reg_number > in_xfer_token) {
//		return PIF_PLUGIN_RETURN_DROP;
//	}

	return PIF_PLUGIN_RETURN_FORWARD;
}