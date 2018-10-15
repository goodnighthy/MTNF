// #include "includes/headers.p4"
// #include "includes/metadata.p4"
// #include "includes/parser.p4"

#include "headers.p4"
#include "metadata.p4"
#include "parser.p4"
//==========================================================================================================
action do_drop() {
	drop();
}

//==========================================================================================================
action tag_attach(tid) {
	modify_field(ipv4.diffserv, tid);
}

table classifier {
	reads {
		ipv4.srcAddr : exact;
		ipv4.dstAddr : exact;
		ipv4.protocol : exact;
		transport.srcPort : exact;
		transport.dstPort : exact;
	}
	actions {
		tag_attach;
		do_drop;
	}
}

//==========================================================================================================
primitive_action primitive_dispatch();

action do_dispatch(port) {
	primitive_dispatch();
	modify_field(standard_metadata.egress_spec, port);
}

table dispatcher {
	reads {
		ipv4.diffserv : exact;
	}
	actions {
		do_dispatch;
	}
}

//==========================================================================================================
action do_forward(port) {
    modify_field(standard_metadata.egress_spec, port);
}

table forward {
    reads {
        standard_metadata.ingress_port : exact;
    }
    actions {
		do_forward;
		do_drop;
    }
}

//==========================================================================================================
control ingress {
	if (standard_metadata.ingress_port == 0 or standard_metadata.ingress_port == 1) {
		apply(classifier);
		apply(dispatcher);
	} else {
		apply(forward);
	}
}

//==========================================================================================================