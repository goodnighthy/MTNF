import copy
import random

thres = 4.2

flowlist = []

#sizelist = [0, 1, 2, 3, 4]
sizelist = [0.2, 0.7, 1.2, 1.7, 2.2, 2.7, 3.2, 3.7, 4.2]
seqlist = []

worker_num = 4
tenant_num = 8
total_flow = 10

def traverse(fl, ptr):
    if ptr == tenant_num:
        flow_sum = 0
        for i in range(tenant_num):
            flow_sum = flow_sum + fl[i]
        if flow_sum < total_flow:
            flowlist.append(copy.copy(fl))
    else:
        for i in range(len(sizelist)):
            fl[ptr] = sizelist[i]
            traverse(fl, ptr + 1)

def traverse2(seq, select_seq, ptr):
    if ptr == tenant_num:
        seqlist.append(copy.copy(seq))
    else:
        for i in range(tenant_num):
            if select_seq[i] == False:
                seq[ptr] = i
                select_seq[i] = True
                traverse2(seq, select_seq, ptr + 1)
                select_seq[i] = False

_fl = []
_seq = []
_select_seq = []
tenant = []
worker = []
for i in range(0, tenant_num):
    _fl.append(0)
    _seq.append(0)
    _select_seq.append(False)
    tenant.append(i / 2)

worker = []
for i in range(0, worker_num):
    worker.append(0)

#traverse(_fl, 0)
traverse2(_seq, _select_seq, 0)


def pktgen():
    return random.choice(flowlist), random.choice(seqlist)

def smartNic(fl, seq):
    for i in range(4):
        worker[i] = 0

    for i in range(8):
        worker[tenant[i]] = worker[tenant[i]] + fl[i]
    
    for i in range(4):
        if worker[i] > thres:
            return True
    
    return False

def scheSmartNic(fl, seq):
    for i in range(worker_num):
        worker[i] = 0

    for i in range(tenant_num):
        worker[tenant[i]] = worker[tenant[i]] + fl[i]

    for i in range(tenant_num):
        tenant_id = seq[i]
        if worker[tenant[tenant_id]] > thres:
            _min = 2 ** 20
            _min_ptr = 0
            for j in range(worker_num):
                if worker[j] < _min:
                    _min = worker[j]
                    _min_ptr = j
            
            if worker[_min_ptr] + fl[tenant_id] < thres:
                worker[tenant[tenant_id]] = worker[tenant[tenant_id]] - fl[tenant_id]
                worker[_min_ptr] = worker[_min_ptr] + fl[tenant_id]
                tenant[tenant_id] = _min_ptr

    for i in range(worker_num):
        if worker[i] > thres:
            return True
    
    return False

_ = 10000

cnt1 = 0
cnt2 = 0
stable_time = 3
#print len(flowlist)

def genNewFl():
    div = []
    _sum = 0
    tmp_size = 0
    for i in range(tenant_num - 1):
        tmp_size = random.randint(1, min(40, 100 - _sum - (tenant_num - i - 1)))
        _sum = _sum + tmp_size
        div.append(round(tmp_size * 0.1, 1))
    div.append(round((100 - _sum) * 0.1, 1))
    return div


total_cnt = 0
#for i in range(100):
while True:
    f = open("./res.txt", "a")
    total_cnt = total_cnt + 1
#    tmp_fl, tmp_seq = pktgen()
#    tmp_fl = flowlist[i]
    tmp_fl = genNewFl()
    tmp_seq = random.choice(seqlist)
    for _t in range(tenant_num):
        tenant[_t] = _t / 2

    if smartNic(tmp_fl, tmp_seq):
        cnt1 = cnt1 + 1

    for slot in range(stable_time):
        scheSmartNic(tmp_fl, random.choice(seqlist))
    if scheSmartNic(tmp_fl, random.choice(seqlist)):
        cnt2 = cnt2 + 1
    if total_cnt % 100 == 0:
        f.write("total: " + str(total_cnt) + \
                "; old:" + str(cnt1 * 100.0 / total_cnt) + " - " + str(cnt1) + \
                "; new:" + str(cnt2 * 100.0 / total_cnt) + " - " + str(cnt2) + "\n")
    f.close()


'''
for i in range(200):
    tmp_fl = genNewFl()
    x = 0
    for j in range(len(tmp_fl)):
        x = x + tmp_fl[j]
    print tmp_fl, x
'''