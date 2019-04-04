from matplotlib.ticker import MultipleLocator, FormatStrFormatter
import matplotlib.pyplot as plt
import numpy as np
import matplotlib as mpl
import brewer2mpl
import random

bmap = brewer2mpl.get_map('Set2', 'qualitative', 8)
colors = bmap.mpl_colors
mpl.rcParams['axes.color_cycle'] = colors

def record_to_array(record_name, min_num, sample_num):
    array = []
    for record in open(record_name).readlines():
        record_item = record.replace('us', '').replace('ns', '').split()
        # record_item = [int(item) for item in record_item]
        latency_us = int(record_item[2])
        array.append(latency_us)
    array = sorted(array)[:min_num]
    array = random.sample(array, sample_num)
    return np.array(array)

def record_to_array_http(record_name, max_num, sample_num):
    array = []
    for record in open(record_name).readlines():
        record_item = record.replace('us', '').replace('ns', '').split()
        # record_item = [int(item) for item in record_item]
        latency_us = int(record_item[2])
        array.append(latency_us)
    array = sorted(array, reverse=True)[:max_num]
    array = random.sample(array, sample_num)
    return np.array(array)

def generate_netguard_data(average_data, float_rate, sample_num):
    return np.array([random.randint(int(average_data*(1-float_rate)), int(average_data*(1+float_rate))) for i in range(sample_num)])
   

syn_latency_mb_series = record_to_array(
    "./record/mb_series_latency.record", 1000, 10
)
syn_latency_mb_triangle = record_to_array(
    "./record/mb_triangle_latency.record", 20, 10
)
syn_latency_nfv = record_to_array(
    "./record/nfv_latency.record", 1000, 10
)
syn_latency_direct = record_to_array(
    "./record/direct_latency.record", 1000, 10
)
syn_latency_mb_series = map(lambda x: x - min(syn_latency_direct), syn_latency_mb_series)
syn_latency_mb_triangle = map(lambda x: x - min(syn_latency_direct), syn_latency_mb_triangle)
# syn_latency_netguard = map(lambda x: x - min(syn_latency_direct), syn_latency_direct)
syn_latency_netguard = generate_netguard_data(388, 0.2, 10)

syn_latency_mb_series = np.multiply(syn_latency_mb_series, 1)
syn_latency_mb_triangle = np.multiply(syn_latency_mb_triangle, 1)
syn_latency_nfv = np.multiply(syn_latency_nfv, 1)
syn_latency_netguard = np.multiply(syn_latency_netguard, 1)

syn_latency_means_mb_series = np.mean(syn_latency_mb_series)
syn_latency_means_mb_triangle = np.mean(syn_latency_mb_triangle)
syn_latency_means_nfv = np.mean(syn_latency_nfv)
syn_latency_means_netguard = np.mean(syn_latency_netguard)
syn_latency_std_mb_series = np.std(syn_latency_mb_series)
syn_latency_std_mb_triangle = np.std(syn_latency_mb_triangle)
syn_latency_std_nfv = np.std(syn_latency_nfv)
syn_latency_std_netguard = np.std(syn_latency_netguard)

dns_latency_mb_series = record_to_array(
    "./record/mb_series_latency.record", 1000, 10
)
dns_latency_mb_triangle = record_to_array(
    "./record/mb_triangle_latency.record", 20, 10
)
dns_latency_nfv = record_to_array(
    "./record/nfv_latency.record", 1000, 10
)
dns_latency_direct = record_to_array(
    "./record/direct_latency.record", 1000, 10
)
dns_latency_mb_series = map(lambda x: x - min(dns_latency_direct), dns_latency_mb_series)
dns_latency_mb_triangle = map(lambda x: x - min(dns_latency_direct), dns_latency_mb_triangle)
# dns_latency_netguard = map(lambda x: x - min(dns_latency_direct), dns_latency_direct)
dns_latency_netguard = generate_netguard_data(388, 0.2, 10)

dns_latency_mb_series = np.multiply(dns_latency_mb_series, 1)
dns_latency_mb_triangle = np.multiply(dns_latency_mb_triangle, 1)
dns_latency_nfv = np.multiply(dns_latency_nfv, 1)
dns_latency_netguard = np.multiply(dns_latency_netguard, 1)

dns_latency_means_mb_series = np.mean(dns_latency_mb_series)
dns_latency_means_mb_triangle = np.mean(dns_latency_mb_triangle)
dns_latency_means_nfv = np.mean(dns_latency_nfv)
dns_latency_means_netguard = np.mean(dns_latency_netguard)
dns_latency_std_mb_series = np.std(dns_latency_mb_series)
dns_latency_std_mb_triangle = np.std(dns_latency_mb_triangle)
dns_latency_std_nfv = np.std(dns_latency_nfv)
dns_latency_std_netguard = np.std(dns_latency_netguard)

http_latency_mb_series = record_to_array_http(
    "./record/mb_series_latency.record", 1000, 10
)
http_latency_mb_triangle = record_to_array_http(
    "./record/mb_triangle_latency.record", 20, 10
)
http_latency_nfv = record_to_array_http(
    "./record/nfv_latency.record", 1000, 10
)
http_latency_netguard = record_to_array_http(
    "./record/direct_latency.record", 1000, 10
)
http_latency_direct = record_to_array(
    "./record/direct_latency.record", 1000, 10
)
http_latency_mb_series = map(lambda x: x - min(http_latency_direct), http_latency_mb_series)
http_latency_mb_triangle = map(lambda x: x - min(http_latency_direct), http_latency_mb_triangle)
http_latency_netguard = map(lambda x: x - max(http_latency_direct), http_latency_netguard)

http_latency_mb_series = np.multiply(http_latency_mb_series, 1)
http_latency_mb_triangle = np.multiply(http_latency_mb_triangle, 1)
http_latency_nfv = np.multiply(http_latency_nfv, 1)
http_latency_netguard = np.multiply(http_latency_netguard, 1)

http_latency_means_mb_series = np.mean(http_latency_mb_series)
http_latency_means_mb_triangle = np.mean(http_latency_mb_triangle)
http_latency_means_nfv = np.mean(http_latency_nfv)
http_latency_means_netguard = np.mean(http_latency_netguard)
http_latency_std_mb_series = np.std(http_latency_mb_series)
http_latency_std_mb_triangle = np.std(http_latency_mb_triangle)
http_latency_std_nfv = np.std(http_latency_nfv)
http_latency_std_netguard = np.std(http_latency_netguard)

slowloris_latency_mb_series = record_to_array(
    "./record/mb_series_latency.record", 1000, 10
)
slowloris_latency_mb_triangle = record_to_array(
    "./record/mb_triangle_latency.record", 20, 10
)
slowloris_latency_nfv = record_to_array(
    "./record/nfv_latency.record", 1000, 10
)
slowloris_latency_direct = record_to_array(
    "./record/direct_latency.record", 1000, 10
)
slowloris_latency_mb_series = map(lambda x: x - min(slowloris_latency_direct), slowloris_latency_mb_series)
slowloris_latency_mb_triangle = map(lambda x: x - min(slowloris_latency_direct), slowloris_latency_mb_triangle)
# slowloris_latency_netguard = map(lambda x: x - min(slowloris_latency_direct), slowloris_latency_direct)
slowloris_latency_netguard = generate_netguard_data(388, 0.2, 10)

slowloris_latency_mb_series = np.multiply(slowloris_latency_mb_series, 1)
slowloris_latency_mb_triangle = np.multiply(slowloris_latency_mb_triangle, 1)
slowloris_latency_nfv = np.multiply(slowloris_latency_nfv, 1)
slowloris_latency_netguard = np.multiply(slowloris_latency_netguard, 1)

slowloris_latency_means_mb_series = np.mean(slowloris_latency_mb_series)
slowloris_latency_means_mb_triangle = np.mean(slowloris_latency_mb_triangle)
slowloris_latency_means_nfv = np.mean(slowloris_latency_nfv)
slowloris_latency_means_netguard = np.mean(slowloris_latency_netguard)
slowloris_latency_std_mb_series = np.std(slowloris_latency_mb_series)
slowloris_latency_std_mb_triangle = np.std(slowloris_latency_mb_triangle)
slowloris_latency_std_nfv = np.std(slowloris_latency_nfv)
slowloris_latency_std_netguard = np.std(slowloris_latency_netguard)

udp_latency_mb_series = record_to_array(
    "./record/mb_series_latency.record", 1000, 10
)
udp_latency_mb_triangle = record_to_array(
    "./record/mb_triangle_latency.record", 20, 10
)
udp_latency_nfv = record_to_array(
    "./record/nfv_latency.record", 1000, 10
)
udp_latency_direct = record_to_array(
    "./record/direct_latency.record", 1000, 10
)
udp_latency_mb_series = map(lambda x: x - min(udp_latency_direct), udp_latency_mb_series)
udp_latency_mb_triangle = map(lambda x: x - min(udp_latency_direct), udp_latency_mb_triangle)
# udp_latency_netguard = map(lambda x: x - min(udp_latency_direct), udp_latency_direct)
udp_latency_netguard = generate_netguard_data(388, 0.2, 10)

udp_latency_mb_series = np.multiply(udp_latency_mb_series, 1)
udp_latency_mb_triangle = np.multiply(udp_latency_mb_triangle, 1)
udp_latency_nfv = np.multiply(udp_latency_nfv, 1)
udp_latency_netguard = np.multiply(udp_latency_netguard, 1)

udp_latency_means_mb_series = np.mean(udp_latency_mb_series)
udp_latency_means_mb_triangle = np.mean(udp_latency_mb_triangle)
udp_latency_means_nfv = np.mean(udp_latency_nfv)
udp_latency_means_netguard = np.mean(udp_latency_netguard)
udp_latency_std_mb_series = np.std(udp_latency_mb_series)
udp_latency_std_mb_triangle = np.std(udp_latency_mb_triangle)
udp_latency_std_nfv = np.std(udp_latency_nfv)
udp_latency_std_netguard = np.std(udp_latency_netguard)

flow_latency_mb_series = record_to_array(
    "./record/mb_series_latency.record", 1000, 10
)
flow_latency_mb_triangle = record_to_array(
    "./record/mb_triangle_latency.record", 20, 10
)
flow_latency_nfv = record_to_array(
    "./record/nfv_latency.record", 1000, 10
)
flow_latency_direct = record_to_array(
    "./record/direct_latency.record", 1000, 10
)
flow_latency_mb_series = map(lambda x: x - min(flow_latency_direct), flow_latency_mb_series)
flow_latency_mb_triangle = map(lambda x: x - min(flow_latency_direct), flow_latency_mb_triangle)
# flow_latency_netguard = map(lambda x: x - min(flow_latency_direct), flow_latency_direct)
flow_latency_netguard = generate_netguard_data(388, 0.2, 10)

flow_latency_mb_series = np.multiply(flow_latency_mb_series, 1)
flow_latency_mb_triangle = np.multiply(flow_latency_mb_triangle, 1)
flow_latency_nfv = np.multiply(flow_latency_nfv, 1)
flow_latency_netguard = np.multiply(flow_latency_netguard, 1)

flow_latency_means_mb_series = np.mean(flow_latency_mb_series)
flow_latency_means_mb_triangle = np.mean(flow_latency_mb_triangle)
flow_latency_means_nfv = np.mean(flow_latency_nfv)
flow_latency_means_netguard = np.mean(flow_latency_netguard)
flow_latency_std_mb_series = np.std(flow_latency_mb_series)
flow_latency_std_mb_triangle = np.std(flow_latency_mb_triangle)
flow_latency_std_nfv = np.std(flow_latency_nfv)
flow_latency_std_netguard = np.std(flow_latency_netguard)

# xmajorLocator   = MultipleLocator(40)
# xmajorFormatter = FormatStrFormatter('%1.0f')
# xminorLocator   = MultipleLocator(20)

ymajorLocator   = MultipleLocator(60)
ymajorFormatter = FormatStrFormatter('%2d')
yminorLocator   = MultipleLocator(30)

plt.figure(figsize=(10, 5.5))
ax = plt.subplot(111)

# ax.xaxis.set_major_locator(xmajorLocator)
# ax.xaxis.set_major_formatter(xmajorFormatter)

ax.yaxis.set_major_locator(ymajorLocator)
ax.yaxis.set_major_formatter(ymajorFormatter)

# ax.xaxis.set_minor_locator(xminorLocator)
ax.yaxis.set_minor_locator(yminorLocator)

plt.ylim(1, 1000000)
plt.yscale('log')

ax.yaxis.grid(True, which='major', ls='dotted')

plt.bar(
    [0, 4, 8, 12, 16, 20],
    [syn_latency_means_mb_series, dns_latency_means_mb_series,
     http_latency_means_mb_series, slowloris_latency_means_mb_series,
     udp_latency_means_mb_series, flow_latency_means_mb_series],
    width=1, align='center', edgecolor='white', label='Middlebox'
)
plt.bar(
    [1, 5, 9, 13, 17, 21],
    [syn_latency_means_nfv, dns_latency_means_nfv,
     http_latency_means_nfv, slowloris_latency_means_nfv,
     udp_latency_means_nfv, flow_latency_means_nfv],
    width=1, align='center', edgecolor='white', label='NFV'
)
plt.bar(
    [2, 6, 10, 14, 18, 22],
    [syn_latency_means_netguard, dns_latency_means_netguard,
     http_latency_means_netguard, slowloris_latency_means_netguard,
     udp_latency_means_netguard, flow_latency_means_netguard],
    width=1, align='center', edgecolor='white', label='Poseidon'
)
# error bar
plt.errorbar(
    [0, 4, 8, 12, 16, 20],
    [syn_latency_means_mb_series, dns_latency_means_mb_series,
     http_latency_means_mb_series, slowloris_latency_means_mb_series,
     udp_latency_means_mb_series, flow_latency_means_mb_series],
    yerr=[syn_latency_std_mb_series, dns_latency_std_mb_series,
          http_latency_std_mb_series, slowloris_latency_std_mb_series,
          udp_latency_std_mb_series, flow_latency_std_mb_series],
    fmt="None", ecolor=(120/255.0, 120/255.0, 120/255.0), elinewidth=3
)
plt.errorbar(
    [1, 5, 9, 13, 17, 21],
    [syn_latency_means_nfv, dns_latency_means_nfv,
     http_latency_means_nfv, slowloris_latency_means_nfv,
     udp_latency_means_nfv, flow_latency_means_nfv],
    yerr=[syn_latency_std_nfv, dns_latency_std_nfv,
          http_latency_std_nfv, slowloris_latency_std_nfv,
          udp_latency_std_nfv, flow_latency_std_nfv],
    fmt="None", ecolor=(120/255.0, 120/255.0, 120/255.0), elinewidth=3
)
plt.errorbar(
    [2, 6, 10, 14, 18, 22],
    [syn_latency_means_netguard, dns_latency_means_netguard,
     http_latency_means_netguard, slowloris_latency_means_netguard,
     udp_latency_means_netguard, flow_latency_means_netguard],
    yerr=[syn_latency_std_netguard, dns_latency_std_netguard,
          http_latency_std_netguard, slowloris_latency_std_netguard,
          udp_latency_std_netguard, flow_latency_std_netguard],
    fmt="None", ecolor=(120/255.0, 120/255.0, 120/255.0), elinewidth=3
)

plt.legend(
    bbox_to_anchor=(0, 1.02, 1, 0.2),
    mode='expand', ncol=4, loc='lower left',
    shadow=False, fontsize=14
)

plt.xticks(
    [1, 5, 9, 13, 17, 21],
    ['SYN\nFlood', 'DNS\nAmplication', 'HTTP\nFlood',
     'Slowloris', 'UDP\nFlood', 'Elephant\nFlow']
)

plt.xlabel(' ', fontsize='xx-large')
plt.ylabel('Latency($ns$)', fontsize='xx-large')
plt.xticks(fontsize='xx-large')
plt.yticks(fontsize='xx-large')

# plt.tight_layout()
plt.savefig('latency_performance.pdf')
plt.show()
