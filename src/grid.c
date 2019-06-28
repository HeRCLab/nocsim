#include "nocsim.h"

/* This file contains methods relating to the "grid" of routers, PEs, and links
 * */

#include <stdio.h>
#include <tcl.h>

/*
int DoubleIt( ClientData Data, Tcl_Interp *pInterp, int argc, Tcl_Obj *CONST argv[] ) {
	Tcl_Obj *pResult;
	int x;
	if (Tcl_GetIntFromObj(pInterp, argv[1], &x) != TCL_OK) { return TCL_ERROR; }

	pResult = Tcl_GetObjResult(pInterp);
	Tcl_SetIntObj(pResult, 2*x);
	return TCL_OK;
}

main (int argc, char *argv[]) {

        Tcl_Interp *myinterp;
        char *action = "set a [expr 5 * 8 + $b]; puts $a ; puts \"doubled to [doubleit $a]\"";
        int status;

        printf ("Your Program will run ... \n");

        myinterp = Tcl_CreateInterp();

	Tcl_CreateObjCommand(myinterp, "doubleit", DoubleIt, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	Tcl_SetVar(myinterp, "b", "5", 0);

        status = Tcl_Eval(myinterp,action);

        printf ("Your Program has completed\n");

	printf("value of a is now: %s\n", Tcl_GetVar(myinterp, "a", 0));


}
*/


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
nocsim_state* nocsim_grid_parse_file(FILE* stream) {
	char* token;
	char* rest;
	nocsim_state* state;
	nocsim_node* cursor;
	nodelist* l;
	unsigned int i;

	alloc(sizeof(nocsim_state), state);
	alloc(NOCSIM_GRID_LINELEN * sizeof(char), token);
	alloc(NOCSIM_GRID_LINELEN * sizeof(char), rest);
	alloc(sizeof(nodelist), l);

	state->RNG_seed = (unsigned int) time(NULL);
	state->num_PE = 0;
	state->num_router = 0;
	state->num_node = 0;
	state->flit_no = 0;
	state->tick = 0;
	state->default_P_inject = 0;
	state->title = "unspecified";

	vec_init(l);
	state->nodes = l;

	*token = 0; *rest = 0;

	while(fscanf(stream, "%s %[a-zA-Z0-9._ ]\n", token, rest) != EOF) {
		dbprintf("token='%s' rest='%s'\n", token, rest);

		if (!strncmp(token, "router", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_router(rest, state);

		} else if (!strncmp(token, "PE", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_PE(rest, state);

		} else if (!strncmp(token, "link", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_link(rest, state);

		} else if (!strncmp(token, "behavior", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_behavior(rest, state);

		} else if (!strncmp(token, "config", NOCSIM_GRID_LINELEN)) {
			nocsim_grid_parse_config(rest, state);

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
	vec_foreach(state->nodes, cursor, i) {
		dbprintf("\t");
		dbprint_node(cursor);
		drprintf("\n");
	}

	dbprintf("graphviz dump of in-memory representation\n");
	nocsim_dump_graphviz(stderr, state);
#endif


	return state;
}

/**
 * @brief Parse a definition of a router and create an appropriate node for it.
 *
 * Note that the caller needs to define behaviors for the routers.
 *
 * @param def
 * @param head
 */
void nocsim_grid_parse_router(char* def, nocsim_state* state) {
	char* id;
	unsigned int row;
	unsigned int col;
	nocsim_node* router;

	alloc(sizeof(nocsim_node), router);
	alloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char), id);

	id[NOCSIM_GRID_LINELEN] = 0; /* guarantee that the id is null terminated */

	if (sscanf(def, "%s %u %u\n", id, &row, &col) <= 0) {
		err(1, "syntax error in grid definition, did not understand router declaration '%s'", def);
	}

	dbprintf("parsed router declaration id='%s' row=%u col=%u\n", id, row, col);

	nocsim_init_node(router, node_router, row, col, id);

	router->node_number = state->num_node;
	router->type_number = state->num_router;
	state->num_node++;
	state->num_router++;
	
	vec_push(state->nodes, router);
}

/**
 * @brief Parse a definition of a PE and create an appropriate node for it.
 *
 * @param def
 * @param head
 */
void nocsim_grid_parse_PE(char* def, nocsim_state* state) {
	char* id;
	unsigned int row;
	unsigned int col;
	float P_inject;
	int num_assigned;
	nocsim_node* PE;
	flitlist* pending;

	alloc(sizeof(nocsim_node), PE);
	alloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char), id);

	id[NOCSIM_GRID_LINELEN] = 0; /* guarantee that the id is null terminated */

	num_assigned = sscanf(def, "%s %u %u %f\n", id, &row, &col, &P_inject);
	if (num_assigned < 3) {
		err(1, "syntax error in grid definition, did not understand PE declaration '%s'", def);

	} else if (num_assigned < 4) {
		// use default P_inject
		P_inject = state->default_P_inject;
		dbprintf("PE definition does not specify P_inject, using default\n");
	}

	dbprintf("parsed PE declaration id='%s' row=%u col=%u P_inject=%f\n",
			id, row, col, P_inject);

	nocsim_init_node(PE, node_PE, row, col, id);

	alloc(sizeof(flitlist), pending);
	vec_init(pending);
	PE->pending = pending;
	PE->P_inject = P_inject;
	PE->node_number = state->num_node;
	state->num_node++;
	PE->type_number = state->num_PE;
	state->num_PE++;

	vec_push(state->nodes, PE);

}

/**
 * @brief Parse a link and create the appropriate entries in the routers and
 * PEs.
 *
 * @param def
 * @param head
 */
void nocsim_grid_parse_link(char* def, nocsim_state* state) {
	char* from_id;
	char* to_id;
	nocsim_node* from;
	nocsim_node* to;
	nocsim_link* link;
	unsigned int i;
	nocsim_node* cursor;

	alloc(sizeof(nocsim_link), link);
	alloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char), from_id);
	alloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char), to_id);

	if (sscanf(def, "%s %s\n", from_id, to_id) <= 0) {
		err(1, "syntax error in grid definition, did not understand link declaration '%s'", def);
	}
	dbprintf("parsed link declaration from='%s' to='%s'\n", from_id, to_id);

	from = NULL; to=NULL;
	vec_foreach(state->nodes, cursor, i) {
		if (cursor->id == NULL) {
			err(1, "node@0x%p missing ID", (void*) cursor);
		}

		if (!strncmp(to_id, cursor->id, NOCSIM_GRID_LINELEN)) {
			to = cursor;
		}
		if (!strncmp(from_id, cursor->id, NOCSIM_GRID_LINELEN)) {
			from = cursor;
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

	free(to_id);
	free(from_id);

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
void nocsim_grid_parse_behavior(char* def, nocsim_state* state) {
	char* id;
	char* behavior_name;
	nocsim_node* cursor;
	nocsim_node* target;
	unsigned int i;
	
	alloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char), id);
	alloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char), behavior_name);

	if (sscanf(def, "%s %s\n", id, behavior_name) <= 0) {
		err(1, "syntax error in grid definition, did not understand behavior declaration '%s'", def);
	}
	dbprintf("parsed behavior declaration id='%s' behavior_name='%s'\n",
			id, behavior_name);

	target = NULL;
	vec_foreach(state->nodes, cursor, i) {
		if (!strncmp(id, cursor->id, NOCSIM_GRID_LINELEN)) {
			if (target != NULL) {
				err(1, "duplicated ID %s\n", id);
			}

			if (cursor->type != node_router) {
				err(1, "may not declare behavior for non-router node %s\n", id);
			}

			target = cursor;
		}
	}

	if (target == NULL) {
		err(1, "cannot declare behavior for unknown ID %s\n", id);
	}

	if (!strncmp(behavior_name, "DOR", NOCSIM_GRID_LINELEN)) {
		target->behavior = nocsim_behavior_DOR;

	} else if (!strncmp(behavior_name, "ADOR", NOCSIM_GRID_LINELEN)) {
		target->behavior = nocsim_behavior_ADOR;

	} else {
		err(1, "unknown behavior %s\n", behavior_name);
	}

	free(id);
	free(behavior_name);

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
void nocsim_grid_parse_config(char* def, nocsim_state* state) {
	char* key;
	char* val;
	char* errstr;
	unsigned int seed;
	unsigned int max_ticks;
	float default_P_inject;

	errstr = NULL;

	alloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char), key);
	alloc((1 + NOCSIM_GRID_LINELEN) * sizeof(char), val);


	if (sscanf(def, "%s %s\n", key, val) <= 0) {
		err(1, "syntax error in grid definition, did not understand config declaration '%s'", def);
	}
	dbprintf("parsed config declaration key='%s' val='%s'\n",
			key, val);

	if (!strncmp(key, "RNG_seed", NOCSIM_GRID_LINELEN)) {
#ifdef __OpenBSD__
		seed = (unsigned int) strtonum(val, 0, UINT_MAX, &errstr);
		if (errstr != NULL) {
			err(1, "could not parse RNG seed '%s'", val);
		}
#else
		seed = (unsigned int) strtoul(val, &errstr, 10);
#endif
		state->RNG_seed = seed;
		dbprintf("state->RNG_seed=%u\n", seed);
		free(val);

	} else if (!strncmp(key, "max_ticks", NOCSIM_GRID_LINELEN)) {
#ifdef __OpenBSD__
		max_ticks = (unsigned int) strtonum(val, 0, UINT_MAX, &errstr);
		if (errstr != NULL) {
			err(1, "could not parse max_ticks '%s'", val);
		}
#else
		max_ticks = (unsigned int) strtoul(val, &errstr, 10);
#endif
		state->max_ticks = max_ticks;
		dbprintf("state->max_ticks=%u\n", max_ticks);
		free(val);

	} else if (!strncmp(key, "default_P_inject", NOCSIM_GRID_LINELEN)) {
		default_P_inject = strtof(val, NULL);
		if (errno == ERANGE) {
			err(1, "could not parse default_P_inject '%s'", val);
		}
		state->default_P_inject = default_P_inject;
		dbprintf("state->default_P_inject=%f\n", default_P_inject);
		free(val);

	} else if (!strncmp(key, "title", NOCSIM_GRID_LINELEN)) {
		state->title = val;
		dbprintf("state->title=%s\n", val);

	} else {
		err(1, "invalid config definition '%s' unknown key '%s'",
				def, key);
	}
	free(key);

}
