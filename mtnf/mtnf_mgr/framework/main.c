#include <stdint.h>
#include <stdio.h>

#include <rte_mbuf.h>
#include <rte_lcore.h>
#include <rte_launch.h>
#include <rte_log.h>

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
master_thread(void *arg) {

}

int
main(int argc, char *argv[]) {
	unsigned cur_lcore, worker_lcores;
	unsigned i;


}