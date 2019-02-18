import os

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
	with open(file_path, 'r') as file:
		lines = file.readlines()
	for line in lines:
		split_str = line.split()
		addtodict2(tenant_name, split_str[2], split_str[0], int(split_str[1]))
		addtodict2(time_name, split_str[0], split_str[2], int(split_str[1]))
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
		tenant_file = open('traffic_B_tenant.txt', 'w')
		tenant_file.write(str(tenant_dic))
		tenant_file.close()
		time_file = open('traffic_B_time.txt', 'w')
		time_file.write(str(time_dic))
		time_file.close()



def main():
	dir = '/data0/traffic/traffic_B'
	read_dir(dir)

if __name__ == "__main__":
    main()