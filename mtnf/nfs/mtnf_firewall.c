#include "mtnf_firewall.h"

#define PASS 1
#define DROP 0

static uint16_t shift_8 = 1UL << 8;
static uint32_t shift_16 = 1UL << 16;
static uint64_t shift_32 = 1UL << 32;

struct ipv4_5tuple {
    uint32_t ip_dst;
    uint32_t ip_src;
    uint16_t port_dst;
    uint16_t port_src;
    uint8_t  proto;
} __attribute__((__packed__));

struct ipv4_firewall_hash_entry {
    struct ipv4_5tuple key;
    uint8_t action;
};


static uint32_t rule_number = 4;
static struct ipv4_firewall_hash_entry ipv4_firewall_hash_entry_array[] = {
        {{50463234,        16885952,         9,9,IPPROTO_UDP}, PASS},
        {{IPv4(192,168,1,1), IPv4(192,168,0,1), 1234, 5678, IPPROTO_TCP}, DROP},
        {{IPv4(111,0,0,0), IPv4(100,30,0,1),  101, 11, IPPROTO_TCP}, PASS},
        {{IPv4(211,0,0,0), IPv4(200,40,0,1),  102, 12, IPPROTO_TCP}, PASS},
};

static inline void
firewall_fill_ipv4_5tuple_key(struct ipv4_5tuple *key, void *ipv4_hdr) {
    struct tcp_hdr *tcp_hdr;
    struct udp_hdr *udp_hdr;

    memset(key, 0, sizeof(struct ipv4_5tuple));
    key->proto  = ((struct ipv4_hdr *)ipv4_hdr)->next_proto_id;
    key->ip_src = ((struct ipv4_hdr *)ipv4_hdr)->src_addr;
    key->ip_dst = ((struct ipv4_hdr *)ipv4_hdr)->dst_addr;

    if (key->proto == IP_PROTOCOL_TCP) {
        tcp_hdr = (struct tcp_hdr *)((uint8_t*)ipv4_hdr + sizeof(struct ipv4_hdr));
        key->port_src = tcp_hdr->src_port;
        key->port_dst = tcp_hdr->dst_port;
    } else if (key->proto == IP_PROTOCOL_UDP) {
        udp_hdr = (struct udp_hdr *)((uint8_t*)ipv4_hdr + sizeof(struct ipv4_hdr));
        key->port_src = udp_hdr->src_port;
        key->port_dst = udp_hdr->dst_port;
    } else {
        key->port_src = 0;
        key->port_dst = 0;
    }
}

/* calc hash value */
static uint32_t
mtnf_hash_val(struct ipv4_5tuple* tmp_turple) {
    printf("calculating: ip_dst: %u, ip_src: %u, port_dst; %u, port_src: %u, proto: %u \n", \
    tmp_turple->ip_dst, tmp_turple->ip_src, tmp_turple->port_dst, tmp_turple->port_src, tmp_turple->proto);
    uint64_t ret = 0;
    ret = tmp_turple->ip_dst % BIG_PRIME;
    ret = (ret * shift_32 + tmp_turple->ip_src) % BIG_PRIME;
    ret = (ret * shift_16 + tmp_turple->port_dst) % BIG_PRIME;
    ret = (ret * shift_16 + tmp_turple->port_src) % BIG_PRIME;
    ret = (ret * shift_8 + tmp_turple->proto) % BIG_PRIME;
    return (uint32_t)ret;
}

/* hash set insert */
static void
mtnf_hash_insert(struct firewall_statistics* stats, struct ipv4_firewall_hash_entry* entry) {
    uint32_t index = mtnf_hash_val(&entry->key);
    printf("inserting index: %u\n", index);
    struct hash_node* ptr = &(stats->hash_map[index]);
    ptr->is_valid = true;
    ptr->ip_src = entry->key.ip_src;
    ptr->ip_dst = entry->key.ip_dst;
    ptr->port_src = entry->key.port_src;
    ptr->port_dst = entry->key.port_dst;
    ptr->proto = entry->key.proto;
    ptr->action = entry->action;
}

/* hash set find */
static int
mtnf_hash_lookup(struct firewall_statistics* stats, struct ipv4_5tuple* key) {
    uint32_t index = mtnf_hash_val(key);
    printf("detecting index of: %u\n", index);
    bool found = false;
    uint64_t action;
    struct hash_node ptr = stats->hash_map[index];

    if (ptr.ip_src == key->ip_src && ptr.ip_dst == key->ip_dst && \
    ptr.port_src == key->port_src && ptr.port_dst == key->port_dst && \
    ptr.proto == key->proto && ptr.is_valid) {
        action = ptr.action;
        found = true;

    }

    // logic here could be modified
    if (found)
        return action;
    else
        return PASS;
}

/* register tenant state */
uint32_t
mtnf_firewall_register(void) {
	return sizeof(struct firewall_statistics);
}

/* init tenant state */
void
mtnf_firewall_init(void *state) {
	struct firewall_statistics *stats;

	stats = (struct firewall_statistics *)state;

    uint32_t i;
    for (i = 0; i < BIG_PRIME; i ++) {
        stats->hash_map[i].is_valid = false;
    }

    for (i = 0; i < rule_number; i ++) {
        mtnf_hash_insert(stats, &ipv4_firewall_hash_entry_array[i]);
    }
}

/* handle tenant packets */
uint16_t
mtnf_firewall_handler(struct rte_mbuf *pkt[], uint16_t num, void *state) {
	struct firewall_statistics *stats;
    struct ipv4_hdr* ipv4hdr;
    struct ipv4_5tuple key;
	uint16_t i, num_out;
    uint8_t ret;

	stats = (struct firewall_statistics *)state;
	num_out = 0;
	for (i = 0; i < num; i++) {
//        mtnf_pkt_print_tcp(pkt[i]);
        printf("processing: %d\n", (int)i);
        ipv4hdr = mtnf_pkt_ipv4_hdr(pkt[i]);
        firewall_fill_ipv4_5tuple_key(&key, ipv4hdr);
        ret = mtnf_hash_lookup(stats, &key);
        if (ret == PASS)
            num_out++;
	}

	return num_out;
}