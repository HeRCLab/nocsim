#include "nocsim.h"

/*** utilities ***************************************************************/

/* declare an object command callback with a given name */
#define interp_command(name) int name \
	(ClientData data, Tcl_Interp* interp, int argc, \
		Tcl_Obj* CONST* argv)

/* safely get an integer from and object -- will be stored in ptr */
#define get_int(interp, obj, ptr) if (Tcl_GetIntFromObj(interp, obj, ptr) != TCL_OK) \
			{return TCL_ERROR;}

/* require n arguments and display message msg if not */
#define req_args(n, msg) if (argc != n) { \
	Tcl_WrongNumArgs(interp, 0, argv, msg); return TCL_ERROR; }

#define validate_direction(dir) do { \
		if (dir < 0 || dir > P) { \
			Tcl_SetResult(interp, "value provided for" #dir "is out of bounds", NULL); \
			return TCL_ERROR; \
		} \
	} while (0)


/*** router ID ROW COL behavior **********************************************/
interp_command(nocsim_create_router) {

	nocsim_state* state = (nocsim_state*) data;
	char* id = NULL;
	char* behavior;
	int row = -1;
	int col = -1;

	req_args(5, "ID ROW COL BEHAVIOR");

	id = Tcl_GetStringFromObj(argv[1], NULL);
	get_int(interp, argv[2], &row);
	get_int(interp, argv[3], &col);
	behavior = Tcl_GetStringFromObj(argv[4], NULL);

	/* nocsim_grid_create_router does not use strcpy() on strings that is
	 * is passed as arguments. We increment the reference count of this
	 * string to ensure that TCL does not attempt to garbage-collect it
	 * after it has already been attached to a router object. In theory,
	 * the router should store a handle to the TCL object that contains
	 * these strings, however as nodes cannot be deleted later, we just
	 * leave it be and trust the kernel to free() for us when the program
	 * exits */
	Tcl_IncrRefCount(argv[1]);  /* id */
	Tcl_IncrRefCount(argv[4]);  /* behavior */

	if (nocsim_node_by_id(state, id) != NULL) {
		Tcl_SetResult(interp, "a node with that ID exists already", NULL);
		return TCL_ERROR;
	}

	nocsim_grid_create_router(state, id, row, col, behavior);

	return TCL_OK;
}

/*** PE ID ROW COL behavior **************************************************/
interp_command(nocsim_create_PE) {
	nocsim_state* state = (nocsim_state*) data;
	char* id = NULL;
	char* behavior;
	int row = -1;
	int col = -1;

	req_args(5, "ID ROW COL BEHAVIOR");

	id = Tcl_GetStringFromObj(argv[1], NULL);
	get_int(interp, argv[2], &row);
	get_int(interp, argv[3], &col);
	behavior = Tcl_GetStringFromObj(argv[4], NULL);

	Tcl_IncrRefCount(argv[1]);
	Tcl_IncrRefCount(argv[4]);

	if (nocsim_node_by_id(state, id) != NULL) {
		Tcl_SetResult(interp, "a node with that ID exists already", NULL);
		return TCL_ERROR;
	}

	nocsim_grid_create_PE(state, id, row, col, behavior);

	return TCL_OK;
}

/*** link ID ID / link ID ID DIR / link ID ID DIR DIR ************************/
interp_command(nocsim_create_link) {
	nocsim_state* state = (nocsim_state*) data;
	char* src = NULL;
	char* dst = NULL;
	nocsim_direction from_dir = DIR_UNDEF;
	nocsim_direction to_dir = DIR_UNDEF;

	if (argc < 3 || argc > 5) {
		Tcl_WrongNumArgs(interp, 0, argv, "link ID ID / link ID ID DIR / link ID ID DIR DIR");
		return TCL_ERROR;
	}

	if (argc >= 3 ) {
		src = Tcl_GetStringFromObj(argv[1], NULL);
		dst = Tcl_GetStringFromObj(argv[2], NULL);
	}

	if (argc == 4) {

		get_int(interp, argv[3], (int*) &to_dir);

	}  else if (argc == 5) {

		get_int(interp, argv[3], (int*) &from_dir);
		get_int(interp, argv[4], (int*) &to_dir);

	}

	if (nocsim_grid_create_link(state, src, dst, from_dir, to_dir) != NOCSIM_RESULT_OK) {
		Tcl_SetResult(interp, state->errstr, NULL);
		return TCL_ERROR;
	}

	return TCL_OK;
}

/*** current *****************************************************************/
interp_command(nocsim_current) {
	UNUSED(argc);
	UNUSED(argv);

	nocsim_state* state = (nocsim_state*) data;

	if (state->current == NULL) {
		Tcl_SetResult(interp, "current may only be called during a behavior callback", NULL);
		return TCL_ERROR;
	}

	Tcl_SetResult(interp, state->current->id, NULL);
	return TCL_OK;
}

/*** step / step N ***********************************************************/
interp_command(nocsim_step_command) {
	int n = 1;

	nocsim_state* state = (nocsim_state*) data;

	if (argc == 2) {
		get_int(interp, argv[1], &n);
	} else if (argc == 1) { n = 1; } else {
		Tcl_WrongNumArgs(interp, 0, argv, "step / step N");
		return TCL_ERROR;
	}

	for (int i = 0 ; i < n ; i++ ) {
		nocsim_step(state, interp);
	}

	state->current = NULL;

	return TCL_OK;
}

/*** nodeinfo ID ATTR ********************************************************/
interp_command(nocsim_nodeinfo) {
	char* id;
	char* attr;
	nocsim_node* node;
	nocsim_state* state = (nocsim_state*) data;
	int length;

	req_args(3, "nodeinfo ID ATTR");

	id = Tcl_GetStringFromObj(argv[1], NULL);
	attr = Tcl_GetStringFromObj(argv[2], &length);

	node = nocsim_node_by_id(state, id);

	if (node == NULL) {
		Tcl_SetResult(interp, "no node found with requested id", NULL);
		return TCL_ERROR;
	}

	if (!strncmp(attr, "type", length)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(node->type));
		return TCL_OK;

	} else if (!strncmp(attr, "row", length)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(node->row));
		return TCL_OK;

	} else if (!strncmp(attr, "col", length)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(node->col));
		return TCL_OK;

	} else if (!strncmp(attr, "behavior", length)) {
		Tcl_SetResult(interp, node->behavior, NULL);
		return TCL_OK;

	} else if (!strncmp(attr, "injected", length)) {
		Tcl_SetObjResult(interp, Tcl_NewLongObj(node->injected));
		return TCL_OK;

	} else if (!strncmp(attr, "routed", length)) {
		Tcl_SetObjResult(interp, Tcl_NewLongObj(node->routed));
		return TCL_OK;

	} else if (!strncmp(attr, "spawned", length)) {
		Tcl_SetObjResult(interp, Tcl_NewLongObj(node->spawned));
		return TCL_OK;

	} else if (!strncmp(attr, "dequeued", length)) {
		Tcl_SetObjResult(interp, Tcl_NewLongObj(node->dequeued));
		return TCL_OK;

	} else if (!strncmp(attr, "backrouted", length)) {
		Tcl_SetObjResult(interp, Tcl_NewLongObj(node->backrouted));
		return TCL_OK;

	} else if (!strncmp(attr, "arrived", length)) {
		Tcl_SetObjResult(interp, Tcl_NewLongObj(node->arrived));
		return TCL_OK;

	} else {
		Tcl_SetResult(interp, "unknown attribute", NULL);
		return TCL_ERROR;
	}

}

/*** linkinfo FROM TO ATTR ***************************************************/
interp_command(nocsim_linkinfo) {
	char* from_str;
	char* to_str;
	char* attr;
	nocsim_state* state = (nocsim_state*) data;
	nocsim_link* l;
	int length;
	nocsim_direction d = DIR_UNDEF;

	req_args(4, "linkinfo FROM TO ATTR");
	from_str = Tcl_GetStringFromObj(argv[1], NULL);
	to_str = Tcl_GetStringFromObj(argv[2], NULL);
	attr = Tcl_GetStringFromObj(argv[3], &length);

	l = nocsim_link_by_nodes(state, from_str, to_str);

	if (l == NULL) {
		Tcl_SetResult(interp, "no such link", NULL);
		return TCL_ERROR;
	}

	if (!strncmp(attr, "current_load", length)) {

		/* XXX in the future, when links be come queues, this
		 * logic will need to be updated */
		if (l->flit == NULL) {
			Tcl_SetObjResult(interp, Tcl_NewIntObj(0));
		} else {
			Tcl_SetObjResult(interp, Tcl_NewIntObj(1));
		}
		return TCL_OK;

	} else if (!strncmp(attr, "in_flight", length)) {
		Tcl_Obj* listPtr = Tcl_NewListObj(0, NULL);
		if (l->flit != NULL) {
			Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewIntObj(l->flit->flit_no));
		}
		Tcl_SetObjResult(interp, listPtr);
		return TCL_OK;

	} else if (!strncmp(attr, "load", length)) {
		Tcl_SetObjResult(interp, Tcl_NewLongObj(l->load));
		return TCL_OK;

	} else if (!strncmp(attr, "from_dir", length)) {
		for (nocsim_direction i = N; i < DIR_UNDEF; i++) {
			if (l->from->outgoing[i] == l) { d = i; break; }
		}

		if (d == DIR_UNDEF) {
			Tcl_SetResult(interp, "simulation state corrupted: link does not originate from it's origin", NULL);
			return TCL_ERROR;
		}
		Tcl_SetObjResult(interp, Tcl_NewIntObj((int) d));
		return TCL_OK;

	} else if (!strncmp(attr, "to_dir", length)) {
		for (nocsim_direction i = N; i < DIR_UNDEF; i++) {
			if (l->to->incoming[i] == l) { d = i; break; }
		}

		if (d == DIR_UNDEF) {
			Tcl_SetResult(interp, "simulation state corrupted: link does not originate from it's origin", NULL);
			return TCL_ERROR;
		}
		Tcl_SetObjResult(interp, Tcl_NewIntObj((int) d));
		return TCL_OK;

	} else {
		Tcl_SetResult(interp, "invalid attribute", NULL);
		return TCL_ERROR;
	}
}


/*** findnode / findnode ROW COL / findnode ROWL ROWU COLL COLU **************/
interp_command(nocsim_findnode) {
	int rowl, rowu, coll, colu;
	unsigned int i;
	nocsim_state* state = (nocsim_state*) data;
	nocsim_node* cursor;
	Tcl_Obj* listPtr;

	if (argc < 2) {
		rowl = 0;
		coll = 0;
		rowu = state->max_row;
		colu = state->max_col;
	} else if (argc == 3) {
		get_int(interp, argv[1], &rowl);
		rowu = rowl;
		get_int(interp, argv[2], &coll);
		colu = coll;
	} else if (argc == 5) {
		get_int(interp, argv[1], &rowl);
		get_int(interp, argv[2], &rowu);
		get_int(interp, argv[3], &coll);
		get_int(interp, argv[4], &colu);
	} else {
		Tcl_WrongNumArgs(interp, 0, argv,
				"findnode / findnode ROW COL / findnode ROWL ROWU COLL COLU");
		return TCL_ERROR;
	}

	listPtr = Tcl_NewListObj(0, NULL);
	vec_foreach(state->nodes, cursor, i) {
		if (cursor->id == NULL) {
			err(1, "node@0x%p missing ID", (void*) cursor);
		}

		if (	(cursor->row < (unsigned int) rowl) ||
			(cursor->row > (unsigned int) rowu) ||
			(cursor->col < (unsigned int) coll) ||
			(cursor->col > (unsigned int) colu)	) { continue; }
		Tcl_ListObjAppendElement(interp, listPtr, str2obj(cursor->id));
	}

	Tcl_SetObjResult(interp, listPtr);
	return TCL_OK;
}

/*** behavior ID BEHAVIOR ****************************************************/
interp_command(nocsim_set_behavior) {
	nocsim_state* state = (nocsim_state*) data;
	char* id = NULL;
	char* behavior;
	nocsim_node* node;

	req_args(3, "ID BEHAVIOR");

	id = Tcl_GetStringFromObj(argv[1], NULL);
	behavior = Tcl_GetStringFromObj(argv[2], NULL);
	Tcl_IncrRefCount(argv[2]);

	node = nocsim_node_by_id(state, id);

	if (node == NULL) {
		Tcl_SetResult(interp, "no node found with requested id", NULL);
		return TCL_ERROR;
	}

	/* XXX: need to free old value? */
	node->behavior = behavior;

	return TCL_OK;
}

/*** randnode / randnode ROW COL / randnode ID *******************************/
interp_command(nocsim_randnode) {
	nocsim_state* state = (nocsim_state*) data;
	char* exclude = "";
	int excluderow = state->max_row + 1;
	int excludecol = state->max_col + 1;
	unsigned int i;
	nocsim_node* node;
	unsigned int counter = 0;

	if (argc == 1) {
	} else if (argc == 2) {
		exclude = Tcl_GetStringFromObj(argv[1], NULL);
	} else if (argc == 3) {
		get_int(interp, argv[1], &excluderow);
		get_int(interp, argv[2], &excludecol);
	} else {
		Tcl_WrongNumArgs(interp, 0, argv,
				"randnode / randnode ROW COL / randnode ID");
		return TCL_ERROR;
	}

	/* keep picking a node until we find one that works */
	do {
		i = randrange(0, state->nodes->length);
		node = state->nodes->data[i];

		/* prevent infinite loops */
		counter ++;
		if (counter > state->num_node * 1000) {
			Tcl_SetResult(interp, "no suitable node found", NULL);
			return TCL_ERROR;
		}

		/* node ID does not match ID, node row and col don't mach
		 * ROW, COL, and the type does not match PE */
	} while (	(!strncmp(exclude, node->id, NOCSIM_GRID_LINELEN)) ||
			(	(node->row == (unsigned int) excluderow) &&
				(node->col == (unsigned int) excludecol)	) ||
			(node->type != node_PE)
		);

	Tcl_SetResult(interp, node->id, NULL);
	return TCL_OK;

}

/*** graphviz ****************************************************************/
interp_command(nocsim_graphviz) {
	UNUSED(interp);
	UNUSED(argc);
	UNUSED(argv);

	nocsim_state* state = (nocsim_state*) data;

	/* TODO: should return result to TCL */
	nocsim_dump_graphviz(stderr, state);

	return TCL_OK;
}

/*** spawn TO ****************************************************************/
interp_command(nocsim_spawn_command) {
	nocsim_state* state = (nocsim_state*) data;
	char* to_id;
	nocsim_node* node;

	req_args(2, "spawn TO");

	if (state->current == NULL) {
		Tcl_SetResult(interp, "spawn may only be called during a behavior callback", NULL);
		return TCL_ERROR;
	}

	to_id = Tcl_GetStringFromObj(argv[1], NULL);

	node = nocsim_node_by_id(state, to_id);
	if (node == NULL) {
		Tcl_SetResult(interp, "no node found with requested id", NULL);
		return TCL_ERROR;
	}

	if (state->current->type != node_PE) {
		Tcl_SetResult(interp, "originating node must be a PE", NULL);
		return TCL_ERROR;
	}

	if (node->type != node_PE) {
		Tcl_SetResult(interp, "destination node must be a PE", NULL);
		return TCL_ERROR;
	}

	if (node == state->current) {
		Tcl_SetResult(interp, "source and destination may not be the same", NULL);
		return TCL_ERROR;
	}

	nocsim_spawn(state, state->current, node);

	return TCL_OK;

}

/*** route FROM TO ***********************************************************/
interp_command(nocsim_route_command) {
	nocsim_state* state = (nocsim_state*) data;
	nocsim_direction from;
	nocsim_direction to;
	nocsim_flit* flit;
	nocsim_node* from_node;
	nocsim_node* to_node;

	req_args(3, "route FROM TO");

	get_int(interp, argv[1], (int*) &from);
	get_int(interp, argv[2], (int*) &to);

	validate_direction(from);
	validate_direction(to);

	/* must be called during a behavior */
	if (state->current == NULL) {
		Tcl_SetResult(interp, "route may only be called during a behavior callback", NULL);
		return TCL_ERROR;
	}

	/* only applicable to routers, since PEs cannot route */
	if (state->current->type != node_router) {
		Tcl_SetResult(interp, "route may only be called for router nodes", NULL);
		return TCL_ERROR;
	}

	/* make sure the relevant links exist */
	if (state->current->incoming[from] == NULL) {
		Tcl_SetResult(interp, "no incoming link from specified direction", NULL);
		return TCL_ERROR;
	}

	if (state->current->outgoing[to] == NULL) {
		Tcl_SetResult(interp, "no outgoing link to specified direction", NULL);
		return TCL_ERROR;
	}

	/* each link can accept only one flit per cycle */
	if (state->current->outgoing[to]->flit_next != NULL) {
		Tcl_SetResult(interp, "cannot route multiple flits through the same outgoing link", NULL);
		return TCL_ERROR;
	}

	/* TODO: should probably have a nocsim_route function in simulation.c
	 * */

	/* route callback */
	flit = state->current->incoming[from]->flit;
	from_node = state->current->incoming[from]->from;
	to_node = state->current->outgoing[to]->to;

	state->routed ++;
	if (state->instruments[INSTRUMENT_ROUTE] != NULL) {
		if (Tcl_Evalf(state->interp, "%s \"%s\" \"%s\" %lu %lu %lu %lu \"%s\" \"%s\"",
					state->instruments[INSTRUMENT_ROUTE],
					flit->from->id, flit->to->id,
					flit->flit_no,
					flit->spawned_at,
					flit->injected_at,
					flit->hops,
					from_node->id, to_node->id
					)) {
			print_tcl_error(state->interp);
			err(1, "unable to proceed, exiting with failure state");
		}
	}

	/* if we are being routed somewhere that isn't our origin, then this
	 * counts as an injection event */
	if ((flit->from != to_node) && (flit->from == from_node)) {
		if (state->instruments[INSTRUMENT_INJECT] != NULL) {
			if (Tcl_Evalf(state->interp, "%s \"%s\" \"%s\" %lu",
					state->instruments[INSTRUMENT_INJECT],
					flit->from->id,
					flit->to->id,
					flit->flit_no)) {
				print_tcl_error(interp);
				err(1, "unable to proceed, exiting with failure state");
			}
		}

		state->injected ++;
		flit->from->injected ++;
	}

	/* performance counters */
	state->current->routed ++;
	state->current->outgoing[to]->load ++;
	flit->hops ++;

	/* move flit to next state */
	state->current->outgoing[to]->flit_next = \
		state->current->incoming[from]->flit;
	state->current->incoming[from]->flit = NULL;

	return TCL_OK;
}

/*** drop FROM ***&***********************************************************/
interp_command(nocsim_drop_command) {
	nocsim_state* state = (nocsim_state*) data;
	nocsim_direction from;

	req_args(2, "drop FROM");

	get_int(interp, argv[1], (int*) &from);

	validate_direction(from);

	/* must be called during a behavior */
	if (state->current == NULL) {
		Tcl_SetResult(interp, "drop may only be called during a behavior callback", NULL);
		return TCL_ERROR;
	}

	/* only applicable to routers, since PEs cannot route */
	if (state->current->type != node_router) {
		Tcl_SetResult(interp, "drop may only be called for router nodes", NULL);
		return TCL_ERROR;
	}

	/* make sure the relevant links exist */
	if (state->current->incoming[from] == NULL) {
		Tcl_SetResult(interp, "no incoming link from specified direction", NULL);
		return TCL_ERROR;
	}

	/* drop the flit */
	free(state->current->incoming[from]->flit);
	state->current->incoming[from]->flit = NULL;

	return TCL_OK;
}

/*** peek DIR ATTR ***********************************************************/
interp_command(nocsim_peek_command) {
	nocsim_state* state = (nocsim_state*) data;
	nocsim_direction dir;
	char* attr;
	int length;

	req_args(3, "peek DIR ATTR");

	get_int(interp, argv[1], (int*) &dir);
	attr = Tcl_GetStringFromObj(argv[2], &length);

	validate_direction(dir);

	if (state->current == NULL) {
		Tcl_SetResult(interp, "route may only be called during a behavior callback", NULL);
		return TCL_ERROR;
	}

	if (state->current->type != node_router) {
		Tcl_SetResult(interp, "route may only be called for router nodes", NULL);
		return TCL_ERROR;
	}

	if (state->current->incoming[dir] == NULL) {
		Tcl_SetResult(interp, "no link in specified direction", NULL);
		return TCL_ERROR;
	}

	if (state->current->incoming[dir]->flit == NULL) {
		Tcl_SetResult(interp, "no flit incoming from specified direction", NULL);
		return TCL_ERROR;
	}

	if (!strncmp(attr, "from", length)) {
		Tcl_SetObjResult(interp, str2obj(state->current->incoming[dir]->flit->from->id));
		return TCL_OK;

	} else if (!strncmp(attr, "to", length)) {
		Tcl_SetObjResult(interp, str2obj(state->current->incoming[dir]->flit->to->id));
		return TCL_OK;

	} else if (!strncmp(attr, "from_row", length)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(state->current->incoming[dir]->flit->from->row));
		return TCL_OK;

	} else if (!strncmp(attr, "from_col", length)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(state->current->incoming[dir]->flit->from->col));
		return TCL_OK;

	} else if (!strncmp(attr, "to_row", length)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(state->current->incoming[dir]->flit->to->row));
		return TCL_OK;

	} else if (!strncmp(attr, "to_col", length)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(state->current->incoming[dir]->flit->to->col));
		return TCL_OK;

	} else if (!strncmp(attr, "spawned_at", length)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(state->current->incoming[dir]->flit->spawned_at));
		return TCL_OK;

	} else if (!strncmp(attr, "injected_at", length)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(state->current->incoming[dir]->flit->injected_at));
		return TCL_OK;

	} else {
		Tcl_SetObjResult(interp, str2obj("unrecognized attribute"));
		return TCL_OK;
	}
}

/*** incoming DIR ************************************************************/
interp_command(nocsim_incoming_command) {
	nocsim_state* state = (nocsim_state*) data;
	nocsim_direction dir;

	/* just check one direction */
	req_args(2, "incoming DIR");
	get_int(interp, argv[1], (int*) &dir);
	validate_direction(dir);

	if (state->current == NULL) {
		Tcl_SetResult(interp, "route may only be called during a behavior callback", NULL);
		return TCL_ERROR;
	}

	if (state->current->type != node_router) {
		Tcl_SetResult(interp, "route may only be called for router nodes", NULL);
		return TCL_ERROR;
	}

	if (state->current->incoming[dir] != NULL) {
		if (state->current->incoming[dir]->flit != NULL) {
			Tcl_SetObjResult(interp, Tcl_NewIntObj(1));
		} else {
			goto none_incoming;
		}
	} else {
none_incoming:
		Tcl_SetObjResult(interp, Tcl_NewIntObj(0));
	}

	return TCL_OK;

}

/*** allincoming *************************************************************/
interp_command(nocsim_allincoming_command) {
	nocsim_state* state = (nocsim_state*) data;

	/* just check one direction */
	req_args(1, "allincoming");

	if (state->current == NULL) {
		Tcl_SetResult(interp, "route may only be called during a behavior callback", NULL);
		return TCL_ERROR;
	}

	if (state->current->type != node_router) {
		Tcl_SetResult(interp, "route may only be called for router nodes", NULL);
		return TCL_ERROR;
	}

	/* retrieve a list of all directions from which there are incoming */
	/* flits  */
	Tcl_Obj* listPtr = Tcl_NewListObj(0, NULL);
	for (nocsim_direction dir = 0 ; dir < DIR_UNDEF ; dir++) {
		if (state->current->incoming[dir] != NULL) {
			if (state->current->incoming[dir]->flit != NULL) {
				Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewIntObj(dir));
			}

		}
	}
	Tcl_SetObjResult(interp, listPtr);
	return TCL_OK;


}

/*** avail DIR ***************************************************************/
interp_command(nocsim_avail_command) {
	nocsim_state* state = (nocsim_state*) data;
	nocsim_direction dir;

	req_args(2, "avail DIR");

	get_int(interp, argv[1], (int*) &dir);

	validate_direction(dir);

	if (state->current == NULL) {
		Tcl_SetResult(interp, "route may only be called during a behavior callback", NULL);
		return TCL_ERROR;
	}

	if (state->current->type != node_router) {
		Tcl_SetResult(interp, "route may only be called for router nodes", NULL);
		return TCL_ERROR;
	}

	if (state->current->outgoing[dir] == NULL) {
		/* no such link */
		Tcl_SetObjResult(interp, Tcl_NewIntObj(2));

	} else if (state->current->outgoing[dir]->flit_next == NULL) {
		/* available */
		Tcl_SetObjResult(interp, Tcl_NewIntObj(0));

	} else {
		/* already used */
		Tcl_SetObjResult(interp, Tcl_NewIntObj(1));
	}

	return TCL_OK;
}

/*** dir2int DIR *************************************************************/
interp_command(nocsim_dir2int) {
	UNUSED(data);
	nocsim_direction result = DIR_UNDEF;
	char* dir;

	req_args(2, "dir2int DIR");

	dir = Tcl_GetStringFromObj(argv[1], NULL);

	result = NOCSIM_STR_TO_DIRECTION(dir);

	if (result <= P) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj((int) result));
		return TCL_OK;
	} else {
		char* errstr = alloc_printf("invalid_direction: %s", dir);
		Tcl_SetObjResult(interp, str2obj(errstr));
		free(errstr);
		return TCL_ERROR;
	}
}

/*** int2dir DIR *************************************************************/
interp_command(nocsim_int2dir) {
	UNUSED(data);
	char* result = NULL;
	nocsim_direction dir;

	req_args(2, "int2dir DIR");

	get_int(interp, argv[1], (int*) &dir);

	result = NOCSIM_DIRECTION_TO_STR(dir);

	if (dir <= P) {
		Tcl_SetObjResult(interp, str2obj(result));
		return TCL_OK;
	} else {
		Tcl_SetObjResult(interp, str2obj("invalid direction"));
		return TCL_ERROR;
	}
}


/*** type2int DIR *************************************************************/
interp_command(nocsim_type2int) {
	UNUSED(data);
	nocsim_node_type result = type_undefined;
	char* type;

	req_args(2, "type2int DIR");

	type = Tcl_GetStringFromObj(argv[1], NULL);

	result = NOCSIM_STR_TO_NODE_TYPE(type);

	if (result != type_undefined) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj((int) result));
		return TCL_OK;
	} else {
		Tcl_SetObjResult(interp, str2obj("invalid type"));
		return TCL_ERROR;
	}
}

/*** int2type DIR *************************************************************/
interp_command(nocsim_int2type) {
	UNUSED(data);
	char* result = NULL;
	nocsim_node_type type;

	req_args(2, "int2type DIR");

	get_int(interp, argv[1], (int*) &type);

	result = NOCSIM_NODE_TYPE_TO_STR(type);

	if (type == node_router || type == node_PE) {
		Tcl_SetObjResult(interp, str2obj(result));
		return TCL_OK;
	} else {
		Tcl_SetObjResult(interp, str2obj("invalid type"));
		return TCL_ERROR;
	}
}

/*** registerinstrument INSTRUMENT PROCEDURE *********************************/
interp_command(nocsim_registerinstrument) {
	nocsim_state* state = (nocsim_state*) data;
	char* instrument_str;
	nocsim_instrument instrument;
	char* procedure;

	req_args(3, "registerinstrument INSTRUMENT PROCEDURE");

	instrument_str = Tcl_GetStringFromObj(argv[1], NULL);
	procedure = Tcl_GetStringFromObj(argv[2], NULL);

	instrument = NOCSIM_STR_TO_INSTRUMENT(instrument_str);

	if (instrument == INSTRUMENT_UNDEFINED) {
		Tcl_SetObjResult(interp, str2obj("unknown instrument"));
		return TCL_ERROR;
	}

	Tcl_IncrRefCount(argv[2]);  /* procedure */
	state->instruments[instrument] = procedure;

	return TCL_OK;
}

/*** conswrite STR ***********************************************************/
interp_command(nocsim_conswrite) {

	char* str;

	if (argc < 2) {
		Tcl_WrongNumArgs(interp, 0, argv, "conswrite STR ...");
		return TCL_ERROR;
	}

	for (int i = 1 ; i < argc ; i++) {

		str = Tcl_GetStringFromObj(argv[1], NULL);

		UNUSED(data);
		printf("%s\n", str);
	}

	return TCL_OK;
}

/*** errwrite STR ***********************************************************/
interp_command(nocsim_errwrite) {

	char* str;

	if (argc < 2) {
		Tcl_WrongNumArgs(interp, 0, argv, "errwrite STR ...");
		return TCL_ERROR;
	}


	for (int i = 1 ; i < argc ; i++) {

		str = Tcl_GetStringFromObj(argv[1], NULL);

		UNUSED(data);
		fprintf(stderr, "%s\n", str);
	}


	return TCL_OK;
}

/*** allnodes ****************************************************************/
interp_command(nocsim_allnodes_command) {
	nocsim_state* state = (nocsim_state*) data;
	nocsim_node* cursor;
	unsigned int i;
	Tcl_Obj* listPtr;

	req_args(1, "allnodes");

	listPtr = Tcl_NewListObj(0, NULL);

	vec_foreach(state->nodes, cursor, i) {
		Tcl_ListObjAppendElement(interp, listPtr, str2obj(cursor->id));
	}

	Tcl_SetObjResult(interp, listPtr);
	return TCL_OK;

}


/*** interpreter implementation **********************************************/

/* XXX: in the future, this should really be split into actual simulation
 * state initialization in simulation.c, and Tcl initialization in nocsim.c.
 *
 * This function is written this way to reduce the friction of going from a C
 * program that instantiates a Tcl interpreter, to being a Tcl library.
 */

void nocsim_create_state(Tcl_Interp* interp, nocsim_state* state) {
	nodelist* l;
	linklist* links;

/*** initialize nocsim state *************************************************/
	alloc(sizeof(nodelist), l);
	alloc(sizeof(linklist), links);

	state->RNG_seed = (unsigned int) time(NULL);
	state->num_PE = 0;
	state->num_router = 0;
	state->num_node = 0;
	state->flit_no = 0;
	state->tick = 0;
	state->title = NULL; /* allocated as a linked var later */
	state->current = NULL;
	state->max_row = 0;
	state->max_col = 0;
	state->injected = 0;
	state->dequeued = 0;
	state->spawned = 0;
	state->backrouted = 0;
	state->routed = 0;
	state->arrived = 0;
	state->errstr = NULL;

	for (int i = 0 ; i < (int) ENUMSIZE_INSTRUMENT ; i++) {
		state->instruments[i] = NULL;
	}

	vec_init(l);
	state->nodes = l;
	state->node_map = kh_init(nnptr);

	vec_init(links);
	state->links = links;


#define defcmd(func, name) \
	Tcl_CreateObjCommand(interp, name, \
			func, (ClientData) state, \
			(Tcl_CmdDeleteProc*) NULL); \

	defcmd(nocsim_create_router, "nocsim::router");
	defcmd(nocsim_create_PE, "nocsim::PE");
	defcmd(nocsim_create_link, "nocsim::link");
	defcmd(nocsim_current, "nocsim::current");
	defcmd(nocsim_graphviz, "nocsim::graphviz");
	defcmd(nocsim_step_command, "nocsim::step");
	defcmd(nocsim_nodeinfo, "nocsim::nodeinfo");
	defcmd(nocsim_findnode, "nocsim::findnode");
	defcmd(nocsim_set_behavior, "nocsim::behavior");
	defcmd(nocsim_randnode, "nocsim::randnode");
	defcmd(nocsim_spawn_command, "nocsim::inject");  /* alias */
	defcmd(nocsim_spawn_command, "nocsim::spawn");
	defcmd(nocsim_route_command, "nocsim::route");
	defcmd(nocsim_peek_command, "nocsim::peek");
	defcmd(nocsim_avail_command, "nocsim::avail");
	defcmd(nocsim_dir2int, "nocsim::dir2int");
	defcmd(nocsim_int2dir, "nocsim::int2dir");
	defcmd(nocsim_type2int, "nocsim::type2int");
	defcmd(nocsim_int2type, "nocsim::int2type");
	defcmd(nocsim_linkinfo, "nocsim::linkinfo");
	defcmd(nocsim_registerinstrument, "nocsim::registerinstrument");
	defcmd(nocsim_conswrite, "nocsim::conswrite");
	defcmd(nocsim_errwrite, "nocsim::errwrite");
	defcmd(nocsim_incoming_command, "nocsim::incoming");
	defcmd(nocsim_allincoming_command, "nocsim::allincoming");
	defcmd(nocsim_drop_command, "nocsim::drop");
	defcmd(nocsim_nodecolor_command, "nocsim::nodecolor");
	defcmd(nocsim_allnodes_command, "nocsim::allnodes");

#undef defcmd

/*** establish linked variables **********************************************/

#define link(typ, sym, ptr, flags) do { \
	typ* temp = (typ*) Tcl_Alloc(sizeof(typ)); \
	temp = (typ*) ptr; \
	Tcl_LinkVar(interp, sym, (char*) temp, flags); \
	} while (0);

	link(int, "nocsim::nocsim_RNG_seed", &(state->RNG_seed), TCL_LINK_INT | TCL_LINK_READ_ONLY);
	link(int, "nocsim::nocsim_num_PE", &(state->num_PE), TCL_LINK_INT | TCL_LINK_READ_ONLY);
	link(int, "nocsim::nocsim_num_router", &(state->num_router), TCL_LINK_INT | TCL_LINK_READ_ONLY);
	link(int, "nocsim::nocsim_num_node", &(state->num_node), TCL_LINK_INT | TCL_LINK_READ_ONLY);
	link(long, "nocsim::nocsim_flit_no", &(state->flit_no), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
	link(long, "nocsim::nocsim_tick", &(state->tick), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
	link(long, "nocsim::nocsim_injected", &(state->injected), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
	link(long, "nocsim::nocsim_spawned", &(state->spawned), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
	link(long, "nocsim::nocsim_dequeued", &(state->dequeued), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
	link(long, "nocsim::nocsim_backrouted", &(state->backrouted), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
	link(long, "nocsim::nocsim_routed", &(state->routed), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
	link(long, "nocsim::nocsim_arrived", &(state->arrived), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
#undef link

	state->title = (char*) Tcl_Alloc(sizeof(char) * 512);
	snprintf(state->title, 512, "unspecified");
	Tcl_LinkVar(interp, "nocsim::nocsim_title", (char*) &(state->title), TCL_LINK_STRING);

	Tcl_Obj* vers = Tcl_NewListObj(0, NULL);
	Tcl_ListObjAppendElement(interp, vers, str2obj(NOC_TOOLS_VERSION_MAJOR));
	Tcl_ListObjAppendElement(interp, vers, str2obj(NOC_TOOLS_VERSION_MINOR));
	Tcl_ListObjAppendElement(interp, vers, str2obj(NOC_TOOLS_VERSION_PATCH));
	Tcl_ListObjAppendElement(interp, vers, str2obj(NOC_TOOLS_VERSION_SUFFIX));
	Tcl_ObjSetVar2(interp, str2obj("nocsim::nocsim_version"), NULL, vers, 0);

	state->interp = interp;
}

#undef get_int
#undef interp_command
#undef req_args
#undef validate_direction
