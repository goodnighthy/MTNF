#!/usr/bin/env python
# coding=utf-8
import numpy as np
from cycler import cycler
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator, FormatStrFormatter
import matplotlib.patches as mpatches
import random
import crc16
from zlib import crc32
import hashlib
import pickle
import time
from abc import ABCMeta, abstractmethod

import sys
sys.path.append("..")
import plot_config

IP_MAX = 0xFFFFFFFF
SWITCH_CACHE_SIZE = 1000

class Switch:
    __metaclass__ = ABCMeta
    CACHE_SIZE = SWITCH_CACHE_SIZE
    ip_list = None
    cache_list = None

    def __init__(self):
        self.cache_list = {}
        self.ip_list = {}

    @abstractmethod
    def receive(self, ip, spoofing):
        pass

    def cache_decay(self):
        for (ip, hit_count) in self.ip_list.items():
            self.ip_list[ip] = int(hit_count * 0.8)
        for (ip, hit_count) in self.cache_list.items():
            self.cache_list[ip] = int(hit_count * 0.8)

    def calculate_hop_percentage(self, hot_ip_list):
        hot_count = 0
        for ip in hot_ip_list:
            if ip in self.cache_list:
                hot_count += 1
        return hot_count / float(len(self.cache_list))


class NetHCFSwitch(Switch):

    def __init__(self):
        Switch.__init__(self)

    def receive(self, ip, spoofing):
        if spoofing:
            return
        # update value
        if ip in self.ip_list:
            self.ip_list[ip] += 1
        else:
            self.ip_list[ip] = 1
        if ip in self.cache_list:
            # if ip already in cache
            self.cache_list[ip] += 1
            return
        # ip is not in cache
        # decide update or not
        if len(self.cache_list) < self.CACHE_SIZE:
            # if cache is not full
            # just insert
            self.cache_list[ip] = self.ip_list[ip]
            return
        # if cache is full
        # see if we need to replace some
        # get the lowest value in cache
        min_hit = 0xFFFFFFFF
        for (ip_int, hit_count) in self.cache_list.items():
            if hit_count < min_hit:
                min_hit = hit_count
                min_ip = ip_int
        if min_hit < self.ip_list[ip]:
            # need to replace!
            del self.cache_list[min_ip]
            self.cache_list[ip] = self.ip_list[ip]

class NetCacheSwitch(Switch):

    def __init__(self):
        Switch.__init__(self)


    def cache_decay(self):
        for (ip, hit_count) in self.ip_list.items():
            self.ip_list[ip] = int(hit_count * 0.9)
        for (ip, hit_count) in self.cache_list.items():
            self.cache_list[ip] = int(hit_count * 0.9)

    def receive(self, ip, spoofing):
        # update value
        if ip in self.ip_list:
            self.ip_list[ip] += 1
        else:
            self.ip_list[ip] = 1
        if ip in self.cache_list:
            # if ip already in cache
            self.cache_list[ip] += 1
            return
        # ip is not in cache
        # decide update or not
        if len(self.cache_list) < self.CACHE_SIZE:
            # if cache is not full
            # just insert
            self.cache_list[ip] = self.ip_list[ip]
        # if cache is full
        # see if we need to replace some
        # get the lowest value in cache
        min_hit = 0xFFFFFFFF
        for (ip_int, hit_count) in self.cache_list.items():
            if hit_count <= min_hit:
                min_hit = hit_count
                min_ip = ip_int
        del self.cache_list[min_ip]
        self.cache_list[ip] = self.ip_list[ip]

class NoUpdateSwitch(Switch):

    def __init__(self):
        Switch.__init__(self)


    def receive(self, ip, spoofing):
        if len(self.cache_list) < self.CACHE_SIZE:
            # if cache is not full
            # just insert
            self.cache_list[ip] = 1

def generate_ip():
    return random.randint(0, IP_MAX)

def simulate():
    HOT_IP_SIZE = 850
    FIX_IP_SIZE = 50
    ATTACK_START_TIME = 200
    HOT_IP_PKT_PER_SEC = 2
    RAND_IP_PER_SEC = 100
    ATTACK_IP_PER_SEC = 200
    ATTACK_PKT_PER_IP_PER_SEC = 15
    # simulate
    s1 = NetHCFSwitch()
    s2 = NetCacheSwitch()
    s3 = NoUpdateSwitch()
    hot_ips = []
    fix_ips = []
    hot_precentage = [[], [], []]
    # first generate a set of ips to fill up the cache
    for i in range(SWITCH_CACHE_SIZE):
        ip = generate_ip()
        s1.receive(ip, False)
        s2.receive(ip, False)
        s3.receive(ip, False)
        if len(hot_ips) < HOT_IP_SIZE:
            hot_ips.append(ip)
        elif len(fix_ips) < FIX_IP_SIZE:
            fix_ips.append(ip)
    time = -1

    while True:
        time += 1
        print("Time: %d" % time)
        # replace part of hot ips
        for i in range(random.randint(10, 30)):
            rand_index = random.randint(0, len(hot_ips) - 1)
            del hot_ips[rand_index]
        for i in range(random.randint(10, 30)):
            rand_ip = generate_ip()
            hot_ips.append(rand_ip)
        # send hot ips
        for ip in hot_ips:
            for i in range(random.randint(3, 5)):
                s1.receive(ip, False)
                s2.receive(ip, False)
                s3.receive(ip, False)
        # send fix ips
        for ip in fix_ips:
            for i in range(HOT_IP_PKT_PER_SEC):
                s1.receive(ip, False)
                s2.receive(ip, False)
                s3.receive(ip, False)
        # randomly send some ips
        for i in range(random.randint(80, 130)):
            ip = generate_ip()
            s1.receive(ip, False)
            s2.receive(ip, False)
            s3.receive(ip, False)
        # attack
        if time > ATTACK_START_TIME:
            for i in range(random.randint(100, 300)):
                ip = generate_ip()
                for j in range(random.randint(10, 40)):
                    s1.receive(ip, True)
                    s2.receive(ip, True)
                    s3.receive(ip, True)
        hot_precentage[0].append(s1.calculate_hop_percentage(hot_ips + fix_ips))
        hot_precentage[1].append(s2.calculate_hop_percentage(hot_ips + fix_ips))
        hot_precentage[2].append(s3.calculate_hop_percentage(hot_ips + fix_ips))
        print("%f, %f, %f" % (hot_precentage[0][time], hot_precentage[1][time], hot_precentage[2][time]))
        s1.cache_decay()
        s2.cache_decay()
        s3.cache_decay()
        if time > 400:
            break
    return hot_precentage

restore_from_file = True
pickle_filename = "./simulation_data.pickle"
if restore_from_file:
    print("Restoring data from file " + pickle_filename)
    with open(pickle_filename, "rb") as f:
        hot_precentage = pickle.load(f)
else:
    hot_precentage = simulate()
    # save data into file
    print("Storing data into file " + pickle_filename)
    with open(pickle_filename, "wb") as f:
        pickle.dump(hot_precentage, f)
period = len(hot_precentage[0])

# plotting
plt.figure(figsize=plot_config.fig_size)
ax = plt.subplot(111)
xmajor = int(period / 8)
xmajorLocator   = MultipleLocator(xmajor)
xmajorFormatter = FormatStrFormatter('%1d')
xminorLocator   = MultipleLocator(xmajor / 2.0)
ymajor = int(1.0 / 5 * 100)
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
color = 0
plt.plot(range(period), [j * 100 for j in hot_precentage[0]], '-', label="NetHCF Update")
plt.plot(range(period), [j * 100 for j in hot_precentage[1]], '-', label="NetCache Update")
plt.plot(range(period), [j * 100 for j in hot_precentage[2]], '-', label="No Update")
plt.ylim(0, 100)
plt.xlim(0, period)
plt.legend(loc='lower right', bbox_to_anchor=(1, 0.2), fontsize=plot_config.font_size, shadow=False)

ax.annotate('Attack Starts', xy=(200, 82),
            xytext=(50, 70), fontsize=plot_config.font_size,
            arrowprops=dict(facecolor=plot_config.colors[0], arrowstyle='fancy',
                connectionstyle="arc3,rad=0.5"),
)
line0 = mpl.lines.Line2D([200, 200], [-2, 102], lw=1, color='grey', linestyle='-.', alpha=0.8)
line0.set_clip_on(False)
ax.add_line(line0)

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
plt.xlabel('Time(s)', fontsize=plot_config.font_size)
plt.ylabel('Per of Hot & Legitimate IPs (%)', fontsize=plot_config.font_size)
plt.tight_layout(rect=[0, 0, 1, 1])
plt.subplots_adjust(wspace=0, hspace=0.05)
plt.savefig('cache_update.pdf')
plt.show()