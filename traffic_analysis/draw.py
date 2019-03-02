# -*- coding: utf-8 -*-

import os
import numpy as numpy
import matplotlib.pyplot as plt
import json

def read_from_file(fn):
	with open(fn, "r") as f:
		read_data = f.read()
		read_data = json.loads(read_data)
	return read_data[0], read_data[1]

def main():
    max_xval, max_cdf_yval = read_from_file("max.txt")
    sum_xval, sum_cdf_yval = read_from_file("sum.txt")
    avg_xval, avg_cdf_yval = read_from_file("avg.txt")
    task2_xval, task2_cdf_yval = read_from_file("task2.txt")
    task3_xval, task3_cdf_yval = read_from_file("time.txt")
    
    plt.plot(max_xval, max_cdf_yval)
    plt.xlabel(u'最大流量', fontproperties='SimHei')
    plt.show()

    plt.plot(sum_xval, sum_cdf_yval)
    plt.xlabel(u'总流量', fontproperties='SimHei')
    plt.show()

    plt.plot(avg_xval, avg_cdf_yval)
    plt.xlabel(u'平均流量', fontproperties='SimHei')
    plt.show()

    plt.plot(task2_xval, task2_cdf_yval)
    plt.xlabel(u'租户出现过的流量', fontproperties='SimHei')
    plt.show()

    plt.plot(task3_xval, task3_cdf_yval)
    plt.xlabel(u'时间', fontproperties='SimHei')
    plt.ylabel(u'租户出现最大流量的80%', fontproperties='SimHei')
    plt.show()

if __name__ == "__main__":
    main()