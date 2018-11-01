#include "mtnf_ids.h"

static void
mtnf_get_next(struct ids_statistics* stats) {
    int str_len, i, j, index;
    char* p;
    int* next;
    for (index = 0; index < IDS_RULE_NUM; index ++) {
        p = stats->str[index];
        next = stats->next[index];

        str_len = strlen(p);
        i = 0;
        j = -1;
        next[0] = -1;
        while (i < str_len - 1) {
            if (j == -1 || p[i] == p[j])
            {
                i ++;
                j ++;
                next[i] = j;
            }
            else
                j = next[j];
        }
    }
}

static bool
mtnf_kmp(struct ids_statistics* stats, char* str) {
    int index, i, j;
    int p_len, s_len = strlen(str);
    char* p;
    int* next;

    index = 0;
    for (index = 0; index < IDS_RULE_NUM; index ++) {
        p = stats->str[index];
        next = stats->next[index];
        i = 0;
        j = 0;
        p_len = strlen(p);
        while (i < s_len / 5 && j < p_len)
        {
            if (j == -1 || str[i] == p[j])  // P 的第一个字符不匹配或 S[i] == P[j]
            {
                i ++;
                j ++;
            }
            else
                j = next[j];  // 当前字符匹配失败，进行跳转
        }

        if (j == p_len)  // 匹配成功
            return true;
    }
    return false;
}

uint32_t
mtnf_ids_register(void) {
    return sizeof(struct ids_statistics);
}

void mtnf_ids_init(void *state) {
    struct ids_statistics *stats;

    stats = (struct ids_statistics *)state;

    int index, j;
    for (index = 0; index < IDS_RULE_NUM; index ++) {
        for (j = 0; j < 50; j ++) {
            stats->str[index][j] = 'a' + (j % 26);
        }
        mtnf_get_next(stats);
    }
}

uint16_t
mtnf_ids_handler(struct rte_mbuf *pkt[], uint16_t num, void *state) {
	struct ids_statistics *stats;
    struct udp_hdr *udp;
    struct tcp_hdr *tcp;
	uint16_t i, num_out;
    char* pkt_data;

	stats = (struct ids_statistics *)state;
	num_out = num;
	for (i = 0; i < num; i ++) {
        /* Check if we have a valid UDP packet */
        udp = mtnf_pkt_udp_hdr(pkt[i]);
        if (udp != NULL) {
            pkt_data = (char *)((uint8_t *) udp) + sizeof(struct udp_hdr);
            
            if (mtnf_kmp(stats, pkt_data)) {
                // if match
            } else {
                // not match
            };
            continue;
        }
        /* Check if we have a valid TCP packet */
        tcp = mtnf_pkt_tcp_hdr(pkt[i]);
        if (tcp != NULL) {
            pkt_data = (char *)((uint8_t *) tcp) + sizeof(struct tcp_hdr);

            if (mtnf_kmp(stats, pkt_data)) {
                // if match
            } else {
                // not match
            };
        }
	}

	return num_out;
}