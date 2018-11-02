
local seq_table_0 = {            -- entries can be in any order
    ["eth_dst_addr"] = "0011:4455:6677",
    ["eth_src_addr"] = "0011:1234:5678",
    ["ip_dst_addr"] = "192.168.1.1",
    ["ip_src_addr"] = "192.168.0.1/24",    -- the 16 is the size of the mask value
    ["sport"] = 1234,            -- Standard port numbers
    ["dport"] = 5678,            -- Standard port numbers
    ["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
    ["ipProto"] = "tcp",    -- udp|tcp|icmp
    ["vlanid"] = 1,            -- 1 - 4095
    ["pktSize"] = 200,        -- 64 - 1518
    ["teid"] = 3,
    ["cos"] = 5,
    ["tos"] = 6
  };

local seq_table_1 = {            -- entries can be in any order
    ["eth_dst_addr"] = "0011:4455:6677",
    ["eth_src_addr"] = "0011:1234:5678",
    ["ip_dst_addr"] = "192.168.1.1",
    ["ip_src_addr"] = "192.168.0.1/24",    -- the 16 is the size of the mask value
    ["sport"] = 1234,            -- Standard port numbers
    ["dport"] = 5678,            -- Standard port numbers
    ["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
    ["ipProto"] = "tcp",    -- udp|tcp|icmp
    ["vlanid"] = 1,            -- 1 - 4095
    ["pktSize"] = 600,        -- 64 - 1518
    ["teid"] = 3,
    ["cos"] = 5,
    ["tos"] = 6
  };

local seq_table_2 = {            -- entries can be in any order
    ["eth_dst_addr"] = "0011:4455:6677",
    ["eth_src_addr"] = "0011:1234:5678",
    ["ip_dst_addr"] = "192.168.1.1",
    ["ip_src_addr"] = "192.168.0.1/24",    -- the 16 is the size of the mask value
    ["sport"] = 1234,            -- Standard port numbers
    ["dport"] = 5678,            -- Standard port numbers
    ["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
    ["ipProto"] = "tcp",    -- udp|tcp|icmp
    ["vlanid"] = 1,            -- 1 - 4095
    ["pktSize"] = 800,        -- 64 - 1518
    ["teid"] = 3,
    ["cos"] = 5,
    ["tos"] = 6
  };

local seq_table_3 = {            -- entries can be in any order
    ["eth_dst_addr"] = "0011:4455:6677",
    ["eth_src_addr"] = "0011:1234:5678",
    ["ip_dst_addr"] = "192.168.1.1",
    ["ip_src_addr"] = "192.168.0.1/24",    -- the 16 is the size of the mask value
    ["sport"] = 1234,            -- Standard port numbers
    ["dport"] = 5678,            -- Standard port numbers
    ["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
    ["ipProto"] = "tcp",    -- udp|tcp|icmp
    ["vlanid"] = 1,            -- 1 - 4095
    ["pktSize"] = 1200,        -- 64 - 1518
    ["teid"] = 3,
    ["cos"] = 5,
    ["tos"] = 6
  };

  local seq_table_4 = {            -- entries can be in any order
  ["eth_dst_addr"] = "0011:4455:6677",
  ["eth_src_addr"] = "0011:1234:5678",
  ["ip_dst_addr"] = "192.168.1.1",
  ["ip_src_addr"] = "192.168.0.2/24",    -- the 16 is the size of the mask value
  ["sport"] = 1234,            -- Standard port numbers
  ["dport"] = 5678,            -- Standard port numbers
  ["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
  ["ipProto"] = "tcp",    -- udp|tcp|icmp
  ["vlanid"] = 1,            -- 1 - 4095
  ["pktSize"] = 200,        -- 64 - 1518
  ["teid"] = 3,
  ["cos"] = 5,
  ["tos"] = 6
};

local seq_table_5 = {            -- entries can be in any order
  ["eth_dst_addr"] = "0011:4455:6677",
  ["eth_src_addr"] = "0011:1234:5678",
  ["ip_dst_addr"] = "192.168.1.1",
  ["ip_src_addr"] = "192.168.0.2/24",    -- the 16 is the size of the mask value
  ["sport"] = 1234,            -- Standard port numbers
  ["dport"] = 5678,            -- Standard port numbers
  ["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
  ["ipProto"] = "tcp",    -- udp|tcp|icmp
  ["vlanid"] = 1,            -- 1 - 4095
  ["pktSize"] = 600,        -- 64 - 1518
  ["teid"] = 3,
  ["cos"] = 5,
  ["tos"] = 6
};

local seq_table_6 = {            -- entries can be in any order
  ["eth_dst_addr"] = "0011:4455:6677",
  ["eth_src_addr"] = "0011:1234:5678",
  ["ip_dst_addr"] = "192.168.1.1",
  ["ip_src_addr"] = "192.168.0.2/24",    -- the 16 is the size of the mask value
  ["sport"] = 1234,            -- Standard port numbers
  ["dport"] = 5678,            -- Standard port numbers
  ["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
  ["ipProto"] = "tcp",    -- udp|tcp|icmp
  ["vlanid"] = 1,            -- 1 - 4095
  ["pktSize"] = 800,        -- 64 - 1518
  ["teid"] = 3,
  ["cos"] = 5,
  ["tos"] = 6
};

local seq_table_7 = {            -- entries can be in any order
  ["eth_dst_addr"] = "0011:4455:6677",
  ["eth_src_addr"] = "0011:1234:5678",
  ["ip_dst_addr"] = "192.168.1.1",
  ["ip_src_addr"] = "192.168.0.2/24",    -- the 16 is the size of the mask value
  ["sport"] = 1234,            -- Standard port numbers
  ["dport"] = 5678,            -- Standard port numbers
  ["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
  ["ipProto"] = "tcp",    -- udp|tcp|icmp
  ["vlanid"] = 1,            -- 1 - 4095
  ["pktSize"] = 1200,        -- 64 - 1518
  ["teid"] = 3,
  ["cos"] = 5,
  ["tos"] = 6
};

local seq_table_8 = {            -- entries can be in any order
["eth_dst_addr"] = "0011:4455:6677",
["eth_src_addr"] = "0011:1234:5678",
["ip_dst_addr"] = "192.168.1.1",
["ip_src_addr"] = "192.168.0.3/24",    -- the 16 is the size of the mask value
["sport"] = 1234,            -- Standard port numbers
["dport"] = 5678,            -- Standard port numbers
["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
["ipProto"] = "tcp",    -- udp|tcp|icmp
["vlanid"] = 1,            -- 1 - 4095
["pktSize"] = 200,        -- 64 - 1518
["teid"] = 3,
["cos"] = 5,
["tos"] = 6
};

local seq_table_9 = {            -- entries can be in any order
["eth_dst_addr"] = "0011:4455:6677",
["eth_src_addr"] = "0011:1234:5678",
["ip_dst_addr"] = "192.168.1.1",
["ip_src_addr"] = "192.168.0.3/24",    -- the 16 is the size of the mask value
["sport"] = 1234,            -- Standard port numbers
["dport"] = 5678,            -- Standard port numbers
["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
["ipProto"] = "tcp",    -- udp|tcp|icmp
["vlanid"] = 1,            -- 1 - 4095
["pktSize"] = 600,        -- 64 - 1518
["teid"] = 3,
["cos"] = 5,
["tos"] = 6
};

local seq_table_10 = {            -- entries can be in any order
["eth_dst_addr"] = "0011:4455:6677",
["eth_src_addr"] = "0011:1234:5678",
["ip_dst_addr"] = "192.168.1.1",
["ip_src_addr"] = "192.168.0.3/24",    -- the 16 is the size of the mask value
["sport"] = 1234,            -- Standard port numbers
["dport"] = 5678,            -- Standard port numbers
["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
["ipProto"] = "tcp",    -- udp|tcp|icmp
["vlanid"] = 1,            -- 1 - 4095
["pktSize"] = 800,        -- 64 - 1518
["teid"] = 3,
["cos"] = 5,
["tos"] = 6
};

local seq_table_11 = {            -- entries can be in any order
["eth_dst_addr"] = "0011:4455:6677",
["eth_src_addr"] = "0011:1234:5678",
["ip_dst_addr"] = "192.168.1.1",
["ip_src_addr"] = "192.168.0.3/24",    -- the 16 is the size of the mask value
["sport"] = 1234,            -- Standard port numbers
["dport"] = 5678,            -- Standard port numbers
["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
["ipProto"] = "tcp",    -- udp|tcp|icmp
["vlanid"] = 1,            -- 1 - 4095
["pktSize"] = 1200,        -- 64 - 1518
["teid"] = 3,
["cos"] = 5,
["tos"] = 6
};

local seq_table_12 = {            -- entries can be in any order
["eth_dst_addr"] = "0011:4455:6677",
["eth_src_addr"] = "0011:1234:5678",
["ip_dst_addr"] = "192.168.1.1",
["ip_src_addr"] = "192.168.0.4/24",    -- the 16 is the size of the mask value
["sport"] = 1234,            -- Standard port numbers
["dport"] = 5678,            -- Standard port numbers
["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
["ipProto"] = "tcp",    -- udp|tcp|icmp
["vlanid"] = 1,            -- 1 - 4095
["pktSize"] = 200,        -- 64 - 1518
["teid"] = 3,
["cos"] = 5,
["tos"] = 6
};

local seq_table_13 = {            -- entries can be in any order
["eth_dst_addr"] = "0011:4455:6677",
["eth_src_addr"] = "0011:1234:5678",
["ip_dst_addr"] = "192.168.1.1",
["ip_src_addr"] = "192.168.0.4/24",    -- the 16 is the size of the mask value
["sport"] = 1234,            -- Standard port numbers
["dport"] = 5678,            -- Standard port numbers
["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
["ipProto"] = "tcp",    -- udp|tcp|icmp
["vlanid"] = 1,            -- 1 - 4095
["pktSize"] = 600,        -- 64 - 1518
["teid"] = 3,
["cos"] = 5,
["tos"] = 6
};

local seq_table_14 = {            -- entries can be in any order
["eth_dst_addr"] = "0011:4455:6677",
["eth_src_addr"] = "0011:1234:5678",
["ip_dst_addr"] = "192.168.1.1",
["ip_src_addr"] = "192.168.0.4/24",    -- the 16 is the size of the mask value
["sport"] = 1234,            -- Standard port numbers
["dport"] = 5678,            -- Standard port numbers
["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
["ipProto"] = "tcp",    -- udp|tcp|icmp
["vlanid"] = 1,            -- 1 - 4095
["pktSize"] = 800,        -- 64 - 1518
["teid"] = 3,
["cos"] = 5,
["tos"] = 6
};

local seq_table_15 = {            -- entries can be in any order
["eth_dst_addr"] = "0011:4455:6677",
["eth_src_addr"] = "0011:1234:5678",
["ip_dst_addr"] = "192.168.1.1",
["ip_src_addr"] = "192.168.0.4/24",    -- the 16 is the size of the mask value
["sport"] = 1234,            -- Standard port numbers
["dport"] = 5678,            -- Standard port numbers
["ethType"] = "ipv4",    -- ipv4|ipv6|vlan
["ipProto"] = "tcp",    -- udp|tcp|icmp
["vlanid"] = 1,            -- 1 - 4095
["pktSize"] = 1200,        -- 64 - 1518
["teid"] = 3,
["cos"] = 5,
["tos"] = 6
};

pktgen.seqTable(0, "1", seq_table_0 );
pktgen.seqTable(1, "1", seq_table_1 );
pktgen.seqTable(2, "1", seq_table_2 );
pktgen.seqTable(3, "1", seq_table_3 );
pktgen.seqTable(4, "1", seq_table_4 );
pktgen.seqTable(5, "1", seq_table_5 );
pktgen.seqTable(6, "1", seq_table_6 );
pktgen.seqTable(7, "1", seq_table_7 );
pktgen.seqTable(8, "1", seq_table_8 );
pktgen.seqTable(9, "1", seq_table_9 );
pktgen.seqTable(10, "1", seq_table_10 );
pktgen.seqTable(11, "1", seq_table_11 );
pktgen.seqTable(12, "1", seq_table_12 );
pktgen.seqTable(13, "1", seq_table_13 );
pktgen.seqTable(14, "1", seq_table_14 );
pktgen.seqTable(15, "1", seq_table_15 );

pktgen.set("1", "seq_cnt", 16);
