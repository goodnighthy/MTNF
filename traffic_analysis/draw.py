# -*- coding: utf-8 -*-

import os
import numpy as numpy
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import json
from cycler import cycler

def read_from_file(fn):
	with open(fn, "r") as f:
		read_data = f.read()
		read_data = json.loads(read_data)
	return read_data[0], read_data[1]

#def draw_single_pic(x_val, y_val, x_label, y_label):

def main():
    max_xval, max_cdf_yval = read_from_file("max.txt")
    sum_xval, sum_cdf_yval = read_from_file("sum.txt")
    avg_xval, avg_cdf_yval = read_from_file("avg.txt")
    task2_xval, task2_cdf_yval = read_from_file("task2.txt")
    task3_xval, task3_cdf_yval = read_from_file("time.txt")

    fig_size = [12, 8]
    font_size = 25
    tick_size = 15
    # colors = brewer2mpl.get_map('Dark2', 'qualitative', 8).mpl_colors
    colors = [
        (74 / 255.0, 114 / 255.0, 176 / 255.0),
        (196 / 255.0, 78 / 255.0, 82 / 255.0),
        (85 / 255.0, 168 / 255.0, 104 / 255.0),
        (129 / 255.0, 114 / 255.0, 178 / 255.0),
        (204 / 255.0, 185 / 255.0, 116 / 255.0),
        (100 / 255.0, 181 / 255.0, 205 / 255.0)
    ]
    plt.rc('axes', prop_cycle=(cycler('color', colors)))
    plt.rc('lines', linewidth=5)


    plt.figure(figsize=fig_size)
#    plt.title(u"租户出现的最大流量", fontsize=font_size)
#    pic, ax1 = plt.subplots(1, 1)
#    plt.plot(max_xval, max_cdf_yval)
#    plt.xlabel(u'最大流量', fontproperties='SimHei', fontsize=font_size)
#    plt.plot(sum_xval, sum_cdf_yval)
#    plt.xlabel(u'总流量', fontproperties='SimHei', fontsize=font_size)
#    plt.plot(avg_xval, avg_cdf_yval)
#    plt.xlabel(u'平均流量', fontproperties='SimHei', fontsize=font_size)
#    plt.plot(task2_xval, task2_cdf_yval)
#    plt.xlabel(u'租户出现过的流量', fontproperties='SimHei', fontsize=font_size)
    plt.plot(task3_xval, task3_cdf_yval)
    plt.xlabel(u'时间', fontproperties='SimHei', fontsize=font_size)
    plt.ylabel(u'租户出现最大流量的80%', fontproperties='SimHei', fontsize=font_size)
    ax = plt.gca()
    ax.xaxis.grid(True, which='major', ls='dotted')
    ax.yaxis.grid(True, which='major', ls='dotted')

    for label in ax.xaxis.get_ticklabels():
        label.set_fontsize(tick_size)
    for label in ax.yaxis.get_ticklabels():
        label.set_fontsize(tick_size)
    # 下面是右上角坐标
    text_x = max_xval[-1]
    text_y = 0.8
    '''
    bbox_props = dict(boxstyle="round", fc="w", ec="0.5", alpha=0.9)
    ax.text(text_x, text_y, "this is text", ha="right", va="top", 
            size=font_size, bbox=bbox_props)

    dot_x = 5000
    dot_y = 0.985
    # text是左上角坐标
    text_x = 100000
    text_y = 0.8
    ax.annotate('this is note', xy=(dot_x, dot_y),
                xytext=(text_x, text_y), fontsize=font_size,
                arrowprops=dict(arrowstyle='fancy',
                    connectionstyle="arc3,rad=0.1"),
    )
    legend_dict = {'l1': colors[0]}
    patchlist = []
    for key in legend_dict:
        data_key = mpatches.Patch(color=legend_dict[key], label=key)
        patchlist.append(data_key)
    plt.legend(handles=patchlist, loc="best", fontsize=font_size)
    '''
    plt.show()

    '''
    plt.figure(figsize=fig_size)
    plt.plot(sum_xval, sum_cdf_yval)
    plt.xlabel(u'总流量', fontproperties='SimHei', fontsize=font_size)
    plt.show()

    plt.figure(figsize=fig_size)
    plt.plot(avg_xval, avg_cdf_yval)
    plt.xlabel(u'平均流量', fontproperties='SimHei', fontsize=font_size)
    plt.show()

    plt.figure(figsize=fig_size)
    plt.plot(task2_xval, task2_cdf_yval)
    plt.xlabel(u'租户出现过的流量', fontproperties='SimHei', fontsize=font_size)
    plt.show()

    plt.figure(figsize=fig_size)
    plt.plot(task3_xval, task3_cdf_yval)
    plt.xlabel(u'时间', fontproperties='SimHei', fontsize=font_size)
    plt.ylabel(u'租户出现最大流量的80%', fontproperties='SimHei', fontsize=font_size)
    plt.show()
    '''

if __name__ == "__main__":
    main()