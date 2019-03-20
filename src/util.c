#include "nocsim.h"

/**
 * @brief Allocate a new node.
 *
 * Does not populate any type-specific fields, other than to initialize them
 * to zero.
 *
 * @param type
 * @param row
 * @param col
 * @param id
 *
 * @return
 */
nocsim_node* nocsim_allocate_node(nocsim_node_type type, unsigned int row, unsigned int col, char* id) {

	nocsim_node* n;

	if ((n = malloc(sizeof(nocsim_node))) == NULL) {
		err(1, "could not allocate node with id=%s", id);
	}


	n->type = type;
	n->row = row;
	n->col = col;
	n->id = id;

	n->incoming[N] = NULL;
	n->incoming[W] = NULL;
	n->incoming[S] = NULL;
	n->incoming[E] = NULL;
	n->incoming[P] = NULL;
	n->outgoing[N] = NULL;
	n->outgoing[W] = NULL;
	n->outgoing[S] = NULL;
	n->outgoing[E] = NULL;
	n->outgoing[P] = NULL;

	if ((n->extra = malloc(sizeof(ll_node))) == NULL) {
		err(1, "could not allocate extra for node id=%s", id);
	}

	n->extra->data = NULL;
	n->extra->next = NULL;

	n->fifo_size = 0;
	if ((n->fifo_head = malloc(sizeof(ll_node))) == NULL) {
		err(1, "could not allocate FIFO for node id=%s", id);
	}

	n->behavior = NULL;

	return n;
}

char* nocsim_fmt_node(nocsim_node* node) {
	char* s;

	if ((s = malloc((4*NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if (node == NULL) {
		snprintf(s, 4*NOCSIM_GRID_LINELEN,
				"node@0x%p:?,? 'unitialized'", (void*) node);

	} else {
		snprintf(s, 4*NOCSIM_GRID_LINELEN,
			"%s@0x%p:%u,%u '%s'",
			 NOCSIM_NODE_TYPE_TO_STR(node->type),
			 (void*) node, node->row, node->col, node->id);

	}

	return s;
}

void nocsim_print_node(FILE* stream, nocsim_node* node) {
	char* s;

	s = nocsim_fmt_node(node);
	fprintf(stream, "%s", s);
	free(s);
}

/**
 * @brief Dump a grid to graphviz on standard out.
 *
 * @param node
 */
void nocsim_dump_graphviz(FILE* stream, ll_node* head) {
	ll_node* cursor;

	fprintf(stream, "digraph G {\n");

	foreach_element(cursor, head) {
		/* XXX: this might be bad, unsigned int might be too short */
		fprintf(stream, "%x [label=\"%s\"]\n",
			(unsigned int) NOCSIM_LL2N(cursor),
			NOCSIM_LL2N(cursor)->id
		);
	}

	foreach_element(cursor, head) {
		for (nocsim_direction d = N; d <= P; d++) {
			if (NOCSIM_LL2N(cursor)->outgoing[d] == NULL) {
				continue;
			}

			fprintf(stream, "%x -> %x\n",
				(unsigned int) NOCSIM_LL2N(cursor),
				(unsigned int) NOCSIM_LL2N(cursor)->outgoing[d]->to
			);
		}
	}

	fprintf(stream, "}\n");
}

void nocsim_append_ll(ll_node* head, void* data) {
	ll_node* node;

	if ((node = malloc(sizeof(ll_node))) == NULL) {
		err(1, NULL);
	}

	node->data = data;

	while (head->next != NULL) {head = head->next;}

	head->next = node;
}

