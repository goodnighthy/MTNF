#include "mtnf_args.h"

/*******************************Internal functions*********************************/

/* display usage */
static void
mtnf_usage(const char *prgname) {
        printf("%s [EAL options] -- -p PORTMASK [-t TENANT_NUMBER] [-T TIMER_PERIOD]\n"
            "  -p PORTMASK: hexadecimal bitmask of ports to configure\n"
            "  -t TENANT_NUMBER: define the number of tenants supported\n"
            "  -T PERIOD: statistics will be refreshed each PERIOD seconds (0 to disable, 3 default)\n",
            prgname);
}

/* parse the portmask */
static int
mtnf_parse_portmask(const char *portmask) {
    char *end = NULL;
    uint32_t pm;
    uint8_t count = 0;

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
            if (count >= rte_eth_dev_count())
                printf("WARNING: requested port %u not present"
                    " - ignoring\n", (unsigned)count);
            else {
                ports->id[ports->num_ports] = count;
                rte_eth_macaddr_get(count, &ports->mac[ports->num_ports++]);
            }
        }
        pm = (pm >> 1);
        count++;
    }

    return pm;
}

/* parse the tenant number */
static int
mtnf_parse_tenant_number(const char *tenant_number) {
    char *end = NULL;
    uint32_t tn;

    if (tenant_number == NULL)
        return -1;

    /* convert parameter to a number and verify */
    tn = strtoul(tenant_number, &end, 10);
    if (tn == 0) {
        printf("WARNING: No ports are being used.\n");
        return 0;
    }
    if ((tenant_number[0] == '\0') || (end == NULL) || (*end != '\0'))
        return -1;

    return tn;
}

/* parse the time period */
static int
mtnf_parse_timer_period(const char *timer_period) {
    char *end = NULL;
    uint32_t tp;

    /* parse number string */
    tp = strtol(timer_period, &end, 10);
    if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
        return -1;

    return tp;
}

/**********************************Interface***************************************/
/* Parse the argument given in the command line of the application */
static int
mtnf_parse_args(int argc, char **argv) {
    int opt, ret, tenant_count, timer_secs;
    char **argvopt;
    int option_index;
    char *prgname = argv[0];

    static struct option lgopts[] = {
        {"port-mask",           required_argument,      NULL,   'p'},
        {"tenant-number",       no_argument,            NULL,   't'},
        {"timer-period",        no_argument,            NULL,   'T'},
    };

    argvopt = argv;

    while ((opt = getopt_long(argc, argvopt, "p:t:T:",
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

        /* tenant number */
        case 't':
            tenant_count = mtnf_parse_tenant_number(optarg);
            if (tenant_count < 0) {
                printf("invalid tenant number\n");
                mtnf_usage(prgname);
                return -1;
            }
            tenant_number = tenant_count;
            break;

        /* timer period */
        case 'T':
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