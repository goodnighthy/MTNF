import os
import numpy as numpy
#import matplotlib.pyplot as plt
import json
from scapy.all import *
import time

def write_to_file(xval, yval, fn):
	with open(fn, 'w') as f:
		write_data = [list(xval), yval]
		write_data = json.dumps(write_data)
		f.write(write_data)

def read_from_file(fn):
	with open(fn, "r") as f:
		read_data = f.read()
		read_data = json.loads(read_data)
	return read_data[0], read_data[1]

def addtodict2(thedict, key_a, key_b, value): 
    if key_a in thedict:
    	if key_b in thedict[key_a]:
    		thedict[key_a].update({key_b: thedict[key_a][key_b] + value})
    	else:
        	thedict[key_a].update({key_b: value})
    else:
        thedict.update({key_a:{key_b: value}})

def read_file(tenant_name, time_name, dir_name, file_name):
	file_path = dir_name + '/' + file_name
	if os.path.isfile(file_path) and file_path.find("bybykey") == -1:
		with open(file_path, 'r') as file:
			lines = file.readlines()
		for line in lines:
			split_str = line.split()
			addtodict2(tenant_name, split_str[3], split_str[0], int(split_str[1]))
			addtodict2(time_name, split_str[0], split_str[3], int(split_str[1]))
		# print(split_str[2], split_str[0], int(split_str[1]))
	# print(tenant_dic)

def read_dir(dir_name):
	for dirpath, dirnames, filenames in os.walk(dir_name):
		file_num = 0
		tenant_dic = dict()
		time_dic = dict()
		for file in filenames:
			read_file(tenant_dic, time_dic, dirpath, file)
			file_num = file_num + 1
			print('file num:', file_num, 'file:', dirpath, file, 'finished!')
			if file_num == 20:
				break
		task_1_dic = parse_task_1and2(tenant_dic)
		parse_task_3(time_dic, task_1_dic)

def read_pcap(tenant_dic, time_dic, file_name):
	i = 0
	for packet in PcapReader(file_name):
		print("processing", i)
		if 'IP' in packet:
			ip = packet['IP'].dst
			time_data = str(int(packet.time))
			packet_len = len(packet)
			addtodict2(tenant_dic, ip, time_data, packet_len)
			addtodict2(time_dic, time_data, ip, packet_len)
			i = i + 1

def read_pcap_dir(dir_name):
	for dirpath, dirnames, filenames in os.walk(dir_name):
		file_num = 0
		tenant_dic = dict()
		time_dic = dict()
		for fn in filenames:
			read_pcap(tenant_dic, time_dic, dirpath + '/' + fn)
			file_num = file_num + 1
			print('file num:', file_num, 'file:', dirpath, fn, 'finished!')
		#	if file_num == 20:
		#		break
		task_1_dic = parse_task_1and2(tenant_dic)
		parse_task_3(time_dic, task_1_dic)

def read_single_dir(file_name):
	tenant_dic = dict()
	time_dic = dict()
	read_pcap(tenant_dic, time_dic, file_name)
	task_1_dic = parse_task_1and2(tenant_dic)
	parse_task_3(time_dic, task_1_dic)

# to prove that the avg traffic of each tenant isn't that large
# can calc max and avg
# to prove that the each tenant's traffic concentrate on some peeks
# print cdf
def parse_task_1and2(tenant_dic):
	thres_cnt = 0
	thres_3_cnt = 0
	thres_5_cnt = 0
	thres_10_cnt = 0
	thres_20_cnt = 0
	thres_30_cnt = 0
	thres_50_cnt = 0
	thres_whole_cnt = 0
	whole_max_max = 0
	whole_max_avg = 0
	whole_max_sum = 0
	tenant_task_data = dict()
	task2_xval = range(101)
	task2_pdf_yval = [0] * 101
	task2_cdf_yval = [0] * 101
	task2_cnt = 0
	for tenant_id in tenant_dic:
		traffic_max = 0
		traffic_avg = 0
		traffic_sum = 0
		traffic_timelist = []

		for time_data in tenant_dic[tenant_id]:
			traffic_val = tenant_dic[tenant_id][time_data]	
			traffic_sum = traffic_sum + traffic_val
			traffic_timelist.append(int(time_data))
			if traffic_max < traffic_val:
				traffic_max = traffic_val

		thres_whole_cnt = thres_whole_cnt + 1
		if len(traffic_timelist) < 3:
			thres_3_cnt = thres_3_cnt + 1
		if len(traffic_timelist) < 5:
			thres_5_cnt = thres_5_cnt + 1
		if len(traffic_timelist) < 10:
			thres_10_cnt = thres_10_cnt + 1
		if len(traffic_timelist) < 20:
			thres_20_cnt = thres_20_cnt + 1
		if len(traffic_timelist) < 30:
			thres_30_cnt = thres_30_cnt + 1
		if len(traffic_timelist) < 50:
			thres_50_cnt = thres_50_cnt + 1

		if len(traffic_timelist) >= 20:
			thres_cnt = thres_cnt + 1

		if len(traffic_timelist) >= 20:
			for time_data in traffic_timelist:
				index = int(round(float(tenant_dic[tenant_id][str(time_data)]) / traffic_max, 2) * 100)
				task2_pdf_yval[index] = task2_pdf_yval[index] + 1
				task2_cnt = task2_cnt + 1

		traffic_timelist.sort()
		traffic_avg = traffic_sum / len(traffic_timelist)

		if whole_max_max < traffic_max:
			whole_max_max = traffic_max
		if whole_max_avg < traffic_avg:
			whole_max_avg = traffic_avg
		if whole_max_sum < traffic_sum:
			whole_max_sum = traffic_sum

		# save traffic data(vary by time)
		tenant_task_data[tenant_id] = {"max": traffic_max, "avg": traffic_avg, \
									"sum": traffic_sum, "cnt": len(traffic_timelist)}

	print("threshold cnt:", thres_cnt)
	# here calculate all avg and max data
	# result is two list
	# avg's bin is 1
	# max's bin is 1

	# when the data is small
	'''
	avg_xval = range(int(whole_max_avg) + 1)
	avg_pdf_yval = [0] * len(avg_xval)
	avg_cdf_yval = [0] * len(avg_xval)
	max_xval = range(int(whole_max_max) + 1)
	max_pdf_yval = [0] * len(max_xval)
	max_cdf_yval = [0] * len(max_xval)
	sum_xval = range(int(whole_max_sum) + 1)
	sum_pdf_yval = [0] * len(sum_xval)
	sum_cdf_yval = [0] * len(sum_xval)
	'''
	# when the data is large
	avg_xval = [0] * 10001
	avg_pdf_yval = [0] * len(avg_xval)
	avg_cdf_yval = [0] * len(avg_xval)
	max_xval = [0] * 10001
	max_pdf_yval = [0] * len(max_xval)
	max_cdf_yval = [0] * len(max_xval)
	sum_xval = [0] * 10001
	sum_pdf_yval = [0] * len(sum_xval)
	sum_cdf_yval = [0] * len(sum_xval)
	for i in range(10001):
		avg_xval[i] = whole_max_avg / 10000 * i
		max_xval[i] = whole_max_max / 10000 * i
		sum_xval[i] = whole_max_sum / 10000 * i

	whole_cnt = 0
	for tenant_id in tenant_dic: # calculate pdf
		whole_cnt = whole_cnt + 1
		avg_index = int(int(tenant_task_data[tenant_id]["avg"]) * 10000 / whole_max_avg)
		avg_pdf_yval[avg_index] = avg_pdf_yval[avg_index] + 1
		max_index = int(int(tenant_task_data[tenant_id]["max"]) * 10000 / whole_max_max)
		max_pdf_yval[max_index] = max_pdf_yval[max_index] + 1
		sum_index = int(int(tenant_task_data[tenant_id]["sum"]) * 10000 / whole_max_sum)
		sum_pdf_yval[sum_index] = sum_pdf_yval[sum_index] + 1
	
	# calculate cdf
	tmpsum = 0
	for i in range(len(avg_pdf_yval)):
		tmpsum = tmpsum + avg_pdf_yval[i]
		avg_cdf_yval[i] = float(tmpsum) / whole_cnt
	tmpsum = 0
	for i in range(len(max_pdf_yval)):
		tmpsum = tmpsum + max_pdf_yval[i]
		max_cdf_yval[i] = float(tmpsum) / whole_cnt
	tmpsum = 0
	for i in range(len(sum_pdf_yval)):
		tmpsum = tmpsum + sum_pdf_yval[i]
		sum_cdf_yval[i] = float(tmpsum) / whole_cnt
	tmpsum = 0
	for i in range(101):
		tmpsum = tmpsum + task2_pdf_yval[i]
		task2_cdf_yval[i] = float(tmpsum) / task2_cnt

	write_to_file(task2_xval, task2_cdf_yval, "task2.txt")
	write_to_file(sum_xval, sum_cdf_yval, "sum.txt")
	write_to_file(max_xval, max_cdf_yval, "max.txt")
	write_to_file(avg_xval, avg_cdf_yval, "avg.txt")

	with open("tenant_data.txt", "a+") as f:
		f.write("/data0/traffic/traffic_B/:\n")
		f.write("lessthan_3: " + str(thres_3_cnt) + " ")
		f.write("lessthan_5: " + str(thres_5_cnt) + " ")
		f.write("lessthan_10: " + str(thres_10_cnt) + " ")
		f.write("lessthan_20: " + str(thres_20_cnt) + " ")
		f.write("lessthan_30: " + str(thres_30_cnt) + " ")
		f.write("lessthan_50: " + str(thres_50_cnt) + " ")
		f.write("task1and3_cnt: " + str(thres_whole_cnt) + " ")
		f.write("task2_cnt: " + str(task2_cnt) + "\n")

	return tenant_task_data

# to prove that only few tenants' traffic reach its peek
def parse_task_3(time_dict, prev_data):
	time_list = []
	for time_id in time_dict:
		time_list.append(int(time_id))
	time_list.sort()
	tenant_number_80 = [0] * len(time_list)

	whole_max = 0
	for index in range(len(time_list)):
		time_id = str(time_list[index])
		for tenant_id in time_dict[time_id]:
			if prev_data[tenant_id]["cnt"] >= 5:
				if time_dict[time_id][tenant_id] > float(prev_data[tenant_id]["max"]) * 0.8:
					tenant_number_80[index] = tenant_number_80[index] + 1
		if whole_max < tenant_number_80[index]:
			whole_max = tenant_number_80[index]
		
	time_xval = range(whole_max + 1)
	time_pdf_yval = [0] * (whole_max + 1)
	time_cdf_yval = [0] * (whole_max + 1)

	# calculate pdf and cdf
	cnt = len(tenant_number_80)
	tmp_sum = 0
	for tenant_number in tenant_number_80:
		time_pdf_yval[tenant_number] = time_pdf_yval[tenant_number] + 1
	for index in range(whole_max + 1):
		tmp_sum = tmp_sum + time_pdf_yval[index]
		time_cdf_yval[index] = float(tmp_sum) / cnt

	write_to_file(time_xval, time_cdf_yval, "time.txt")

def main():
#	dir = './data/'
#	dir = '/data0/traffic/traffic_A'
	dir = '/data0/traffic/traffic_B/'
#	dir = '/data0/traffic/traffic_C'
#	file_name = '/data0/traffic/baidu/new_idcebgw4_00018_20180110152913'
#	file_name = '/data0/equinix-chicago.dirA.20160121-125911.UTC.anon.pcap'
#	file_name = '/data0/equinix-chicago.dirB.20160121-135641.UTC.anon.pcap'
#	dir = '/data0/traffic/baidu/'
	read_dir(dir)
#	read_pcap(file_name)
#	read_pcap_dir(dir)
#	read_single_dir(file_name)

if __name__ == "__main__":
    main()