#include "mtnf_tenant.h"

void *states;
uint32_t state_length;

static bool tenant_lock[MAX_TENANTS] = {false};

/* init tenants state */
void
mtnf_states_init(void) {
	uint16_t i;

//	state_length = mtnf_aes_encrypt_register();
	state_length = mtnf_ids_register();
//	state_length = mtnf_firewall_register();
//	state_length = mtnf_monitor_register();
	states = memzone_reserve(MTNF_TENANT_STATE, state_length, tenant_number);

	for (i = 0; i < tenant_number; i++) {
//		mtnf_aes_encrypt_init((void *)((uint8_t *)states + state_length * i));
		mtnf_ids_init((void *)((uint8_t *)states + state_length * i));
//		mtnf_firewall_init((void *)((uint8_t *)states + state_length * i));
//		mtnf_monitor_init((void *)((uint8_t *)states + state_length * i));
	}
}

/* packet handler for tenant id */
uint16_t
mtnf_packets_handler(struct tenants_buffer *buffer, uint8_t id) {
	uint16_t nb_handler;
	void *state;

	while (tenant_lock[id]);

	tenant_lock[id] = true;

	state = (void *)((uint8_t *)states + state_length * id);
//	nb_handler = mtnf_aes_encrypt_handler(buffer->buffer_slot, buffer->num, state);
	nb_handler = mtnf_ids_handler(buffer->buffer_slot, buffer->num, state);
//	nb_handler = mtnf_firewall_handler(buffer->buffer_slot, buffer->num, state);
//	nb_handler = mtnf_monitor_handler(buffer->buffer_slot, buffer->num, state);

	tenant_lock[id] = false;
	buffer->num = 0;

	return nb_handler;
}

