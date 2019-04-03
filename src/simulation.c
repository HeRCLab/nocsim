#include "nocsim.h"

void nocsim_step(ll_node* head) {
	ll_node* cursor;
	ll_node* temp;

	dbprintf("beginning tick number %lu\n", ll2meta(head)->tick);
	printf("tick %lu\n", ll2meta(head)->tick);

	/* PEs generate packets */
	foreach_element(cursor, head) {
		if (ll2node(cursor)->type == node_PE) {
			if (with_P(ll2node(cursor)->P_inject)) {
				nocsim_inject(head, ll2node(cursor));
			}
		}
	}

	/* PEs send packets into links */
	foreach_element(cursor, head) {
		if (ll2node(cursor)->type != node_PE) { continue; }

		if (ll2node(cursor)->fifo_head->next != NULL) {

			ll2node(cursor)->outgoing[P]->flit = \
				(nocsim_flit*) ll2node(cursor)->fifo_head->next->data;

			printf("pop %lu from %s to %s->%s\n",
				ll2node(cursor)->outgoing[P]->flit->flit_no,
				ll2node(cursor)->id,
				ll2node(cursor)->id,
				ll2node(cursor)->outgoing[P]->to->id);

			/* pop out of the FIFO */
			temp = ll2node(cursor)->fifo_head->next;
			if (temp->next != NULL) {
				ll2node(cursor)->fifo_head->next = temp->next;
			} else {
				ll2node(cursor)->fifo_head->next = NULL;
			}
			ll2node(cursor)->fifo_size--;
			temp->next = NULL;
			temp->data = NULL;
			free(temp);
		}
	}

	/* calculate next state */
	foreach_element(cursor, head) {
		if (ll2node(cursor)->type != node_router) {
			continue;
		} else if (ll2node(cursor)->behavior == NULL) {
			err(1, "%s %s has no defined behavior\n",
				NOCSIM_NODE_TYPE_TO_STR(ll2node(cursor)->type),
				ll2node(cursor)->id);
		} else {
			ll2node(cursor)->behavior(ll2node(cursor));
		}
	}

	/* flip states */
	foreach_element(cursor, head) {
		for (nocsim_direction dir = N ; dir <= P ; dir++) {
			if (ll2node(cursor)->incoming[dir] != NULL) {
				ll2node(cursor)->incoming[dir]->flit = \
					ll2node(cursor)->incoming[dir]->flit_next;
				ll2node(cursor)->incoming[dir]->flit_next = NULL;
			}
		}
	}

	/* check if packet arrived */
	foreach_element(cursor, head) {
		for (nocsim_direction dir = N ; dir <= P ; dir++) {
			if (ll2node(cursor)->incoming[dir] != NULL) {
				if (ll2node(cursor)->incoming[dir]->flit != NULL) {
					if (ll2node(cursor)->incoming[dir]->flit->to == ll2node(cursor)) {
						printf("arrived %lu at %s\n",
							ll2node(cursor)->incoming[dir]->flit->flit_no,
							ll2node(cursor)->id);

						/* delete the flit */
						ll2node(cursor)->incoming[dir]->flit = NULL;
						free((ll2node(cursor)->incoming[dir]->flit));
					}
				}
			}

		}
	}

	ll2meta(head)->tick++;
}

/**
 * @brief Unconditionally inject a flit
 *
 * @param head
 * @param from
 */
void nocsim_inject(ll_node* head, nocsim_node* from) {
	unsigned int target_PE_num;
	unsigned int counter;
	list cursor;
	list flit_elem;
	nocsim_node* to;
	nocsim_flit* flit;

	to = NULL;

	/* Find a target PE that isn't us */
	do {
		target_PE_num = randrange(0,
			ll2meta(head)->num_PE);
	} while (target_PE_num == from->type_number);

	/* find the target */
	counter = 0;
	foreach_element(cursor, head) {
		if (ll2node(cursor)->type != node_PE) {continue;}

		if (counter == target_PE_num) {
			to = ll2node(cursor);
			break;
		}
		counter ++;
	}

	if (to == NULL) {
		err(1, "could not find PE# %u", target_PE_num);
	}

	alloc(sizeof(nocsim_flit), flit);

	flit->from = from;
	flit->to = to;
	flit->spawned_at = ll2meta(head)->tick;
	flit->flit_no = ll2meta(head)->flit_no;
	ll2meta(head)->flit_no ++;
	alloc(sizeof(list), flit_elem);
	flit_elem->data = flit;
	flit_elem->next = NULL;

	/* insert into FIFO */
	cursor = from->fifo_head;
	while (cursor->next != NULL) {cursor = cursor->next;}
	cursor->next = flit_elem;
	from->fifo_size++;

	printf("inject %lu from %s %u %u to %s %u %u\n",
			flit->flit_no,
			from->id, from->row, from->col,
			to->id, to->row, to->col);
	printf("push %lu into %s\n", flit->flit_no, from->id);


}

