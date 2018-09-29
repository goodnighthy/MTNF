#define ETHERTYPE_IPV4 0x0800
// #define UDP_PROTOCOL 0x11
// #define TCP_PROTOCOL 0x06

parser start {
    return parse_ethernet;
}


parser parse_ethernet {
    extract(ethernet);
    return select(latest.etherType) {
        ETHERTYPE_IPV4 : parse_ipv4;
        // No default, so drop it if not ipv4 packet
    }
}


parser parse_ipv4 {
    extract(ipv4);
    return parse_transport;
    // return select(ipv4.protocol) {
    //     TCP_PROTOCOL : parse_tcp;
    //     UDP_PROTOCOL : parse_udp;
    //     // No default, so drop it if not tcp or udp packet
    // }
}


// parser parse_udp {
//     extract(udp);
//     return  ingress;
// }


// parser parse_tcp {
//     extract(tcp);
//     return  ingress;
// }

parser parse_transport {
    extract(transport);
    return ingress;
}
