eval
    主要是测试S6框架特性的，比如并行同步、stale read and write之类的，使用的方法是一些计数器的加减

sample
- accounting_app.cpp
    虽然好像是和subnet有关，但是应该就是根据每来一个包，在相应的<SubnetKey, Counter>的Map上+64（因为subnet值是手动设置的30）
- asset_monitor_app.cpp
    每到达一个包，更新asset，
    asset->type |= _AT_OS_FLAG;
    asset->os_id = random() % OS_CNT;(OS_CNT=4)
    asset->type |= _AT_SERVICE_FLAG;
    asset->service_id = random() % SERVICE_CNT;(SERVICE_CNT=5)
- echo_app.cpp
    啥也不干（一个局部变量++）
- nat_app.cpp(done)
    每有一个新的流，产生一个新的NatEntry(forward and reverse)。
    理论上：一个cluster_ip对应一个port_pool(port_pool为data_pool的封装)，对应一个 data_pool，存储一个public_ip和一个用过的port
    forward NatEntry
        new_ip:data_pool.public_ip
        new_port:找到的一个新port
        is_forward: true
    backward NatEntry
        new_ip:iph->src_addr
        new_port:tcph->src_port
        is_forward:false
    update_header
- reasm_app.cpp(done)
    get ref_entry
    模拟packet到达的顺序,重新排列
- sink_app.cpp
    drop
- snort_app.cpp
    实际上是一个ids,中间有添加flow的操作，但是后面又没有用到？？？？这写的是啥子啊
    is_malicious这个函数还没看，反正是ids的核心操作
- src_ip_counter_app.cpp
    src ip counter
- stupid_byte_counter_app.cpp
    get packet size and add to `<ipkey, counter>` map
- tcp_track_app.cpp
    根据fkey创建flow对象，统计server->client和client->server的流量大小。（flow对象的这个init_c2s和init_s2c的条件是三次握手？）
- udp_counter_global_app.cpp
- udp_counter_local_app.cpp
    上面两个代码完全一样？？？？
    对每个udp包, cnt++, 统计包大小

prads_app.cpp
    这就是论文中 提到的除了NAT和snort之外的那个prads了
    对每一个来的包很详细的统计，大概如下
	printf("-- Total packets received from libpcap    :%12u\n", pr_s.got_packets);
	printf("-- Total Ethernet packets received        :%12u\n", pr_s.eth_recv);
	printf("-- Total VLAN packets received            :%12u\n", pr_s.vlan_recv);
	printf("-- Total ARP packets received             :%12u\n", pr_s.arp_recv);
	printf("-- Total IPv4 packets received            :%12u\n", pr_s.ip4_recv);
	printf("-- Total IPv6 packets received            :%12u\n", pr_s.ip6_recv);
	printf("-- Total Other link packets received      :%12u\n", pr_s.otherl_recv);
	printf("-- Total IPinIPv4 packets received        :%12u\n", pr_s.ip4ip_recv);
	printf("-- Total IPinIPv6 packets received        :%12u\n", pr_s.ip6ip_recv);
	printf("-- Total GRE packets received             :%12u\n", pr_s.gre_recv);
	printf("-- Total TCP packets received             :%12u\n", pr_s.tcp_recv);
	printf("-- Total UDP packets received             :%12u\n", pr_s.udp_recv);
	printf("-- Total ICMP packets received            :%12u\n", pr_s.icmp_recv);
	printf("-- Total Other transport packets received :%12u\n", pr_s.othert_recv);
	printf("--\n");
	//printf("-- Total sessions tracked                 :%12lu\n", cxtrackerid);
	printf("-- Total assets detected                  :%12u\n", pr_s.assets);
	printf("-- Total TCP OS fingerprints detected     :%12u\n", pr_s.tcp_os_assets);
	printf("-- Total UDP OS fingerprints detected     :%12u\n", pr_s.udp_os_assets);
	printf("-- Total ICMP OS fingerprints detected    :%12u\n", pr_s.icmp_os_assets);
	printf("-- Total DHCP OS fingerprints detected    :%12u\n", pr_s.dhcp_os_assets);
	printf("-- Total TCP service assets detected      :%12u\n", pr_s.tcp_services);
	printf("-- Total TCP client assets detected       :%12u\n", pr_s.tcp_clients);
	printf("-- Total UDP service assets detected      :%12u\n", pr_s.udp_services);
	printf("-- Total UDP client assets detected       :%12u\n", pr_s.udp_clients);


