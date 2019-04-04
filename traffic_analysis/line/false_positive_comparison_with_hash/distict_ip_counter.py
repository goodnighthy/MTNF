#!/usr/bin/env python
# coding=utf-8

from scapy.utils import PcapReader, PcapWriter
from scapy.all import *
import sys


filename = sys.argv[1]
print("Reading " + filename)

pkt_reader = PcapReader(filename)
# pcap_reader = PcapReader("./p1p1_500w.pcap")

ip_pool = []
count = 0

while True:
    pkt = pkt_reader.read_packet()
    if pkt is None:
        break
    if not pkt.haslayer('Ether') or not pkt.haslayer('IP'):
        continue
    if pkt['IP'].src not in ip_pool:
        ip_pool.append(pkt['IP'].src)
    count = count + 1
    if count % 10000 == 0:
        print("Reading line " + str(count))

print("File " + filename + " has " + str(len(ip_pool)) + " distice IPs.")
