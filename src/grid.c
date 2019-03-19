#include "nocsim.h"

/* This file contains methods relating to the "grid" of routers, PEs, and links
 * */

/**
 * @brief Parse an entire file containing a grid definition
 *
 * @param stream
 *
 * @return linked list of all nodes in the grid
 */
ll_node* nocsim_grid_parse_file(FILE* stream) {
	char* token;
	char* rest;
	ll_node* head;
	ll_node* cursor;

	if ((head = malloc(sizeof(ll_node))) == NULL) {
		err(1, NULL);
	}

	head->data = NULL;
	head->next = NULL;

	if ((token = malloc(NOCSIM_GRID_LINELEN * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if ((rest = malloc(NOCSIM_GRID_LINELEN * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	*token = 0; *rest = 0;

	while(fscanf(stream, "%s %[a-zA-Z0-9 ]\n", token, rest) != EOF) {
		dbprintf("token='%s' rest='%s'\n", token, rest);

		if (!strncmp(token, "router", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_router(rest, head);

		} else if (!strncmp(token, "PE", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_PE(rest, head);

		} else if (!strncmp(token, "link", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_link(rest, head);

		} else {
			err(1, "syntax error in grid definition, unknown token '%s'", token);

		}

		*token = 0; *rest = 0;
	}

	free(token);
	free(rest);

	dbprintf("dumping in-memory representation of grid definition... \n");
	for (cursor = head->next; cursor != NULL; cursor = cursor->next) {
		dbprintf("\t");
		dbprint_node(NOCSIM_LL2N(cursor));
		drprintf("\n");
	}

	dbprintf("reached end of grid definition file\n");

	nocsim_dump_graphviz(stdout, head);

	return head;
}

/**
 * @brief Parse a definition of a router and create an appropriate node for it.
 *
 * Note that the caller needs to define behaviors for the routers.
 *
 * @param def
 * @param head
 */
void nocsim_grid_parse_router(char* def, ll_node* head) {
	char* id;
	unsigned int row;
	unsigned int col;
	nocsim_node* router;
	ll_node* tail;

	if ((id = malloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	id[NOCSIM_GRID_LINELEN] = 0; /* guarantee that the id is null terminated */

	if ((tail = malloc(sizeof(ll_node))) == NULL) {
		err(1, NULL);
	}

	if (sscanf(def, "%s %u %u\n", id, &row, &col) <= 0) {
		err(1, "syntax error in grid definition, did not understand router declaration '%s'", def);
	}

	dbprintf("parsed router declaration id='%s' row=%u col=%u\n", id, row, col);

	router = nocsim_allocate_node(node_router, row, col, id);

	tail->data = (void*) router;
	tail->next = NULL;

	/* advance head to the tail of the list of grid elements and append
	 * this router to it */
	while (head->next != NULL) {head = head->next;}
	head->next = tail;
}

/**
 * @brief Parse a definition of a PE and create an appropriate node for it.
 *
 * @param def
 * @param head
 */
void nocsim_grid_parse_PE(char* def, ll_node* head) {
	char* id;
	unsigned int row;
	unsigned int col;
	nocsim_node* PE;
	ll_node* tail;

	if ((id = malloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	id[NOCSIM_GRID_LINELEN] = 0; /* guarantee that the id is null terminated */

	if ((tail = malloc(sizeof(ll_node))) == NULL) {
		err(1, NULL);
	}

	if (sscanf(def, "%s %u %u\n", id, &row, &col) <= 0) {
		err(1, "syntax error in grid definition, did not understand PE declaration '%s'", def);
	}

	dbprintf("parsed PE declaration id='%s' row=%u col=%u\n", id, row, col);

	PE = nocsim_allocate_node(node_PE, row, col, id);

	tail->data = (void*) PE;
	tail->next = NULL;

	/* advance head to the tail of the list of grid elements and append
	 * this router to it */
	while (head->next != NULL) {head = head->next;}
	head->next = tail;
}

/**
 * @brief Parse a link and create the appropriate entries in the routers and
 * PEs.
 *
 * @param def
 * @param head
 */
void nocsim_grid_parse_link(char* def, ll_node* head) {
	char* from_id;
	char* to_id;
	nocsim_node* from;
	nocsim_node* to;
	ll_node* cursor;
	nocsim_link* link;

	if ((link = malloc(sizeof(nocsim_link))) == NULL) {
		err(1, NULL);
	}

	if ((from_id = malloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if ((to_id = malloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if (sscanf(def, "%s %s\n", from_id, to_id) <= 0) {
		err(1, "syntax error in grid definition, did not understand PE declaration '%s'", def);
	}
	dbprintf("parsed link declaration from='%s' to='%s'\n", from_id, to_id);

	from = NULL; to=NULL;
	for (cursor = head->next; cursor != NULL; cursor = cursor->next) {
		if (!strncmp(to_id, NOCSIM_LL2N(cursor)->id, NOCSIM_GRID_LINELEN)) {
			to = NOCSIM_LL2N(cursor);
		}
		if (!strncmp(from_id, NOCSIM_LL2N(cursor)->id, NOCSIM_GRID_LINELEN)) {
			from = NOCSIM_LL2N(cursor);
		}
	}

	dbprintf("from node=");
	dbprint_node(from);
	drprintf("\n");

	dbprintf("to node=");
	dbprint_node(to);
	drprintf("\n");

	if (from == NULL) {
		err(1, "could not link from unknown node '%s'", from_id);
	}

	if (to == NULL) {
		err(1, "could not link to unknown node '%s'", to_id);
	}

	link->to = to;
	link->from = from;
	link->packet = NULL;

	if ((from->type == node_PE) && (to->type == node_PE)) {
		err(1, "cannot create illegal link from PE '%s' to PE '%s'",
			from_id, to_id);

	} else if (((from->type == node_PE) && (to->type == node_router)) ||
		((from->type == node_router) && (to->type == node_PE))) {
		dbprintf("link direction: PE\n");
		from->outgoing[P] = link;
		to->incoming[P] = link;

	} else {
		if (from->row < to->row) {
			dbprintf("link direction: S->N (down)\n");
			from->outgoing[S] = link;
			to->incoming[N] = link;

		} else if (from->row > to->row) {
			dbprintf("link direction: N->S (up)\n");
			from->outgoing[N] = link;
			to->incoming[S] = link;

		} else if (from->col < to->col) {
			dbprintf("link direction: E->W (right)\n");
			from->outgoing[E] = link;
			to->incoming[W] = link;

		} else if (from->col > to->col) {
			dbprintf("link direction: W->E (left)\n");
			from->outgoing[W] = link;
			to->incoming[E] = link;

		} else {
			fprintf(stderr, "nodes: ");
			nocsim_print_node(stderr, from);
			fprintf(stderr, " and ");
			nocsim_print_node(stderr, to);
			fprintf(stderr, " cannot be linked because they have overlapping coordinates\n");
			err(1, NULL);

		}


	}

}
