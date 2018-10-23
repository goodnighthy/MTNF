#include <stdint.h>

#include <nfp/me.h>
#include <nfp/mem_atomic.h>
#include <pif_common.h>
#include <pif_plugin.h>

//=============================================================================================================
/* max number of tenants supported */
#define MAX_TENANTS 16 
#define MAX_WORKERS 4

/* each entry represents the CUR_SLOT, NB_PKT, WID, MAX_NB */
__export __mem static uint32_t tenants[MAX_TENANTS][4] = {{0, 0, 0, 2000000},
														  {0, 0, 1, 2000000},
														  {0, 0, 2, 2000000},
														  {0, 0, 3, 2000000}}; 

/* each entry represents the CUR_SLOT, LOAD, LAST_LOAD, FLAG, TIMES, MAX_LOAD */
__export __mem static uint32_t workers[MAX_WORKERS][5] = {{0, 0, 0, 0, 10000000},
														  {0, 0, 0, 0, 10000000},
														  {0, 0, 0, 0, 10000000},
														  {0, 0, 0, 0, 10000000}}; 

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
	__xread uint32_t in_xfer_tid_cur_slot, in_xfer_nb_pkt, in_xfer_wid, in_xfer_max_nb;
	__xread uint32_t in_xfer_wid_cur_slot, in_xfer_load, in_xfer_last_load, in_xfer_times, in_xfer_max_load;
	__gpr uint32_t out_reg_nb_pkt, out_reg_load, out_reg_last_load, out_reg_times;
	__xwrite uint32_t out_xfer_tid_cur_slot, out_xfer_nb_pkt, out_xfer_wid;
	__xwrite uint32_t out_xfer_wid_cur_slot, out_xfer_load, out_xfer_last_load, out_xfer_times;
	uint32_t i, timestamp, timeslot, tid, min_load, min_worker;
	PIF_PLUGIN_ipv4_T *ipv4;

	timestamp = pif_plugin_meta_get__intrinsic_metadata__ingress_global_timestamp(headers);
	timeslot = (timestamp >> 30); //about 1.8s
	ipv4 = pif_plugin_hdr_get_ipv4(headers);
	tid = ipv4->diffserv;

//==============================================================================================================
	mem_read32(&in_xfer_tid_cur_slot, &tenants[tid][0], sizeof(uint32_t));
	mem_read32(&in_xfer_nb_pkt, &tenants[tid][1], sizeof(uint32_t));
	mem_read32(&in_xfer_wid, &tenants[tid][2], sizeof(uint32_t));
	mem_read32(&in_xfer_max_nb, &tenants[tid][3], sizeof(uint32_t));

	if (timeslot == in_xfer_tid_cur_slot) {
		/* normal tenant packet */
		out_reg_nb_pkt = in_xfer_nb_pkt;
		out_reg_nb_pkt += 1;
		if (out_reg_nb_pkt > in_xfer_max_nb) {
			return PIF_PLUGIN_RETURN_DROP;
		}
		out_xfer_nb_pkt = out_reg_nb_pkt;
		mem_write32(&out_xfer_nb_pkt, &tenants[tid][1], sizeof(uint32_t));
	} else {
		/* first tenant packet */
		out_xfer_tid_cur_slot = timeslot;
		out_xfer_nb_pkt = 1;
		mem_write32(&out_xfer_tid_cur_slot, &tenants[tid][0], sizeof(uint32_t));
		mem_write32(&out_xfer_nb_pkt, &tenants[tid][1], sizeof(uint32_t));

		/* update worker table */
		mem_read32(&in_xfer_wid_cur_slot, &workers[in_xfer_wid][0], sizeof(uint32_t));
		if (timeslot != in_xfer_wid_cur_slot) {
			/* first worker packet */
			mem_read32(&in_xfer_load, &workers[in_xfer_wid][1], sizeof(uint32_t));
			out_xfer_wid_cur_slot = timeslot;
			out_xfer_load = 0;
			out_xfer_last_load = in_xfer_load;
			mem_write32(&out_xfer_wid_cur_slot, &workers[in_xfer_wid][0], sizeof(uint32_t));
			mem_write32(&out_xfer_load, &workers[in_xfer_wid][1], sizeof(uint32_t));
			mem_write32(&out_xfer_last_load, &workers[in_xfer_wid][2], sizeof(uint32_t));
		}

		mem_read32(&in_xfer_last_load, &workers[in_xfer_wid][2], sizeof(uint32_t));
		mem_read32(&in_xfer_max_load, &workers[in_xfer_wid][4], sizeof(uint32_t));
		if (in_xfer_last_load < in_xfer_max_load) {
			out_xfer_times = 0;
			mem_write32(&out_xfer_times, &workers[in_xfer_wid][3], sizeof(uint32_t));
		} else {
			mem_read32(&in_xfer_times, &workers[in_xfer_wid][3], sizeof(uint32_t));
			out_reg_times = in_xfer_times;
			out_reg_times += 1;
			/* if (out_reg_times > threshold), report overload */
			out_xfer_times = out_reg_times;
			mem_write32(&out_xfer_times, &workers[in_xfer_wid][3], sizeof(uint32_t));

			min_load = 0xffffffff;
			for (i = 0; i < MAX_WORKERS; i++) {
				mem_read32(&in_xfer_last_load, &workers[i][2], sizeof(uint32_t));
				if (min_load > in_xfer_last_load) {
					min_load = in_xfer_last_load;
					min_worker = i;
				}
			}
			min_load = min_load + in_xfer_nb_pkt;
			mem_read32(&in_xfer_max_load, &workers[min_worker][4], sizeof(uint32_t));
			if (min_load < in_xfer_max_load) {
				out_xfer_wid = min_worker;
				mem_write32(&out_xfer_wid, &tenants[tid][2], sizeof(uint32_t));
				out_xfer_last_load = min_load;
				mem_write32(&out_xfer_last_load, &workers[min_worker][2], sizeof(uint32_t));
				mem_read32(&in_xfer_last_load, &workers[in_xfer_wid][2], sizeof(uint32_t));
				out_reg_last_load = in_xfer_last_load;
				out_reg_last_load -= in_xfer_nb_pkt;
				out_xfer_last_load = out_reg_last_load;
				mem_write32(&out_xfer_last_load, &workers[in_xfer_wid][2], sizeof(uint32_t));
			}
		}
	}
	/* update worker load */
	mem_read32(&in_xfer_load, &workers[in_xfer_wid][1], sizeof(uint32_t));
	out_reg_load = in_xfer_load;
	out_reg_load += 1;
	out_xfer_load = out_reg_load;
	mem_write32(&out_xfer_load, &workers[in_xfer_wid][1], sizeof(uint32_t));
	pif_plugin_meta_set__dispatch_metadata__worker_id(headers, in_xfer_wid);

	return PIF_PLUGIN_RETURN_FORWARD;
}