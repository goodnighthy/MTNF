#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <rte_ethdev.h>

/* parse the portmask */
static int
parse_portmask(const char *portmask) {
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
        if (end == NULL || *end != '\0')
                return -1;

        return pm;
}