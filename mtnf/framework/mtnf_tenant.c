#include "mtnf_tenant.h"

void *states;
uint32_t state_length;

/* init tenants state */
void
mtnf_states_init(void) {
	state_length = mtnf_monitor_register();
	states = memzone_reserve(MTNF_TENANT_STATE, state_length, tenant_number);
}

/* packet handler for tenant id */
uint16_t
mtnf_packets_handler(struct tenants_buffer *buffer, uint8_t id) {
	uint16_t nb_handler;
	void *state;

	state = (void *)((uint8_t *)states + state_length * id);
	nb_handler = mtnf_monitor_handler(buffer->buffer_slot, buffer->num, state);

	buffer->num = 0;

	return nb_handler;
}

