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
 * PE ID ROW COL P_inject
 *
 * link ID ID
 *
 * behavior ID BEHAVIOR
 *
 * config KEY VALUE
 *
 * @param stream
 *
 * @return linked list of all nodes in the grid
 */
ll_node* nocsim_grid_parse_file(FILE* stream) {
	char* token;
	char* rest;
	ll_node* head;
#ifdef EBUG
	ll_node* cursor;
#endif
	nocsim_meta* meta;

	alloc(sizeof(ll_node), head);

	head->data = NULL;
	head->next = NULL;

	if ((token = malloc(NOCSIM_GRID_LINELEN * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if ((rest = malloc(NOCSIM_GRID_LINELEN * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if ((meta = malloc(sizeof(nocsim_meta))) == NULL) {
		err(1, NULL);
	}

	meta->RNG_seed = (unsigned int) time(NULL);
	meta->num_PE = 0;
	meta->num_router = 0;
	meta->num_node = 0;
	meta->flit_no = 0;
	meta->tick = 0;
	meta->default_P_inject = 0;
	meta->title = "unspecified";

	head->data = (void*) meta;

	*token = 0; *rest = 0;

	while(fscanf(stream, "%s %[a-zA-Z0-9._ ]\n", token, rest) != EOF) {
		dbprintf("token='%s' rest='%s'\n", token, rest);

		if (!strncmp(token, "router", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_router(rest, head);

		} else if (!strncmp(token, "PE", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_PE(rest, head);

		} else if (!strncmp(token, "link", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_link(rest, head);

		} else if (!strncmp(token, "behavior", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_behavior(rest, head);

		} else if (!strncmp(token, "config", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_config(rest, meta);

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
		dbprint_node(ll2node(cursor));
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

	router->node_number = ll2meta(head)->num_node;
	ll2meta(head)->num_node++;
	router->type_number = ll2meta(head)->num_router;
	ll2meta(head)->num_router++;

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
	float P_inject;
	int num_assigned;
	nocsim_node* PE;
	ll_node* tail;

	if ((id = malloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	id[NOCSIM_GRID_LINELEN] = 0; /* guarantee that the id is null terminated */

	if ((tail = malloc(sizeof(ll_node))) == NULL) {
		err(1, NULL);
	}

	num_assigned = sscanf(def, "%s %u %u %f\n", id, &row, &col, &P_inject);
	if (num_assigned < 3) {
		err(1, "syntax error in grid definition, did not understand PE declaration '%s'", def);

	} else if (num_assigned < 4) {
		// use default P_inject
		P_inject = ll2meta(head)->default_P_inject;
		dbprintf("PE definition does not specify P_inject, using default\n");
	}

	dbprintf("parsed PE declaration id='%s' row=%u col=%u P_inject=%f\n",
			id, row, col, P_inject);

	PE = nocsim_allocate_node(node_PE, row, col, id);

	alloc(sizeof(list), PE->fifo_head);
	PE->fifo_head->next = NULL;
	PE->fifo_head->data = NULL;
	PE->P_inject = P_inject;
	PE->node_number = ll2meta(head)->num_node;
	ll2meta(head)->num_node++;
	PE->type_number = ll2meta(head)->num_PE;
	ll2meta(head)->num_PE++;

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
		if (!strncmp(to_id, ll2node(cursor)->id, NOCSIM_GRID_LINELEN)) {
			to = ll2node(cursor);
		}
		if (!strncmp(from_id, ll2node(cursor)->id, NOCSIM_GRID_LINELEN)) {
			from = ll2node(cursor);
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
	link->flit = NULL;
	link->flit_next = NULL;

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
		if (!strncmp(id, ll2node(cursor)->id, NOCSIM_GRID_LINELEN)) {
			if (target != NULL) {
				err(1, "duplicated ID %s\n", id);
			}

			if (ll2node(cursor)->type != node_router) {
				err(1, "may not declare behavior for non-router node %s\n", id);
			}

			target = cursor;
		}
	}

	if (target == NULL) {
		err(1, "cannot declare behavior for unknown ID %s\n", id);
	}

	if (!strncmp(behavior_name, "DOR", NOCSIM_GRID_LINELEN)) {
		ll2node(target)->behavior = nocsim_behavior_DOR;

	} else if (!strncmp(behavior_name, "ADOR", NOCSIM_GRID_LINELEN)) {
		ll2node(target)->behavior = nocsim_behavior_ADOR;

	} else {
		err(1, "unknown behavior %s\n", behavior_name);
	}


}

/**
 * @brief Parse a config declaration and assign it to the given router.
 *
 * Supported config keys:
 *
 * * RNG_seed
 * * max_ticks
 * * default_P_inject
 * * title
 *
 * @param def
 * @param head
 */
void nocsim_grid_parse_config(char* def, nocsim_meta * meta) {
	char* key;
	char* val;
	const char* errstr;
	unsigned int seed;
	unsigned int max_ticks;
	float default_P_inject;

	errstr = NULL;

	if ((key = malloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if ((val = malloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}


	if (sscanf(def, "%s %s\n", key, val) <= 0) {
		err(1, "syntax error in grid definition, did not understand config declaration '%s'", def);
	}
	dbprintf("parsed config declaration key='%s' val='%s'\n",
			key, val);

	if (!strncmp(key, "RNG_seed", NOCSIM_GRID_LINELEN)) {
		seed = (unsigned int) strtonum(val, 0, UINT_MAX, &errstr);
		if (errstr != NULL) {
			err(1, "could not parse RNG seed '%s'", val);
		}
		meta->RNG_seed = seed;
		dbprintf("meta->RNG_seed=%u\n", seed);

	} else if (!strncmp(key, "max_ticks", NOCSIM_GRID_LINELEN)) {
		max_ticks = (unsigned int) strtonum(val, 0, UINT_MAX, &errstr);
		if (errstr != NULL) {
			err(1, "could not parse max_ticks '%s'", val);
		}
		meta->max_ticks = max_ticks;
		dbprintf("meta->max_ticks=%u\n", max_ticks);

	} else if (!strncmp(key, "default_P_inject", NOCSIM_GRID_LINELEN)) {
		default_P_inject = strtof(val, NULL);
		if (errno == ERANGE) {
			err(1, "could not parse default_P_inject '%s'", val);
		}
		meta->default_P_inject = default_P_inject;
		dbprintf("meta->default_P_inject=%f\n", default_P_inject);

	} else if (!strncmp(key, "title", NOCSIM_GRID_LINELEN)) {
		meta->title = val;
		dbprintf("meta->title=%s\n", val);

	} else {
		err(1, "invalid config definition '%s' unknown key '%s'",
				def, key);
	}


}
