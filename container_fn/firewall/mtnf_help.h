#ifndef _MTNF_HELP_H_
#define _MTNF_HELP_H_

#include <inttypes.h>

#include <rte_mbuf.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>
#include <rte_branch_prediction.h>


#define IP_PROTOCOL_TCP 6
#define IP_PROTOCOL_UDP 17

/**
 * Flip the source and destination mac address of a packet
 */
int
mtnf_pkt_mac_addr_swap(struct rte_mbuf *pkt, unsigned dst_port);

/**
 * Return a pointer to the tcp/udp/ip header in the packet, or NULL if not a TCP packet
 */
struct tcp_hdr *
mtnf_pkt_tcp_hdr(struct rte_mbuf *pkt);

struct ether_hdr *
mtnf_pkt_ether_hdr(struct rte_mbuf *pkt);

struct udp_hdr *
mtnf_pkt_udp_hdr(struct rte_mbuf *pkt);

struct ipv4_hdr *
mtnf_pkt_ipv4_hdr(struct rte_mbuf *pkt);

void *
mtnf_pkt_payload(struct rte_mbuf *pkt, int *payload_size);

/**
 * Check the type of a packet. Return 1 if packet is of the specified type, else 0
 */
int
mtnf_pkt_is_tcp(struct rte_mbuf *pkt);

int
mtnf_pkt_is_udp(struct rte_mbuf *pkt);

int
mtnf_pkt_is_ipv4(struct rte_mbuf *pkt);

/**
 * Print out a packet or header.  Check to be sure DPDK doesn't already do any of these
 */
void
mtnf_pkt_print(struct rte_mbuf *pkt);

void
mtnf_pkt_print_tcp(struct tcp_hdr *hdr);

void
mtnf_pkt_print_udp(struct udp_hdr *hdr);

void
mtnf_pkt_print_ipv4(struct ipv4_hdr *hdr);

void
mtnf_pkt_print_ether(struct ether_hdr *hdr);

#endif