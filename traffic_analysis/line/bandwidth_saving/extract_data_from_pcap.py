#!/usr/bin/env python
# coding=utf-8

from scapy.utils import PcapReader, PcapWriter
from scapy.all import *
import math
import sys
import json
import random
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator, FormatStrFormatter
import brewer2mpl
import pickle
# from scipy.interpolate import spline
import matplotlib.patches as mpatches

# plot config
sys.path.append("..")
import plot_config

def read_file(filename, interval_in_s):
    start_time = -1
    stop_time = interval
    sender_reader = PcapReader(filename)
    SYN_pps = []
    ACK_pps = []
    UDP_pps = []
    normal_pps = []
    SYN_count_in_interval = 0
    ACK_count_in_interval = 0
    UDP_count_in_interval = 0
    normal_count_in_interval = 0
    while True:
        pkt = sender_reader.read_packet()
        if pkt is None:
            break
        if not pkt.haslayer('Ether') or not pkt.haslayer('IP'):
            continue
        if start_time == -1:
            start_time = pkt.time
        time = pkt.time - start_time
        while time > stop_time:
            if stop_time != interval:
                if SYN_pps[-1] \
                    + ACK_pps[-1] \
                    + UDP_pps[-1] == 0 \
                    and \
                    SYN_count_in_interval \
                    + ACK_count_in_interval \
                    + UDP_count_in_interval != 0:
                    print("time:%f, stop_time: %f" % (time, stop_time))
                    print("SYN %d ACK %d UDP %d normal %d" % (SYN_count_in_interval, ACK_count_in_interval, UDP_count_in_interval, normal_count_in_interval))
                elif SYN_pps[-1] \
                    + ACK_pps[-1] \
                    + UDP_pps[-1] != 0 \
                    and \
                    SYN_count_in_interval \
                    + ACK_count_in_interval \
                    + UDP_count_in_interval == 0:
                    print("time:%f, stop_time: %f" % (time, stop_time))
                    print("SYN %d ACK %d UDP %d normal %d" % (SYN_count_in_interval, ACK_count_in_interval, UDP_count_in_interval, normal_count_in_interval))
            normal_pps.append(normal_count_in_interval / 10.0)
            SYN_pps.append(SYN_count_in_interval / 10.0)
            ACK_pps.append(ACK_count_in_interval / 10.0)
            UDP_pps.append(UDP_count_in_interval / 10.0)
            stop_time += interval
            SYN_count_in_interval = 0
            ACK_count_in_interval = 0
            UDP_count_in_interval = 0
            normal_count_in_interval = 0


        if pkt['Ether'].src == "00:00:00:00:00:11":
            # spoofed!
            if pkt.haslayer('UDP'):
                UDP_count_in_interval += 1
            elif pkt.haslayer('TCP'):
                if pkt['TCP'].flags == 0x02:
                    SYN_count_in_interval += 1
                elif pkt['TCP'].flags == 0x10:
                    ACK_count_in_interval += 1
        else:
            # unspoofed!
            normal_count_in_interval += 1
    sender_reader.close()
    return (SYN_pps, ACK_pps, UDP_pps, normal_pps), stop_time - interval


extract_from_pcap = False
#13.73
sender_filename = "/home/kongxiao0532/sender_spoofing.pcap"
receiver_filename = "/home/kongxiao0532/receiver_spoofing.pcap"
pickle_filename = "./traffic_data.pickle"

interval = 0.01 #s

if extract_from_pcap:
    sender_pps, sender_stoptime = read_file(sender_filename, interval)
    receiver_pps, receiver_stoptime = read_file(receiver_filename, interval)

    print("Storing pps, stoptime info into " + pickle_filename)
    with open(pickle_filename, "wb") as f:
        pickle.dump((sender_pps, sender_stoptime, receiver_pps, receiver_stoptime), f,  protocol=pickle.HIGHEST_PROTOCOL)
        f.flush()
        f.close()
    print("Safely stored!")
else:
    with open(pickle_filename, 'rb') as f:
        tmp = pickle.load(f)
        sender_pps = tmp[0]
        sender_stoptime = tmp[1]
        receiver_pps = tmp[2]
        receiver_stoptime = tmp[3]
        f.close()
    print("Pickle loaded!")

false_positive = \
    (np.mean(receiver_pps[0]) + np.mean(receiver_pps[1]) + np.mean(receiver_pps[2])) / \
        (np.mean(sender_pps[0]) + np.mean(sender_pps[1]) + np.mean(sender_pps[2]))

print("FP: %f" % false_positive)
# time:12.130019, stop_time: 12.130000
# SYN 50 ACK 34 UDP 37 normal 299
# time:21.700052, stop_time: 21.700000
# SYN 0 ACK 0 UDP 0 normal 300
# time:12.140002, stop_time: 12.140000
# SYN 38 ACK 25 UDP 23 normal 298
# time:21.710065, stop_time: 21.710000
# SYN 0 ACK 0 UDP 0 normal 300

attack_start = 12.14
attack_stop = 21.7
entering_filtering = 12.17

# calculating timeline
timeline_sender = [time / float(100) for time in range(0, len(sender_pps[0]))]
timeline_receiver = [time / float(100) for time in range(0, len(receiver_pps[0]))]
start_time = 8 # in sec
end_time = 26 # in sec
timeline_sender = timeline_sender[start_time * 100 : end_time * 100]
timeline_receiver = timeline_receiver[start_time * 100 : end_time * 100]

# plotting
plt.figure(figsize=plot_config.fig_size)
xmajorLocator   = MultipleLocator(2)
xmajorFormatter = FormatStrFormatter('%1d')
xminorLocator   = MultipleLocator(1)

ymajorLocator   = MultipleLocator(18)
ymajorFormatter = FormatStrFormatter('%4d')
yminorLocator   = MultipleLocator(9)


ax = plt.subplot(211)

ax.xaxis.set_major_locator(xmajorLocator)
ax.xaxis.set_major_formatter(xmajorFormatter)

ax.yaxis.set_major_locator(ymajorLocator)
ax.yaxis.set_major_formatter(ymajorFormatter)

ax.xaxis.set_minor_locator(xminorLocator)
ax.yaxis.set_minor_locator(yminorLocator)

ax.xaxis.grid(True, which='major', ls='dotted')
ax.yaxis.grid(True, which='major', ls='dotted')

ax.annotate('Attack Starts', xy=(attack_start, 0),
            xytext=(attack_start - 4, 11), fontsize=plot_config.font_size,
            arrowprops=dict(arrowstyle='fancy',
                connectionstyle="arc3,rad=0.1"),
)

ax.annotate('Attack Stops', xy=(attack_stop, 21),
            xytext=(attack_stop + 0.3, 23), fontsize=plot_config.font_size,
            arrowprops=dict(arrowstyle='fancy',
                connectionstyle="arc3,rad=-0.1"),
)
bbox_props = dict(boxstyle="round", fc="w", ec="0.5", alpha=0.9)
ax.text(end_time - 1.0 * end_time / 60, 38, "Client Side", ha="right", va="top", size=plot_config.font_size,
        bbox=bbox_props)

plt.ylim(0, 40)
plt.xlim(start_time, end_time)
plt.plot(timeline_sender, sender_pps[3][start_time * 100 : end_time * 100])
plt.plot(timeline_sender, sender_pps[0][start_time * 100 : end_time * 100])
# plt.plot(timeline_sender, sender_pps[1][start_time * 100 : end_time * 100])
# plt.plot(timeline_sender, sender_pps[2][start_time * 100 : end_time * 100])


legend_dict = {
    'Normal Traffic' : plot_config.colors[0],
    'Spoofed Packets' : plot_config.colors[1],
    # 'Spoofing ACK Packets' : plot_config.colors[2],
    # 'Spoofing UDP Packets' : plot_config.colors[3]
}

patchList = []
for key in legend_dict:
        data_key = mpatches.Patch(color=legend_dict[key], label=key)
        patchList.append(data_key)

plt.legend(handles=patchList, bbox_to_anchor=(0, 1.02, 1, 0.2), loc='lower left', fontsize=plot_config.font_size, ncol=2, mode='expand')

plt.tick_params(
    axis='x',          # changes apply to the x-axis
    which='both',      # both major and minor ticks are affected
    bottom=False,      # ticks along the bottom edge are off
    top=False,         # ticks along the top edge are off
    labelbottom=False) # labels along the bottom edge are 'xx-large'
for label in ax.yaxis.get_ticklabels():
    label.set_fontsize(plot_config.font_size)

# # plt.xlabel('Time(s)')
plt.ylabel('Speed (Kpps)', fontsize=plot_config.font_size)

bx = plt.subplot(212)

bx.xaxis.set_major_locator(xmajorLocator)
bx.xaxis.set_major_formatter(xmajorFormatter)

bx.yaxis.set_major_locator(ymajorLocator)
bx.yaxis.set_major_formatter(ymajorFormatter)

bx.xaxis.set_minor_locator(xminorLocator)
bx.yaxis.set_minor_locator(yminorLocator)

bx.xaxis.grid(True, which='major', ls='dotted')
bx.yaxis.grid(True, which='major', ls='dotted')


bx.annotate('Switch to Filtering State', xy=(entering_filtering, 7.5),
            xytext=(entering_filtering + 0.5, 9), fontsize=plot_config.font_size,
            arrowprops=dict(facecolor=plot_config.colors[1], arrowstyle='fancy',
                connectionstyle="arc3,rad=0.2"),
)
bx.text(end_time - 1.0 * end_time / 60, 38, "Server Side", ha="right", va="top", size=plot_config.font_size,
        bbox=bbox_props)

x,y = np.array([[attack_start - 0.05, attack_start - 0.05], [-3, 83]])
line0 = mpl.lines.Line2D(x, y, lw=1, color='grey', linestyle='-.', alpha=0.8)
line0.set_clip_on(False)
bx.add_line(line0)
x,y = np.array([[attack_stop, attack_stop], [-3, 83]])
line1 = mpl.lines.Line2D(x, y, lw=1, color='grey', linestyle='-.', alpha=0.8)
line1.set_clip_on(False)
bx.add_line(line1)
x,y = np.array([[entering_filtering, entering_filtering], [-3, 83]])
line2 = mpl.lines.Line2D(x, y, lw=1, color='grey', linestyle='-.', alpha=0.8)
line2.set_clip_on(False)
bx.add_line(line2)


plt.plot(timeline_receiver, receiver_pps[3][start_time * 100 : end_time * 100])
plt.plot(timeline_receiver, receiver_pps[0][start_time * 100 : end_time * 100])
# plt.plot(timeline_receiver, receiver_pps[1][start_time * 100 : end_time * 100])
# plt.plot(timeline_receiver, receiver_pps[2][start_time * 100 : end_time * 100])

plt.ylim(0, 40)
plt.xlim(start_time, end_time)


for label in bx.xaxis.get_ticklabels():
    label.set_fontsize(plot_config.font_size)
# plt.tick_params(
#     axis='x',          # changes apply to the x-axis
#     which='both',      # both major and minor ticks are affected
#     bottom=False,      # ticks along the bottom edge are off
#     top=False,         # ticks along the top edge are off
#     labelbottom=False) # labels along the bottom edge are off
for label in bx.yaxis.get_ticklabels():
    label.set_fontsize(plot_config.font_size)

plt.xlabel('Time(s)', fontsize=plot_config.font_size)
plt.ylabel('Speed (Kpps)', fontsize=plot_config.font_size)


plt.tight_layout(rect=[0, 0, 1, 1])
plt.subplots_adjust(wspace=0, hspace=0.05)
plt.savefig('bandwidth_saving.pdf')
plt.show()
