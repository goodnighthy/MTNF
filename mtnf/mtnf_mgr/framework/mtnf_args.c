#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <rte_ethdev.h>

#include "mtnf_args.h"

/********************************Global variables*********************************/
/* postmask of the devices */
uint32_t port_mask;
uint32_t port_number;

/* time period of print statistics */
uint64_t timer_period = 3; /* default period is 3 seconds */

/*******************************Internal functions*********************************/

/* display usage */
static void
mtnf_usage(const char *prgname)
{
        printf("%s [EAL options] -- -p PORTMASK [-q NQ]\n"
               "  -p PORTMASK: hexadecimal bitmask of ports to configure\n"
                   "  -T PERIOD: statistics will be refreshed each PERIOD seconds (0 to disable, 3 default)\n",
               prgname);
}

/* parse the portmask */
static int
mtnf_parse_portmask(const char *portmask) {
        char *end = NULL;
        uint32_t pm;

        if (portmask == NULL)
                return -1;

        /* convert parameter to a number and verify */
        pm = strtoul(portmask, &end, 16);
        if (pm == 0) {
                printf("WARNING: No ports are being used.\n");
                return 0;
        }
        if ((portmask[0] == '\0') || (end == NULL) || (*end != '\0'))
                return -1;

        /* loop through bits of the mask and mark ports */
        while (pm != 0) {
                if (pm & 0x01) { /* bit is set in mask, use port */
                        port_number++;
                }
                pm = (pm >> 1);
        }

        return pm;
}

/* parse the time period */
static int
mtnf_parse_timer_period(const char *q_arg)
{
        char *end = NULL;
        int n;

        /* parse number string */
        n = strtol(q_arg, &end, 10);
        if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
                return -1;
        if (n >= MAX_TIMER_PERIOD)
                return -1;

        return n;
}

/**********************************Interface***************************************/
/* Parse the argument given in the command line of the application */
static int
mtnf_parse_args(int argc, char **argv)
{
        int opt, ret, timer_secs;
        char **argvopt;
        int option_index;
        char *prgname = argv[0];

        static struct option lgopts[] = {
                {"port-mask",           required_argument,      NULL,   'p'},
                {"timer-period",        no_argument,            NULL,   't'},
        };

        argvopt = argv;

        while ((opt = getopt_long(argc, argvopt, "p:t:",
                                  lgopts, &option_index)) != EOF) {

                switch (opt) {
                /* portmask */
                case 'p':
                        port_mask = mtnf_parse_portmask(optarg);
                        if (port_mask == 0) {
                                printf("invalid portmask\n");
                                mtnf_usage(prgname);
                                return -1;
                        }
                        break;

                /* timer period */
                case 't':
                        timer_secs = mtnf_parse_timer_period(optarg);
                        if (timer_secs < 0) {
                                printf("invalid timer period\n");
                                mtnf_usage(prgname);
                                return -1;
                        }
                        timer_period = timer_secs;
                        break;

                default:
                        mtnf_usage(prgname);
                        return -1;
                }
        }

        if (optind >= 0)
                argv[optind-1] = prgname;

        ret = optind-1;
        optind = 1; /* reset getopt lib */
        return ret;
}