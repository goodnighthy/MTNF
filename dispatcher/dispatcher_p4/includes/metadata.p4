header_type dispatch_metadata_t {
	fields {
		worker_id : 32;
	}
}

metadata dispatch_metadata_t dispatch_metadata;

header_type intrinsic_metadata_t {
    fields {
        ingress_global_timestamp : 32;
    }
}

metadata intrinsic_metadata_t intrinsic_metadata;
