#include "nocsim.h"

/* This file contains methods relating to the "grid" of routers, PEs, and links
 * */

/**
 * @brief Parse an entire file containing a grid definition
 *
 * While it is generally possible to traverse the grid using the links it
 * contains (although a grid could certainly contain disconnected sub-graphs)
 * every node (PE or router) which is created is installed into a linked list.
 * The head of this list is returned.
 *
 * A grid definition may contain, in any order, any of the following types
 * of declarations:
 *
 * router ID ROW COL
 *
 * PE ID ROW COL
 *
 * link ID ID
 *
 * behavior ID BEHAVIOR
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

		} else if (!strncmp(token, "behavior", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_behavior(rest, head);

		} else {
			err(1, "syntax error in grid definition, unknown token '%s'", token);

		}

		*token = 0; *rest = 0;
	}

	dbprintf("reached end of grid definition file\n");

	free(token);
	free(rest);

#ifdef EBUG
	dbprintf("dumping in-memory representation of grid definition... \n");
	foreach_element(cursor, head) {
		dbprintf("\t");
		dbprint_node(NOCSIM_LL2N(cursor));
		drprintf("\n");
	}

	dbprintf("graphviz dump of in-memory representation\n");
	nocsim_dump_graphviz(stderr, head);
#endif

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
		err(1, "syntax error in grid definition, did not understand link declaration '%s'", def);
	}
	dbprintf("parsed link declaration from='%s' to='%s'\n", from_id, to_id);

	from = NULL; to=NULL;
	foreach_element(cursor, head) {
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
	link->packet_next = NULL;

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

/**
 * @brief Parse a behavior declaration and assign it to the given router.
 *
 * Supported behaviors:
 *
 * * "DOR"
 *
 * @param def
 * @param head
 */
void nocsim_grid_parse_behavior(char* def, ll_node* head) {
	char* id;
	char* behavior_name;
	ll_node* cursor;
	ll_node* target;

	if ((id = malloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if ((behavior_name = malloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if (sscanf(def, "%s %s\n", id, behavior_name) <= 0) {
		err(1, "syntax error in grid definition, did not understand behavior declaration '%s'", def);
	}
	dbprintf("parsed behavior declaration id='%s' behavior_name='%s'\n",
			id, behavior_name);

	target = NULL;
	foreach_element(cursor, head) {
		if (!strncmp(id, NOCSIM_LL2N(cursor)->id, NOCSIM_GRID_LINELEN)) {
			if (target != NULL) {
				err(1, "duplicated ID %s\n", id);
			}

			if (NOCSIM_LL2N(cursor)->type != node_router) {
				err(1, "may not declare behavior for non-router node %s\n", id);
			}

			target = cursor;
		}
	}

	if (target == NULL) {
		err(1, "cannot declare behavior for unknown ID %s\n", id);
	}

	if (!strncmp(behavior_name, "DOR", NOCSIM_GRID_LINELEN)) {
		NOCSIM_LL2N(target)->behavior = nocsim_behavior_DOR;
	} else {
		err(1, "unknown behavior %s\n", behavior_name);
	}


}
