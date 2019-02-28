import copy
import random
import time

thres = 4.2

# gen all shuffle sequence, returns a list of sequence list
def genSeqList(tenant_num):
    seqlist = []
    _seq = [0] * tenant_num
    _select_seq = [False] * tenant_num
    def traverse(seq, select_seq, ptr):
        if ptr == tenant_num:
            seqlist.append(copy.copy(seq))
        else:
            for i in range(tenant_num):
                if select_seq[i] == False:
                    seq[ptr] = i
                    select_seq[i] = True
                    traverse(seq, select_seq, ptr + 1)
                    select_seq[i] = False
    traverse(_seq, _select_seq, 0)
    return seqlist

# default schedule
def smartNic(fl, seq, worker_num, tenant_num):
    worker = [0] * worker_num

    for i in range(tenant_num):
        worker[tenant[i]] = worker[tenant[i]] + fl[i]
    
    for i in range(worker_num):
        if worker[i] > thres:
            return True
    
    return False

# our schedule
def scheSmartNic(fl, seq, worker_num, tenant_num):
    worker = [0] * worker_num

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

# gen new flow
def genNewFl(tenant_num):
    div = []
    _sum = 0
    tmp_size = 0
    for i in range(tenant_num - 1):
        tmp_size = random.randint(1, min(40, 100 - _sum - (tenant_num - i - 1)))
        _sum = _sum + tmp_size
        div.append(round(tmp_size * 0.1, 1))
    div.append(round((100 - _sum) * 0.1, 1))
    return div

def genNewFl2(tenant_num):
    while True:
        div = []
        dot_list = [0, 10]
        for i in range(tenant_num - 1):
            # because i use randint, so i gen numbers between 1 and 99, then / 10
            # then i get flow between 0.1 and 9.9
            tmp_dot = round(float(random.randint(1, 99)) / 10, 1)
            dot_list.append(tmp_dot)
        dot_list.sort()

        #  gen flow list and check if it is legal( <= 4.0 and >= 0.1 )
        for i in range(tenant_num):
            tmp_fl_size = dot_list[i + 1] - dot_list[i]
            if tmp_fl_size <= 4.0 and tmp_fl_size > 0:
                div.append(round(dot_list[i + 1] - dot_list[i], 2))
            else:
                break
        if len(div) == tenant_num:
            break
    return div

if __name__ == '__main__':
    run_time = 10000

    start_time = time.time()

    worker_num = 4
    tenant_num = 8

    cnt1 = 0 # cnt1 is the overload cnt of default scheme
    cnt2 = 0 # cnt2 is the overload cnt of our scheme
    stable_time = 3 # run how many times before we consider the flow status as stable

    _fl = [0] * tenant_num
    tenant = [0] * tenant_num

    seqlist = genSeqList(tenant_num)

    for total_cnt in range(1, run_time):
    #    f = open("./res.txt", "a")
        # gen the new flow and the sequence of the entering order of the flow
        tmp_fl = genNewFl2(tenant_num)
        tmp_seq = random.choice(seqlist)

        # init the origin worker choice of each tenant
        for _t in range(tenant_num):
            tenant[_t] = _t / (tenant_num / worker_num)

        # check default schedule's overload
        if smartNic(tmp_fl, tmp_seq, worker_num, tenant_num):
            cnt1 = cnt1 + 1

        # run our schedule for stable time, check if it is overloaded
        for slot in range(stable_time):
            scheSmartNic(tmp_fl, random.choice(seqlist), worker_num, tenant_num)
        if scheSmartNic(tmp_fl, random.choice(seqlist), worker_num, tenant_num):
            cnt2 = cnt2 + 1

        # print data
        if total_cnt % 1000 == 0:
            print("total: " + str(total_cnt) + \
                    "; old:" + str(cnt1 * 100.0 / total_cnt) + " - " + str(cnt1) + \
                    "; new:" + str(cnt2 * 100.0 / total_cnt) + " - " + str(cnt2) + "\n")
    #    f.close()


    end_time = time.time()

    print("runtime: " + str(end_time - start_time))