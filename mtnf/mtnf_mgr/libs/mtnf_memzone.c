#include <stdint.h>

#include <memzone.h>

#include "mtnf_memzone.h"

/* reserve num * length bytes memzone */
void *
memzone_reserve(const char *memzone_name, uint32_t entry_length, uint32_t entry_num) {
	const rte_memzone *mz;

	mz = rte_memzone_reserve(memzone_name, entry_length * entry_num,
                                rte_socket_id(), NO_FLAGS);
    if (mz == NULL)
        rte_exit(EXIT_FAILURE, "Cannot reserve memory zone for %s information\n", memzone_name);
    memset(mz->addr, 0, entry_length * entry_num);

    return mz_addr;
}

/* lookup memzone through memzone_name */
void *
memzone_lookup(const char *memzone_name) {
	const rte_memzone *mz;
	void *mz_addr;

	mz = rte_memzone_lookup(memzone_name);

	if (mz == NULL)
		rte_exit(EXIT_FAILURE, "Cannot get %s information", memzone_name);
	mz_addr = mz->addr;

	return mz_addr;
}