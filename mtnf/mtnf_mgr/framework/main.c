#include <stdint.h>
#include <stdio.h>
#include <signal.h>

#include <rte_mbuf.h>
#include <rte_lcore.h>
#include <rte_launch.h>
#include <rte_log.h>

#include "mtnf_init.h"
#include "mtnf_stats.h"

#define RTE_LOGTYPE_MTNF          RTE_LOGTYPE_USER1
#define MAX_PKT_BURST 32
#define BUFFER_SIZE 128

static volatile bool keep_running = 1;

static void handle_signal(int sig);

static void
handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        keep_running = 0;
    }
}

static int
worker_thread(void *arg) {


}

static int
master_thread(void) {
	const uint64_t sleeptime;

	RTE_LOG(INFO, APP, "Core %d: Running master thread\n", rte_lcore_id());

	/* Initial pause so above printf is seen */
    sleep(3);

    /* Loop forever: sleep always returns 0 or <= param */
    while (keep_running && sleep(sleeptime) <= sleeptime) {
        mtnf_stats_display_all(sleeptime);
    }

    RTE_LOG(INFO, APP, "Core %d: Master thread done\n", rte_lcore_id());
}

int
main(int argc, char *argv[]) {
	unsigned cur_lcore, worker_lcores;
	unsigned i;

	/* init the system */
	if (init(argc, argv) < 0)
		return -1;
	RTE_LOG(INFO, APP, "Finished Process Init.\n");

	cur_lcore = rte_lcore_id();
    worker_lcores = rte_lcore_count() - 1;

    RTE_LOG(INFO, APP, "%d cores available in total\n", rte_lcore_count());
    RTE_LOG(INFO, APP, "%d cores available for worker\n", worker_lcores);
    RTE_LOG(INFO, APP, "%d cores available for master\n", 1);

	/* Listen for ^C and docker stop so we can exit gracefully */
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    /* Launch each worker thread on cores */
    for (i = 0; i < worker_lcores; i++) {
            struct worker_info *worker_info = calloc(1, sizeof(struct worker_info));
            worker_info->id = i;
            cur_lcore = rte_get_next_lcore(cur_lcore, 1, 1);
            if (rte_eal_remote_launch(worker_thread, (void*)worker_info,  cur_lcore) == -EBUSY) {
                    RTE_LOG(ERR, APP, "Core %d is already busy, can't use for worker %d\n", cur_lcore, worker_info->id);
                    return -1;
            }
    }

    /* Master thread handles statistics and NF management */
    master_thread();
    return 0;
}