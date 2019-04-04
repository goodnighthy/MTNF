#!/usr/bin/env python
# coding=utf-8

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
import time

IP_NUMBER = 1
IP_MAX = 0xFFFFFFFF

class Packet:
    src_ip = ""
    ttl = -1
    passes = False

class Switch:
    ip_to_hc_table_size = -1
    # IP_TO_HC_TABLE_SIZE = 8388608
    hc_array = []
    update_array = []
    hash_func = []

    def __init__(self, table_size=10000, hash_func=["crc32"]):
        np.random.seed(0)
        self.ip_to_hc_table_size = table_size
        self.hash_func = hash_func
        self.hc_array = [0] * self.ip_to_hc_table_size
        self.update_array = [0] * self.ip_to_hc_table_size

    # def __ip_str2int(self, ip_str):
        # return "%u.%u.%u.%u" % ((ip_str >> 24) & 0xff,
        #     (ip_str >> 16) & 0xff, (ip_str >>  8) & 0xff, ip_str & 0xff)

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

    def __hash(self, ip, hash_func="crc32"):
        ip_bytes = str.encode(str(ip))
        if hash_func == "crc32":
            hash_val = crc32(ip_bytes)
        elif hash_func == "sha1":
            hash_val = int(hashlib.sha1(ip_bytes).hexdigest(), 16)
        elif hash_func == "md5":
            hash_val = int(hashlib.md5(ip_bytes).hexdigest(), 16)
        elif hash_func == None:
            hash_val = ip
        elif hash_func == "crc16":
            hash_val = crc16.crc16xmodem(bytes(ip))
        else:
            print("Error: Hash method " + hash_func + " does not exist.")
            return None
        return hash_val % self.ip_to_hc_table_size

    def copy_and_clear(self):
        for i in range(self.ip_to_hc_table_size):
            self.hc_array[i] = self.update_array[i]
        self.update_array = [0] * self.ip_to_hc_table_size

    def learn_ip(self, ip, true_ttl):
        true_hc = self.__ttl2hc(true_ttl)
        for i in range(len(self.hash_func)):
            index = self.__hash(ip, hash_func=self.hash_func[i])
            # insert into hc_array
            self.hc_array[index] |= 1 << true_hc
            # insert into update_array
            self.update_array[index] |= 1 << true_hc


    def passes(self, ip, ttl):
        hc = self.__ttl2hc(ttl)
        target_bitmap = 1 << hc
        for i in range(len(self.hash_func)):
            index = self.__hash(ip, hash_func=self.hash_func[i])
            if self.hc_array[index] & target_bitmap == 0:
                # not in
                return False
        # passes
        # insert into update_array
        for i in range(len(self.hash_func)):
            index = self.__hash(ip, hash_func=self.hash_func[i])
            self.update_array[index] |= 1 << hc
        return True

def generate_ip():
    return random.randint(0, IP_MAX)

def generate_ttl():
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

def simulate_hash(
    switch_table_size, hash_func=None,
    stop_per_threshold=None, stop_sec_threshold=None,
    new_ip_per_second=None, fake_ip_per_second=1000
    ):
    switch = Switch(switch_table_size, hash_func)
    src_host_pool = {} # ip_int : ttl
    src_ip_pool = []
    false_poisitive_percentage = [0]
    if new_ip_per_second is None:
        new_ip_per_second = int(switch.ip_to_hc_table_size / 500)
    print("The size of switch register is %d" % switch.ip_to_hc_table_size)
    print("%d new IPs per second" % new_ip_per_second)
    while True:
        # every 1 second
        time_tick = len(false_poisitive_percentage)
        print("%d second:" % time_tick)
        # copy and clear update_array
        if time_tick % 400 == 0:
            switch.copy_and_clear()
        # simulate learning
        # generate new ip and insert into switch
        for i in range(new_ip_per_second):
            ip_int = generate_ip()
            while ip_int in src_host_pool.keys():
                ip_int = generate_ip()
            ttl = generate_ttl()
            switch.learn_ip(ip_int, ttl)
            src_host_pool[ip_int] = ttl
            src_ip_pool.append(ip_int)
        print("%d new IPs have been inserted" % new_ip_per_second)
        # send normal packets
        count = 0
        for i in range(1000):
            ip_index = random.randint(0, len(src_ip_pool) - 1)
            ip = src_ip_pool[ip_index]
            ttl = src_host_pool[ip]
            if not switch.passes(ip, ttl):
                del src_ip_pool[ip_index]
                count += 1
        print(count / 1000.0)
        # generate fake hopcount
        # send packets and calculate false positive percentage
        false_positive_count = 0
        for i in range(fake_ip_per_second):
            ip_int = generate_ip()
            fake_ttl = generate_ttl()
            if switch.passes(ip_int, fake_ttl):
                false_positive_count += 1
        false_poisitive_percentage.append(
            false_positive_count / float(fake_ip_per_second)
        )
        print("%d packets sent, %d passed, fp:%f" %
            (
                fake_ip_per_second,
                false_positive_count,
                false_positive_count / float(fake_ip_per_second)
            )
        )
        if stop_per_threshold is not None:
            if false_positive_count / float(fake_ip_per_second) >= stop_per_threshold:
                break
        if stop_sec_threshold is not None:
            if len(false_poisitive_percentage) >= stop_sec_threshold:
                break
    return false_poisitive_percentage

# simulate hash version
stop_per_threshold = 1
stop_sec_threshold = 5000
test_fake_ip_per_sec = 3000
configs = [
    [100000, 200, ["crc32"], "Single Hash Table"],
    [100000, 200, ["crc32", "md5"], "Two Hash Tables"],
    [100000, 200, ["crc32", "md5", "sha1"], "Three Hash Tables"],
    # [100000, 1000, ["crc32"]],
    # [100000, 1000, ["crc32", "md5"]],
    # [100000, 1000, ["crc32", "md5", "sha1"]],
]
hash_tn_per = []


restore_from_file = True
pickle_filename = "./simulation_data.pickle"

if restore_from_file:
    print("Restoring data from file " + pickle_filename)
    with open(pickle_filename, "rb") as f:
        hash_tn_per = pickle.load(f)
else:
    for config in configs:
        hash_tn_per.append(
            simulate_hash(
                switch_table_size=config[0],
                stop_per_threshold=stop_per_threshold,
                stop_sec_threshold=stop_sec_threshold,
                new_ip_per_second=config[1],
                fake_ip_per_second=test_fake_ip_per_sec,
                hash_func=config[2]
            )
        )
    # save data into file
    print("Storing data into file " + pickle_filename)
    with open(pickle_filename, "wb") as f:
        pickle.dump(hash_tn_per, f)

period = len(hash_tn_per[0])
print("%f %f %f" % (hash_tn_per[0][4999], hash_tn_per[1][4999], hash_tn_per[2][4999]))

# set plot colors
bmap = brewer2mpl.get_map('Set1', 'qualitative', 5)
colors = bmap.mpl_colors

# plotting
plt.figure(figsize=(10, 8))
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
for i in range(len(configs)):
    plt.plot(range(period), [i * 100 for i in hash_tn_per[i]], '-', color=colors[color], label=configs[i][3])
    color = (color + 1) % 5
plt.plot(range(period), [0] * period, 'o-', color=colors[color], label="Cache Method")
plt.ylim(0, 100)
plt.xlim(0, period)


plt.legend(loc='upper right', fontsize='xx-large', shadow=False)

for label in ax.xaxis.get_ticklabels():
    label.set_fontsize('xx-large')
# plt.tick_params(
#     axis='x',          # changes apply to the x-axis
#     which='both',      # both major and minor ticks are affected
#     bottom=False,      # ticks along the bottom edge are off
#     top=False,         # ticks along the top edge are off
#     labelbottom=False) # labels along the bottom edge are off
for label in ax.yaxis.get_ticklabels():
    label.set_fontsize('xx-large')

plt.xlabel('Time(s)', fontsize='xx-large')
plt.ylabel('False Positive Percentage (%)', fontsize='xx-large')


plt.tight_layout(rect=[0, 0, 1, 1])
plt.subplots_adjust(wspace=0, hspace=0.05)
plt.savefig('false_positive_percentage.pdf')
plt.show()
