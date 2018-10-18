/*********************************************************************
 *                     openNetVM
 *              https://sdnfv.github.io
 *
 *   BSD LICENSE
 *
 *   Copyright(c)
 *            2015-2017 George Washington University
 *            2015-2017 University of California Riverside
 *            2016-2017 Hewlett Packard Enterprise Development LP
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * The name of the author may not be used to endorse or promote
 *       products derived from this software without specific prior
 *       written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * aesencrypt.c - Encrypt UDP packets using AES
 ********************************************************************/

#include "mtnf_aes_encrypt.h"

/* AES encryption parameters */
static BYTE key[1][32] = {
  {0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4}
};
static BYTE iv[1][16] = {
  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f}
};

/* register tenant state */
uint32_t
mtnf_aes_encrypt_register(void) {
    return sizeof(struct aes_encrypt_statistics);
}

/* init tenant state */
void
mtnf_aes_encrypt_init(void *state) {
	struct aes_encrypt_statistics *stats;
	stats = (struct aes_encrypt_statistics *)state;
    /* Initialise encryption engine. Key should be configurable. */
	aes_key_setup(key[0], stats->key_schedule, 256);
}

/* handle tenant packets */
uint16_t
mtnf_aes_encrypt_handler(struct rte_mbuf *pkt[], uint16_t num, void *state) {
	struct aes_encrypt_statistics *stats;
    struct udp_hdr *udp;
    struct tcp_hdr *tcp;
	uint16_t i, num_out, plen, hlen;
    uint8_t *pkt_data, *eth;
    BYTE tmp_data[10000];

	stats = (struct aes_encrypt_statistics *)state;
	num_out = 0;
	for (i = 0; i < num; i++) {
        /* Check if we have a valid UDP packet */
        udp = mtnf_pkt_udp_hdr(pkt[i]);
        if (udp != NULL) {
            /* Get at the payload */
            pkt_data = ((uint8_t *) udp) + sizeof(struct udp_hdr);
            /* Calculate length */
            eth = rte_pktmbuf_mtod(pkt[i], uint8_t *);
            hlen = pkt_data - eth;
            plen = pkt[i]->pkt_len - hlen;
            
            aes_encrypt_ctr(pkt_data, plen, tmp_data, stats->key_schedule, 256, iv[0]);
            num_out ++;
            continue;
        }
        /* Check if we have a valid TCP packet */
        tcp = mtnf_pkt_tcp_hdr(pkt[i]);
        if (tcp != NULL) {
            /* Get at the payload */
            pkt_data = ((uint8_t *) tcp) + sizeof(struct tcp_hdr);
            /* Calculate length */
            eth = rte_pktmbuf_mtod(pkt[i], uint8_t *);
            hlen = pkt_data - eth;
            plen = pkt[i]->pkt_len - hlen;

            aes_encrypt_ctr(pkt_data, plen, tmp_data, stats->key_schedule, 256, iv[0]);
            num_out ++;
        }
	}

	return num_out;
}

/*
 * This function displays stats. It uses ANSI terminal codes to clear
 * screen when called. It is called from a single non-master
 * thread in the server process, when the process is run with more
 * than one lcore enabled.
 */
static void
do_stats_display(struct rte_mbuf* pkt) {
        const char clr[] = { 27, '[', '2', 'J', '\0' };
        const char topLeft[] = { 27, '[', '1', ';', '1', 'H', '\0' };
        struct ipv4_hdr* ip;

        /* Clear screen and move to top left */
        printf("%s%s", clr, topLeft);

        printf("PACKETS\n");
        printf("-----\n");
        printf("Port : %d\n", pkt->port);
        printf("Size : %d\n", pkt->pkt_len);
        printf("\n\n");

        ip = mtnf_pkt_ipv4_hdr(pkt);
        if (ip != NULL) {
    		struct udp_hdr *udp;
            mtnf_pkt_print(pkt);
    		/* Check if we have a valid UDP packet */
    		udp = mtnf_pkt_udp_hdr(pkt);
            if (udp != NULL) {
                uint8_t *	pkt_data;
                pkt_data = ((uint8_t *) udp) + sizeof(struct udp_hdr);
                printf("Payload : %.32s\n", pkt_data);
            }
        } else {
                printf("No IP4 header found\n");
        }
}