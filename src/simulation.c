#include "nocsim.h"

void nocsim_step(ll_node* head, unsigned int tick) {
	ll_node* cursor;

	dbprintf("beginning tick number %u\n", tick);

	/* calculate next state */
	foreach_element(cursor, head) {
		if (NOCSIM_LL2N(cursor)->type == node_PE) {
			dbprintf("TODO: handle PE routing\n");
		} else if (NOCSIM_LL2N(cursor)->behavior == NULL) {
			err(1, "%s %s has no defined behavior\n",
				NOCSIM_NODE_TYPE_TO_STR(NOCSIM_LL2N(cursor)->type),
				NOCSIM_LL2N(cursor)->id);
		} else {
			NOCSIM_LL2N(cursor)->behavior(NOCSIM_LL2N(cursor));
		}
	}

	/* flip states */
	foreach_element(cursor, head) {
	}
}
