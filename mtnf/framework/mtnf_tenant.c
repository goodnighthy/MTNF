#include "mtnf_tenant.h"

void *states;
uint32_t state_length;

/*********************************test*************************************/
static uint32_t
mtnf_state_register(void) {
	return sizeof(uint32_t);
}

static uint16_t
mtnf_nf_handler(struct rte_mbuf *pkt[], uint16_t num, void *memory) {
	uint32_t *state;
	uint16_t out;

	state = (uint32_t *)memory;
	*state = *state + num;
	out = num;

	return out;
}

/* init tenants state */
void
mtnf_states_init(void) {
	state_length = mtnf_state_register();
	states = memzone_reserve(MTNF_TENANT_STATE, state_length, tenant_number);
}

/* packet handler for tenant id */
uint16_t
mtnf_packets_handler(struct tenants_buffer *buffer, uint8_t id) {
	uint16_t nb_handler;
	void *state;

	state = (void *)((uint8_t *)states + state_length * id);
	nb_handler = mtnf_nf_handler(buffer->buffer_slot, buffer->num, state);

	buffer->num = 0;

	return nb_handler;
}

