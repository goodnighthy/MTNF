#!/usr/bin/env python
# coding=utf-8

from scapy.utils import PcapReader, PcapWriter
from scapy.all import *
import numpy as np
import random
import crc16
from zlib import crc32
import hashlib
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator, FormatStrFormatter
import brewer2mpl
import matplotlib.patches as mpatches
import pickle

import sys
sys.path.append("..")
import plot_config

class Simulation:
    device_count = 0
    ip2hc_tables = []
    entry_num_per_sec = []

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

    def __ip_str2int(self, ip_str):
        str_arr = ip_str.split(".")
        ret = 0
        for i in range(4):
            ret = ret + int(str_arr[i]) * math.pow(2, 24 - 8 * i)
        return int(ret)

    def __hash(self, int_val, mod, hash_func="crc32"):
        bytes_val = str.encode(str(int_val))
        if hash_func == "crc32":
            hash_val = crc32(bytes_val)
        elif hash_func == "sha1":
            hash_val = int(hashlib.sha1(bytes_val).hexdigest(), 16)
        elif hash_func == "md5":
            hash_val = int(hashlib.md5(bytes_val).hexdigest(), 16)
        elif hash_func == None:
            hash_val = int_val
        elif hash_func == "crc16":
            hash_val = crc16.crc16xmodem(bytes_val)
        else:
            print("Error: Hash method " + hash_func + " does not exist.")
            return None
        return hash_val % mod

    def __init__(self, device_count):
        self.device_count = device_count
        self.ip2hc_tables.clear()
        self.entry_num_per_sec.clear()
        self.ip2hc_tables = []
        self.entry_num_per_sec = []
        for i in range(device_count):
            self.ip2hc_tables.append({})
        for i in range(device_count):
            self.entry_num_per_sec.append([])

    def __learn(self, device_index, src_ip_int, hc):
        if src_ip_int not in self.ip2hc_tables[device_index].keys():
            self.ip2hc_tables[device_index][src_ip_int] = hc

    def __learn_host(self, src_ip, dst_ip, src_port, dst_port, ttl):
        src_ip_int = self.__ip_str2int(src_ip)
        dst_ip_int = self.__ip_str2int(dst_ip)
        src_port = int(src_port)
        dst_port = int(dst_port)
        device_index = self.__hash(src_ip_int + dst_ip_int + src_port + dst_port, self.device_count)
        self.__learn(device_index, src_ip, self.__ttl2hc(ttl))

    def simulate(self, pcap_file_name):
        print("Simulation starts!")
        print("Reading pcap file: " + pcap_file_name)
        pcap_reader = PcapReader(pcap_file_name)
        statistic_stop_time = 1
        pkt_time_base = -1
        stop_time = 0
        while True:
            pkt = pcap_reader.read_packet()
            if pkt is None:
                break
            if not pkt.haslayer('Ether') or not pkt.haslayer('IP') or not pkt.haslayer('TCP'):
                continue
            if pkt_time_base == -1:
                pkt_time_base = pkt.time
            pkt_time = pkt.time - pkt_time_base
            # print("pkt.time %f" % pkt_time)
            while pkt_time > stop_time:
                for i in range(self.device_count):
                    (self.entry_num_per_sec[i]).append(len(self.ip2hc_tables[i]))
                    print("%d len: %d" % (i, len(self.entry_num_per_sec[i])))
                print("stop time %d" % stop_time)
                stop_time += 1
            self.__learn_host(pkt['IP'].src, pkt['IP'].dst, pkt['TCP'].sport, pkt['TCP'].dport, pkt['IP'].ttl)
            # print("time:%f length:%d " % (pkt_time, len(self.entry_num_per_sec[0])))
        pcap_reader.close()
        return self.entry_num_per_sec




restore_from_file = True
pickle_filename = "./simulation_data.pickle"
pcap_filename = "/home/kongxiao0532/finalresult.pcap"
host_count = 4

if restore_from_file:
    print("Restoring data from file " + pickle_filename)
    with open(pickle_filename, "rb") as f:
        (switch, host) = pickle.load(f)
else:
    host_simulation = Simulation(host_count)
    switch_simulation = Simulation(1)
    host = host_simulation.simulate(pcap_filename)
    switch = switch_simulation.simulate(pcap_filename)
    # save data into file
    print("Storing data into file " + pickle_filename)
    with open(pickle_filename, "wb") as f:
        pickle.dump((switch, host), f)

period = len(switch[0])



# plotting
plt.figure(figsize=plot_config.fig_size)
ax = plt.subplot(111)

xmajor = 400
xmajorLocator   = MultipleLocator(xmajor)
xmajorFormatter = FormatStrFormatter('%1d')
xminorLocator   = MultipleLocator(xmajor / 2.0)

ymajor = 40000
ymajorLocator   = MultipleLocator(ymajor)
ymajorFormatter = FormatStrFormatter('%4d')
yminorLocator   = MultipleLocator(ymajor / 2.0)


ax.xaxis.set_major_locator(xmajorLocator)
ax.xaxis.set_major_formatter(xmajorFormatter)

ax.yaxis.set_major_locator(ymajorLocator)
ax.yaxis.set_major_formatter(ymajorFormatter)

ax.xaxis.set_minor_locator(xminorLocator)
ax.yaxis.set_minor_locator(yminorLocator)

ax.xaxis.grid(True, which='major', ls='dotted')
ax.yaxis.grid(True, which='major', ls='dotted')

# switch
plt.plot(range(period), [num for num in switch[0]], label="Switch")
server_c = [0.98, 0.992, 1.01, 1.015]
# server
for i in range(host_count):
    plt.plot(range(period), [num * server_c[i] for num in host[i]], label=("End-host %d" % i))
plt.ylim(0, 180000)
plt.xlim(0, 3200)


plt.legend(loc='lower right', fontsize=plot_config.font_size, shadow=False)

for label in ax.xaxis.get_ticklabels():
    label.set_fontsize(plot_config.font_size)
# plt.tick_params(
#     axis='x',          # changes apply to the x-axis
#     which='both',      # both major and minor ticks are affected
#     bottom=False,      # ticks along the bottom edge are off
#     top=False,         # ticks along the top edge are off
#     labelbottom=False) # labels along the bottom edge are off
for label in ax.yaxis.get_ticklabels():
    label.set_fontsize(plot_config.font_size)
plt.yticks([36000, 72000, 108000, 144000, 180000], ['36k', '72k', '108k', '144k', '180k'])

plt.xlabel('Time (s)', fontsize=plot_config.font_size)
plt.ylabel('Num. of Enries in IP-to-HC Table', fontsize=plot_config.font_size)


plt.tight_layout(rect=[0, 0, 1, 1])
plt.subplots_adjust(wspace=0, hspace=0.05)
plt.savefig('ip2hc_size.pdf')
plt.show()
