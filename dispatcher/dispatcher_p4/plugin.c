#include <stdint.h>

#include <nfp/me.h>
#include <nfp/mem_atomic.h>
#include <pif_common.h>
#include <pif_plugin.h>

//=============================================================================================================
/* max number of tenants supported */
#define MAX_TENANTS 16 

/* each entry represents the timeslot, number, token and port */
__export __mem static uint32_t tenants[MAX_TENANTS][4] = {{0, 0, 20000, 0},
														  {0, 0, 20000, 1},
														  {0, 0, 20000, 2},
														  {0, 0, 20000, 3}}; 

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
	mem_read32(&in_xfer_port, &tenants[tid][3], sizeof(uint32_t));
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

	if (out_reg_number > in_xfer_token) {
		return PIF_PLUGIN_RETURN_DROP;
	}

	pif_plugin_meta_set__standard_metadata__egress_spec(headers, in_xfer_port);
	
	return PIF_PLUGIN_RETURN_FORWARD;
}
