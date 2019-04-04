#!/usr/bin/env python
# coding=utf-8

import numpy as np
from scapy.utils import PcapReader, PcapWriter
from scapy.all import *
import os
import math
import sys
import json
import random

ETH_TYPE_IPv4 = 0x800
IP_MAX = 0xFFFFFFFF

class TrafficFileGenerator:

    pcap_filename = ""
    ip_hc_filename = ""
    ip_fake_ttl_filename = ""
    src_hosts = {}
    src_hosts_with_fake_ttl = {}
    counter = 0

    def __init__(self, pcap_filename=None):
        if pcap_filename is not None:
            # validate filename
            if not pcap_filename.endswith(".pcap"):
                print("Invalid PCAP filename! Exiting...")
                return
            self.pcap_filename = pcap_filename
            self.ip_hc_filename = str.join(".", pcap_filename.split(".")[:-1]) + "_ip_hc.txt"
            self.ip_fake_ttl_filename = str.join(".", pcap_filename.split(".")[:-1]) + "_fake_ttl.txt"

    def __ttl2hc(self, ttl):
        initial_ttl = 0
        if ttl >= 0 and ttl <= 29:
            initial_ttl =  30
        elif ttl >= 30 and ttl <= 31:
            initial_ttl =  32
        elif ttl >= 32 and ttl <= 59:
            initial_ttl =  60
        elif ttl >= 60 and ttl <= 63:
            initial_ttl =  64
        elif ttl >= 64 and ttl <= 127:
            initial_ttl =  127
        else:
            return None
        return initial_ttl - ttl

    def __gen_fake_ttl(self, ttl):
        if ttl >= 0 and ttl <= 127:
            return 127 - ttl
        else:
            return None

    def __ip_str2int(self, ip_str):
        str_arr = ip_str.split(".")
        ret = 0
        for i in range(4):
            ret = ret + int(str_arr[i]) * math.pow(2, 24 - 8 * i)
        return int(ret)

    def __ip_int2str(self, ip_int):
        return "%u.%u.%u.%u" % ((ip_int >> 24) & 0xff,
            (ip_int >> 16) & 0xff, (ip_int >>  8) & 0xff, ip_int & 0xff)

    def extract_ip2hc_table(self):
        self.counter = 0

        print("Reading pcap file: " + self.pcap_filename)
        # src_hosts is a dict

        pcap_reader = PcapReader(self.pcap_filename)

        while True:
            pkt = pcap_reader.read_packet()
            if pkt is None:
                break
            if not pkt.haslayer('Ether') or not pkt.haslayer('IP'):
                continue
            self.counter += 1
            # progress indicator
            if self.counter % 10000 == 0:
                print(str(self.counter) + "packets have been processed")

            # ipv4 packets
            ip_int = self.__ip_str2int(pkt['IP'].src)
            if ip_int in self.src_hosts:
                continue
            # unrecorded source host
            hc = self.__ttl2hc(pkt['IP'].ttl)
            fake_ttl = self.__gen_fake_ttl(pkt['IP'].ttl)
            if hc is not None and fake_ttl is not None:
                self.src_hosts[ip_int] = hc
                self.src_hosts_with_fake_ttl[ip_int] = fake_ttl

        pcap_reader.close()
        print(str(len(self.src_hosts)) + " source hosts extracted")

        print("Writing ip,hc dict into " + self.ip_hc_filename + "...")
        with open(self.ip_hc_filename, "w") as f:
            json.dump(self.src_hosts, f)
            f.close()

    def restore_from_file(self):
        print("Restoring ip,hc dict from " + self.ip_hc_filename + "...")
        with open(self.ip_hc_filename, "r") as f:
            self.src_hosts = json.load(f)
            f.close()

    def modify_traffic(self, output_filename, fake_ttl=False,
                src_mac=None, dst_mac=None, src_ip_addr=None, dst_ip_addr=None):
        if fake_ttl is True and len(self.src_hosts_with_fake_ttl) == 0:
            print("Please extract ip2hc table before modify traffic with fake ttl.")

        # show modify request
        request = "Generating " + output_filename + " with\n"
        if fake_ttl:
            request += " fake ttl\n"
        if src_mac is not None:
            request += " src mac:" + src_mac + "\n"
        if dst_mac is not None:
            request += " dst mac:" + dst_mac + "\n"
        if src_ip_addr is not None:
            request += " src ip addr:" + src_ip_addr + "\n"
        if dst_ip_addr is not None:
            request += " dst ip addr:" + dst_ip_addr + "\n"
        print(request + "\n")

        pcap_reader = PcapReader(self.pcap_filename)
        pcap_writer = PcapWriter(output_filename)

        counter = 0

        while True:
            pkt = pcap_reader.read_packet()
            if pkt is None:
                break
            if not pkt.haslayer('Ether') or not pkt.haslayer('IP'):
                continue
            # ipv4 packets
            counter += 1
            ip_int = self.__ip_str2int(pkt['IP'].src)
            if fake_ttl:
                pkt['IP'].ttl = self.src_hosts_with_fake_ttl[ip_int]
            if src_mac is not None:
                pkt['Ethernet'].src = src_mac
            if dst_mac is not None:
                pkt['Ethernet'].dst = dst_mac
            if src_ip_addr is not None:
                pkt['IP'].src = src_ip_addr
            if dst_ip_addr is not None:
                pkt['IP'].dst = dst_ip_addr

            pcap_writer.write(pkt)
            if counter % 10000 == 0:
                print("%d packets have been processed\n" % counter)

        pcap_writer.flush()
        pcap_writer.close()
        pcap_reader.close()

    def __generate_ip(self):
        return random.randint(0, IP_MAX)


    def __generate_rand_ttl(self):
        rand_hc = -1
        while rand_hc < 1 or rand_hc > 63:
            rand_hc = int(np.random.normal(16.5, 4))
        # choose one possible ttl
        if rand_hc <= 2:
            initial_ttl = (30, 32, 60, 64, 127)[random.randint(0, 4)]
        elif rand_hc <= 4:
            initial_ttl = (30, 60, 64, 127)[random.randint(0, 3)]
        elif rand_hc <= 28:
            initial_ttl = (30, 60, 127)[random.randint(0, 2)]
        elif rand_hc <= 30:
            initial_ttl = (30, 127)[random.randint(0, 1)]
        elif rand_hc <= 63:
            initial_ttl = 127
        return initial_ttl - rand_hc


    def gen_traffic(self, output_filename,
                    dst_ip_addr, dst_mac, src_mac="00:00:00:00:00:01",
                    ip_num=10000, packet_num=500000, payload="Normal Traffic pkts"):

        print("PAcket Number: %d" % packet_num)

        if not output_filename.endswith(".pcap"):
            print("Invalid PCAP filename! Exiting...")
            return
        self.pcap_filename = output_filename
        self.ip_hc_filename = str.join(".", output_filename.split(".")[:-1]) + "_ip_hc.txt"
        self.ip_fake_ttl_filename = str.join(".", output_filename.split(".")[:-1]) + "_fake_ttl.txt"

        # show modify request
        request = "Generating " + output_filename + " with\n"
        if src_mac is not None:
            request += " src mac:" + src_mac + "\n"
        if dst_mac is not None:
            request += " dst mac:" + dst_mac + "\n"
        if dst_ip_addr is not None:
            request += " dst ip addr:" + dst_ip_addr + "\n"
        print(request + "\n")

        pcap_writer = PcapWriter(output_filename)

        src_ips = []
        src_hosts_with_ttl = {}

        for i in range(packet_num):
            if len(self.src_hosts) < ip_num:
                # pick a random src ip
                src_ip = self.__generate_ip()
                # pick a random hc
                ttl = self.__generate_rand_ttl()
            else:
                src_ip = src_ips[random.randint(0, len(src_ips) - 1)]
                ttl = src_hosts_with_ttl[src_ip]
            # calculate ttl according to hc
            pkt = Ether(src=src_mac, dst=dst_mac)/IP(src=self.__ip_int2str(src_ip), dst=dst_ip_addr, ttl=ttl)/TCP(flags=0x10)/payload
            pcap_writer.write(pkt)
            if src_ip not in src_ips:
                src_ips.append(src_ip)
                src_hosts_with_ttl[src_ip] = ttl
                self.src_hosts[src_ip] = self.__ttl2hc(ttl)
            if i % 10000 == 0:
                print("%d packets have been produced\n" % i)

        print(str(len(self.src_hosts)) + " source hosts produced")

        print("Writing ip,hc dict into " + self.ip_hc_filename + "...")
        with open(self.ip_hc_filename, "w") as f:
            json.dump(self.src_hosts, f)
            f.close()

        pcap_writer.flush()
        pcap_writer.close()

    def gen_spoofing_attack(self, output_filename,
                    dst_ip_addr, dst_mac, src_mac="00:00:00:00:00:11",
                    syn=True, ack=True, udp=True,
                    packet_num=500000, payload="Spoofing pkts"):
        if len(self.src_hosts_with_fake_ttl) == 0:
            print("Please extract ip2hc table before modify traffic with fake ttl.")
        # convert dict to array
        src_hosts_array = []
        for (src_ip, hc) in self.src_hosts.items():
            src_hosts_array.append(src_ip)

        # show modify request
        request = "Generating " + output_filename + " including\n"
        if syn:
            request += " spoofing SYN packets\n"
        if ack:
            request += " spoofing ACK packets\n"
        if udp:
            request += " spoofing UDP packets\n"
        request += "with\n"
        if src_mac is not None:
            request += " src mac:" + src_mac + "\n"
        if dst_mac is not None:
            request += " dst mac:" + dst_mac + "\n"
        if dst_ip_addr is not None:
            request += " dst ip addr:" + dst_ip_addr + "\n"
        print(request + "\n")

        pkt_types = []
        if syn:
            pkt_types.append("SYN")
        if ack:
            pkt_types.append("ACK")
        if udp:
            pkt_types.append("UDP")

        pcap_writer = PcapWriter(output_filename)


        for i in range(packet_num):
            # pick a random packet type
            pkt_type = pkt_types[random.randint(0, len(pkt_types) - 1)]
            # pick a random src ip
            src_host = src_hosts_array[random.randint(0, len(src_hosts_array) - 1)]
            # pick a random hc
            rand_ttl = self.__generate_rand_ttl()
            # calculate ttl according to hc
            if pkt_type == "SYN":
                pkt = Ether(src=src_mac, dst=dst_mac)/IP(src=self.__ip_int2str(int(src_host)),
                    dst=dst_ip_addr, ttl=rand_ttl)/TCP(flags=0x02)/payload
            elif pkt_type == "ACK":
                pkt = Ether(src=src_mac, dst=dst_mac)/IP(src=self.__ip_int2str(int(src_host)),
                    dst=dst_ip_addr, ttl=rand_ttl)/TCP(flags=0x10)/payload
            elif pkt_type == "UDP":
                pkt = Ether(src=src_mac, dst=dst_mac)/IP(src=self.__ip_int2str(int(src_host)),
                    dst=dst_ip_addr, ttl=rand_ttl)/UDP()/payload
            pcap_writer.write(pkt)
            if i % 10000 == 0:
                print("%d packets have been produced\n" % i)

        pcap_writer.flush()
        pcap_writer.close()




pcap_filename = str(sys.argv[1])
if os.path.exists(pcap_filename):
    # read from existing file
    modified_filename = str.join(".", pcap_filename.split(".")[:-1]) + "_modified.pcap"
    gen = TrafficFileGenerator(pcap_filename)
    # gen.extract_ip2hc_table()
    gen.restore_from_file()
    # gen.modify_traffic(modified_filename, dst_ip_addr="10.0.0.4", dst_mac="68:91:d0:61:b4:c4")
    gen.gen_spoofing_attack("./spoofing_attack.pcap", "10.0.0.4", "68:91:d0:61:b4:c4")
else:
    # generate new traffic
    gen = TrafficFileGenerator()
    gen.gen_traffic(pcap_filename, "10.0.0.4", "68:91:d0:61:b4:c4")
    gen.gen_spoofing_attack("./spoofing_attack.pcap", "10.0.0.4", "68:91:d0:61:b4:c4")