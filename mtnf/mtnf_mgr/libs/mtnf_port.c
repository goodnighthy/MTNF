#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_memzone.h>
#include <rte_ether.h>

#include "mtnf_port.h"
#include "mtnf_memzone.h"

static const struct rte_eth_conf default_eth_conf = {
    .link_speeds = ETH_LINK_SPEED_AUTONEG, /* auto negotiate speed */
    .lpbk_mode = 0,
    .rxmode =
        {
            .mq_mode = ETH_MQ_RX_RSS,    /* Use RSS without DCB or VMDQ */
            .max_rx_pkt_len = 0,         /* valid only if jumbo is on */
            .split_hdr_size = 0,         /* valid only if HS is on */
            .header_split = 0,           /* Header Split off */
            .hw_ip_checksum = HW_RXCSUM, /* IP checksum offload */
            .hw_vlan_filter = 0,         /* VLAN filtering */
            .hw_vlan_strip = 0,          /* VLAN strip */
            .hw_vlan_extend = 0,         /* Extended VLAN */
            .jumbo_frame = 0,            /* Jumbo Frame support */
            .hw_strip_crc = 1,           /* CRC stripped by hardware */
        },
    .txmode =
        {
            .mq_mode = ETH_MQ_TX_NONE, /* Disable DCB and VMDQ */
        },
    .rx_adv_conf.rss_conf =
        {
        	.rss_key = rss_symmetric_key,
            .rss_hf = ETH_RSS_IP | ETH_RSS_UDP | ETH_RSS_TCP,
        },
    /* No flow director */
    .fdir_conf =
        {
            .mode = RTE_FDIR_MODE_NONE,
        },
    /* No interrupt */
    .intr_conf =
        {
            .lsc = 0,
        },
};

/************************Internal functions***************************/

/* Init a single port */
static int 
init_single_port(struct rte_mempool *mbuf_pool, uint8_t port_id) {
	const uint16_t rxRings = RX_QUEUE, txRings = TX_QUEUE;
    const uint16_t nb_rxd = RX_RING_SIZE;
    const uint16_t nb_txd = TX_RING_SIZE;
	struct rte_eth_conf port_conf = default_eth_conf;
	struct rte_eth_rxconf rxq_conf;
	struct rte_eth_txconf txq_conf;
	struct rte_eth_dev_info dev_info;

    int retval;
    uint16_t q;

    printf("Port %u init ... \n", (unsigned)port_id);
    printf("Port %u socket id %u ... \n", (unsigned)port_id, (unsigned)rte_eth_dev_socket_id(port_id));

    if (port_id >= rte_eth_dev_count()) {
    	rte_exit(EXIT_FAILURE, "Cannot init port %u since it is bigger than the total number\n", port_id);
    }

    rte_eth_dev_info_get(port_id, &dev_info);
    if (dev_info.tx_offload_capa & DEV_TX_OFFLOAD_MBUF_FAST_FREE)
        port_conf.txmode.offloads |=
            DEV_TX_OFFLOAD_MBUF_FAST_FREE;

    retval = rte_eth_dev_configure(port_id, rxRings, txRings, &port_conf);
    if (retval < 0)
		rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d, port=%u\n", retval, port_id);

    retval = rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &nb_rxd, &nb_txd);
	if (retval < 0)
		rte_exit(EXIT_FAILURE, "Cannot adjust number of descriptors: err=%d, port=%u\n", retval, port_id);

	rxq_conf = dev_info.default_rxconf;
	rxq_conf.offloads = port_conf.rxmode.offloads;
    for (q = 0; q < rxRings; q++) {
        retval = rte_eth_rx_queue_setup(port_id, q, nb_rxd,
                                        rte_eth_dev_socket_id(port_id),
                                        &rxq_conf, mbuf_pool);
        if (retval < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_rx_queue_setup:err=%d, port=%u\n", retval, port_id);
    }

    txconf = dev_info.default_txconf;
    txconf.txq_flags = ETH_TXQ_FLAGS_IGNORE;
    txconf.offloads = port_conf.txmode.offloads;
    for (q = 0; q < txRings; q++) {
        retval = rte_eth_tx_queue_setup(port_id, q, nb_txd,
                                        rte_eth_dev_socket_id(port_id),
                                        &txconf);
        if (retval < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup:err=%d, port=%u\n", retval, port_id);
    }

    rte_eth_promiscuous_enable(port_id);

    retval = rte_eth_dev_start(port_id);
    if (retval < 0)
        rte_exit(EXIT_FAILURE, "rte_eth_dev_start:err=%d, port=%u\n", retval, port_id);

    return 0;
}

static const char *
print_MAC(uint8_t port) {
    static const char err_address[] = "00:00:00:00:00:00";
    static char addresses[RTE_MAX_ETHPORTS][sizeof(err_address)];

    if (unlikely(port >= RTE_MAX_ETHPORTS))
        return err_address;
    if (unlikely(addresses[port][0] == '\0')) {
        struct ether_addr mac;
        rte_eth_macaddr_get(port, &mac);
        snprintf(addresses[port], sizeof(addresses[port]),
                        "%02x:%02x:%02x:%02x:%02x:%02x\n",
                        mac.addr_bytes[0], mac.addr_bytes[1],
                        mac.addr_bytes[2], mac.addr_bytes[3],
                        mac.addr_bytes[4], mac.addr_bytes[5]);
    }
    return addresses[port];
}

/**********************************Interface*************************************/

/* Init all ports */
struct ports_info *
init_all_ports(const char *ports_info_name, uint32_t port_mask, struct rte_mempool *mbuf_pool) {
	int retval;
	uint8_t port_id, total_ports;
    struct ports_info *local_ports_info;
    // const struct rte_memzone *mz_ports;

    // /* set up ports info */
    // mz_ports = rte_memzone_reserve(ports_info_name, sizeof(*local_ports_info),
    //                                 rte_socket_id(), NO_FLAGS);
    // if (mz_port == NULL)
    //     rte_exit(EXIT_FAILURE, "Cannot reserve memory zone for port information\n");
    // local_ports_info = mz_ports->addr;
    local_ports_info = memzone_reserve(ports_info_name, sizeof(*local_ports_info), 1);

    total_ports = rte_eth_dev_count();
    local_ports_info.num_ports = 0;
    for (port_id = 0; port_id < total_ports; port_id++) {
        /* skip ports that are not enabled */
        if ((portmask & (1 << port_id)) == 0) {
            printf("\nSkipping disabled port %d\n", port_id);
            continue;
        }
        /* init port */
        retval = init_single_port(mbuf_pool, port_id);
        if (retval != 0)
        	rte_exit(EXIT_FAILURE, "Cannot init port %u\n", port_id);

        local_ports_info.id[local_ports_info.num_ports] = port_id;
        rte_eth_macaddr_get(port_id, &local_ports_info.mac[local_ports_info.num_ports]);
        printf("Port %u MAC: %02"PRIx8" %02"PRIx8" %02"PRIx8
               " %02"PRIx8" %02"PRIx8" %02"PRIx8"\n",
        port_id,
        local_ports_info.mac[local_ports_info.num_ports].addr_bytes[0], 
        local_ports_info.mac[local_ports_info.num_ports].addr_bytes[1],
        local_ports_info.mac[local_ports_info.num_ports].addr_bytes[2], 
        local_ports_info.mac[local_ports_info.num_ports].addr_bytes[3],
        local_ports_info.mac[local_ports_info.num_ports].addr_bytes[4], 
        local_ports_info.mac[local_ports_info.num_ports].addr_bytes[5]);
        
        local_ports_info.ports[local_ports_info.num_ports].rx = 0;
        local_ports_info.ports[local_ports_info.num_ports].tx = 0;
        local_ports_info.ports[local_ports_info.num_ports].tx_drop = 0;
        local_ports_info.num_ports++;

    }
    return local_ports_info;

}

/* Check the link status of all ports in up to 9s, and print them finally */
void 
check_all_ports_link_status(uint32_t port_mask) {
	uint8_t portid, total_ports;
	uint8_t count, all_ports_up, print_flag = 0;
	struct rte_eth_link link;

	printf("\nChecking link status");
	fflush(stdout);
	for (count = 0; count <= MAX_CHECK_TIME; count++) {
		all_ports_up = 1;
		total_ports = rte_eth_dev_count();
		for (port_id = 0; port_id < total_ports; port_id++) {
			if ((port_mask & (1 << portid)) == 0)
				continue;
			memset(&link, 0, sizeof(link));
			rte_eth_link_get_nowait(portid, &link);
			/* print link status if flag set */
			if (print_flag == 1) {
				if (link.link_status)
					printf("Port%d Link Up. Speed %u Mbps - %s\n",portid, link.link_speed,
				(lin	k.link_duplex == ETH_LINK_FULL_DUPLEX) ? ("full-duplex") : ("half-duplex\n"));
				else
					printf("Port %d Link Down\n", portid);
				continue;
			}
			/* clear all_ports_up flag if any link down */
			if (link.link_status == ETH_LINK_DOWN) {
				all_ports_up = 0;
				break;
			}
		}
		/* after finally printing all link status, get out */
		if (print_flag == 1)
			break;

		if (all_ports_up == 0) {
			printf(".");
			fflush(stdout);
			rte_delay_ms(CHECK_INTERVAL);
		}

		/* set the print_flag if all ports up or timeout */
		if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
			print_flag = 1;
			printf("done\n");
		}
	}
}

/* display the statistics of all ports */
void 
mtnf_stats_display_ports(unsigned difftime, struct ports_info *portsinfo) {
        unsigned i;
        /* Arrays to store last TX/RX count to calculate rate */
        static uint64_t rx_last[RTE_MAX_ETHPORTS];
        static uint64_t tx_last[RTE_MAX_ETHPORTS];
        static uint64_t tx_drop_last[RTE_MAX_ETHPORTS];

        printf("PORTS\n");
        printf("-----\n");
        for (i = 0; i < portsinfo->num_ports; i++)
                printf("Port %u: '%s'\t", (unsigned)portsinfo->id[i],
                                print_MAC(portsinfo->id[i]));
        printf("\n\n");
        for (i = 0; i < portsinfo->num_ports; i++) {
                printf("Port %u - rx: %9"PRIu64"  (%9"PRIu64" pps)\t"
                                "tx: %9"PRIu64"  (%9"PRIu64" pps)\t"
                                "tx_drop: %9"PRIu64"  (%9"PRIu64" pps)\n\n",
                                (unsigned)portsinfo->id[i],
                                portsinfo->ports[portsinfo->id[i]].rx,
                                (portsinfo->ports[portsinfo->id[i]].rx - rx_last[i])
                                        /difftime,
                                portsinfo->ports[portsinfo->id[i]].tx,
                                (portsinfo->ports[portsinfo->id[i]].tx - tx_last[i])
                                        /difftime,
                                portsinfo->ports[portsinfo->id[i]].tx_drop,
                                (portsinfo->ports[portsinfo->id[i]].tx_drop - tx_drop_last[i])
                                        /difftime);

                rx_last[i] = portsinfo->ports[portsinfo->id[i]].rx;
                tx_last[i] = portsinfo->ports[portsinfo->id[i]].tx;
                tx_drop_last[i] = portsinfo->ports[portsinfo->id[i]].tx_drop;
        }
}