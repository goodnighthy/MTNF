#include "mtnf_nat.h"

static uint16_t shift_8 = 1UL << 8;
static uint32_t shift_16 = 1UL << 16;
static uint64_t shift_32 = 1UL << 32;

/* register tenant state */
uint32_t
mtnf_nat_register(void) {
	return sizeof(struct nat_statistics);
}

/* init tenant state */
void
mtnf_nat_init(void *state) {
	struct nat_statistics *stats;

	stats = (struct nat_statistics *)state;

    memset(flowkey_map, 0, sizeof(struct flow_key) * BIG_PRIME * BUCKET_SIZE);
    memset(natentry_map, 0, sizeof(struct nat_entry) * BIG_PRIME * BUCKET_SIZE);
    memset(bucket_cnt, 0, sizeof(uint16_t) * BIG_PRIME);
    memset(port_used, 0, sizeof(bool) * 65536);
}

/* no use of rtr_be_to_cpu_16 because have to maintain the origin data */
static void fill_fkey(struct ipv4_hdr *ipv4, struct flow_key *f_key) {
    struct tcp_hdr *tcp_hdr;
    struct udp_hdr *udp_hdr;
    f_key.proto = ipv4->next_proto_id;
    f_key.src_ip = ipv4->src_addr;
    f_key.dst_ip = ipv4->dst_addr;
    if (f_key.proto == IP_PROTOCOL_TCP) {
        tcp_hdr = (struct tcp_hdr *)((uint8_t*)ipv4 + sizeof(struct ipv4_hdr));
        key->port_src = tcp_hdr->src_port;
        key->port_dst = tcp_hdr->dst_port;
    } else if (f_key.proto == IP_PROTOCOL_UDP) {
        udp_hdr = (struct udp_hdr *)((uint8_t*)ipv4 + sizeof(struct ipv4_hdr));
        key->port_src = udp_hdr->src_port;
        key->port_dst = udp_hdr->dst_port;
    } else {
        f_key.src_port = 0;
        f_key.dst_port = 0;
    }
}

static uint32_t hash_flowkey(struct flow_key *key) {
    uint64_t hash_val = 0;
    hash_val = key->src_ip % BIG_PRIME;
    hash_val = (hash_val * shift_32 + key->dst_ip) % BIG_PRIME;
    hash_val = (hash_val * shift_16 + key->src_port) % BIG_PRIME;
    hash_val = (hash_val * shift_16 + key->dst_port) % BIG_PRIME;
    hash_val = (hash_val * shift_8 + key->proto) % BIG_PRIME;

    return (uint32_t)hash_val;
}

static uint32_t hash_natentry(struct nat_entry *key) {
    uint64_t hash_val = 0;
    hash_val = (key->new_ip * shift_16 + key->new_port) % BIG_PRIME;

    return (uint32_t)hash_val;
}

static void update_hdr(struct ipv4_hdr *hdr, struct nat_entry *nat) {
    struct tcp_hdr *tcp_hdr;
    struct udp_hdr *udp_hdr;
    if (ipv4->next_proto_id == IP_PROTOCOL_TCP) {
        tcp_hdr = (struct tcp_hdr *)((uint8_t*)ipv4 + sizeof(struct ipv4_hdr));
        ipv4->src_addr = nat->new_ip;
        tcp_hdr->port_src = nat->new_port;
    } else if (f_key.proto == IP_PROTOCOL_UDP) {
        udp_hdr = (struct udp_hdr *)((uint8_t*)ipv4 + sizeof(struct ipv4_hdr));
        ipv4->src_addr = nat->new_ip;
        udp_hdr->port_src = nat->new_port;
    } else {
        // it shouldn't happen
    }
}

/*  since our nf doesn't have further processing 
    here reverse the ip and port to simulate the opposite nat */
static void ipv4_hdr_reverse(struct ipv4_hdr *hdr) {
    uint32_t tmp_ip;
    uint16_t tmp_port;
    struct tcp_hdr *tcp_hdr;
    struct udp_hdr *udp_hdr;
    tmp_ip = ipv4->src_addr;
    ipv4->src_addr = ipv4->dst_addr;
    ipv4->dst_addr = tmp_ip;
    if (ipv4->next_proto_id == IP_PROTOCOL_TCP) {
        tcp_hdr = (struct tcp_hdr *)((uint8_t*)ipv4 + sizeof(struct ipv4_hdr));
        tmp_port = tcp_hdr->port_src;
        tcp_hdr->port_src = tcp_hdr->port_dst;
        tcp_hdr->port_dst = tmp_port;
    } else if (f_key.proto == IP_PROTOCOL_UDP) {
        udp_hdr = (struct udp_hdr *)((uint8_t*)ipv4 + sizeof(struct ipv4_hdr));
        tmp_port = udp_hdr->port_src;
        udp_hdr->port_src = udp_hdr->port_dst;
        udp_hdr->port_dst = tmp_port;
    } else {
        // it shouldn't happen
    }
}

static void fkey_reverse(struct flow_key *f_key) {
    uint32_t tmp_ip;
    uint16_t tmp_port;
    tmp_ip = f_key->src_ip;
    f_key->src_ip = f_key->dst_ip;
    f_key->dst_ip = tmp_ip;
    tmp_port = f_key->src_port;
    f_key->src_port = f_key->dst_port;
    f_key->dst_port = tmp_port;
}

static struct nat_entry* get_natentry(void *state, struct flow_key* fkey) {
    struct nat_statistics stats = (struct nat_statistics *)state;
    uint32_t hash_index = hash_flowkey(fkey), bucket_index;
    struct flow_key *tmp_key;

    /* find nat entry */
    for (bucket_index = 0; bucket_index < bucket_cnt[hash_index]; bucket_index ++) {
        tmp_key = &(stats->flowkey_map[hash_index][bucket_index]);
        if (tmp_key->src_ip == f_key.src_ip && tmp_key->dst_ip == f_key.dst_ip \
            tmp_key->src_port == f_key.src_port && tmp_key->dst_port == f_key.dst_port \
            tmp_key->proto == f_key.proto) {
                return &(natentry_map[hash_index][bucket_index]);
        }
    }
    return NULL;
}

/* handle tenant packets */
uint16_t
mtnf_nat_handler(struct rte_mbuf *pkt[], uint16_t num, void *state) {
	struct nat_statistics *stats;
	uint16_t i, num_out, len;
	struct ipv4_hdr* ipv4;
    struct flow_key f_key;
    uint32_t hash_index, bucket_index;
    struct nat_entry *fwd_entry, *bwd_entry;
    bool found_nat;
    srand((unsigned)time(NULL));

    // new ip should depend on cluster
    // here just say 1, like what S6 did
    uint16_t new_ip = 1;

	stats = (struct nat_statistics *)state;
	num_out = num;
	uint64_t sum, tmpi;
	for (i = 0; i < num; i++) {
		ipv4 = mtnf_pkt_ipv4_hdr(pkt[i]);

        /* gen flowkey */
        fill_fkey(ipv4, &f_key);

        hash_index = hash_flowkey(&f_key);
        found_nat = false;

        /* nat process */
        fwd_entry = get_natentry(stats, &f_key);

        /* found */
        if (fwd_entry != NULL) {
            fkey_reverse(&f_key);
            bwd_entry = get_natentry(&f_key);
            found_nat = true;
        } else {
            /* if bucket is full, do nothing */
            bucket_index = stats->bucket_cnt[hash_index];
            if (bucket_index == BUCKET_SIZE) {
                found_nat = false;                
            } else { /* create a new entry */
                /* try to pick a random port for 10 times */
                /* like what S6 did */
                uint8_t trail = 0;
                uint16_t new_port = 0;

                for (trail = 0; trail < 10; trail ++) {
                    /* new port should be from 1 to 65535 */
                    new_port = (rand() * 3 + 17) % 65535;
                    /* port 0 is reserved, so if 0 then try again*/
                    if (new_port != 0 && port_used[new_port] == false) {
                        port_used[new_port] = true;
                        break;
                    } else
                        new_port = 0;
                }

                /* add forward flow key and nat entry */
                stats->flowkey_map[hash_index][bucket_index].src_ip = f_key.src_ip;
                stats->flowkey_map[hash_index][bucket_index].dst_ip = f_key.dst_ip;
                stats->flowkey_map[hash_index][bucket_index].src_port = f_key.src_port;
                stats->flowkey_map[hash_index][bucket_index].dst_port = f_key.dst_port;
                stats->flowkey_map[hash_index][bucket_index].proto = f_key.proto;
                stats->natentry_map[hash_index][bucket_index].new_ip = new_ip;
                stats->natentry_map[hash_index][bucket_index].new_port = new_port;
                stats->natentry_map[hash_index][bucket_index].is_forward = true;
                fwd_entry = &(stats->natentry_map[hash_index][bucket_index]);

                /* add backward flow key and nat entry */
                uint32_t tmp_ip, last_hashindex = hash_index;
                uint16_t tmp_port;
                tmp_ip = new_ip;
                tmp_port = new_port;
                new_ip = f_key.src_ip;
                new_port = f_key.src_port;
                fkey_reverse(f_key);
                f_key.dst_ip = tmp_ip;
                f_key.dst_port = tmp_port;
                hash_index = hash_flowkey(&f_key);
                bucket_index = stats->bucket_cnt[hash_index];
                if (bucket_index < BUCKET_SIZE) { /* bucket not full */
                    stats->flowkey_map[hash_index][bucket_index].src_ip = f_key.src_ip;
                    stats->flowkey_map[hash_index][bucket_index].dst_ip = f_key.dst_ip;
                    stats->flowkey_map[hash_index][bucket_index].src_port = f_key.src_port;
                    stats->flowkey_map[hash_index][bucket_index].dst_port = f_key.dst_port;
                    stats->flowkey_map[hash_index][bucket_index].proto = f_key.proto;
                    stats->natentry_map[hash_index][bucket_index].new_ip = new_ip;
                    stats->natentry_map[hash_index][bucket_index].new_port = new_port;
                    stats->natentry_map[hash_index][bucket_index].is_forward = false;
                    bwd_entry = &(stats->natentry_map[hash_index][bucket_index]);
                    stats->bucket_cnt[hash_index] ++;
                    stats->bucket_cnt[last_hashindex] ++;
                    found_nat = true;
                } else {
                    found_nat = false;
                    // do nothing
                }
            }
        }

        if (found_nat) {
            update_hdr(ipv4, fwd_entry);
            simulate_reverse(ipv4);
            update_hdr(ipv4, bwd_entry);
        } else {
            simulate_reverse(ipv4);
        }
	}

	return num_out;
}