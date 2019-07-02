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

	nocsim_grid_create_PE(state, id, row, col, behavior);

	return TCL_OK;
}

/*** link ID ID **************************************************************/
interp_command(nocsim_create_link) {
	nocsim_state* state = (nocsim_state*) data;
	char* src = NULL;
	char* dst = NULL;

	req_args(3, "SRCID DSTID");

	src = Tcl_GetStringFromObj(argv[1], NULL);
	dst = Tcl_GetStringFromObj(argv[2], NULL);

	nocsim_grid_create_link(state, src, dst);

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
	UNUSED(interp);
	int n = 1;

	nocsim_state* state = (nocsim_state*) data;

	if (argc > 2) {
		get_int(interp, argv[1], &n);
	}

	if (state->enable_simulation == 1) {
		for (int i = 0 ; i < n ; i++ ) {
			nocsim_step(state);
		}
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

	req_args(3, "ID ATTR");

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
	} else {
		Tcl_SetResult(interp, "unknown attribute", NULL);
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
	char* exclude;
	int excluderow = state->max_row + 1;
	int excludecol = state->max_col + 1;
	unsigned int i;
	nocsim_node* node;
	unsigned int counter = 0;

	if (argc == 1) {
		exclude = "";
	} else if (argc == 2) {
		exclude = Tcl_GetStringFromObj(argv[1], NULL);
	} else if (argc == 3) {
		get_int(interp, argv[1], &excluderow);
		get_int(interp, argv[2], &excludecol);
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

/*** interpreter implementation **********************************************/

void nocsim_interp(char* scriptfile, char* runme, int argc, char** argv) {
	nocsim_state* state;
	Tcl_Interp *interp;
	nodelist* l;
	char* tcl_library_path;

/*** initialize nocsim state *************************************************/
	alloc(sizeof(nocsim_state), state);
	alloc(sizeof(nodelist), l);

	state->RNG_seed = (unsigned int) time(NULL);
	state->num_PE = 0;
	state->num_router = 0;
	state->num_node = 0;
	state->flit_no = 0;
	state->tick = 0;
	state->default_P_inject = 0;
	state->title = NULL; /* allocated as a linked var later */
	state->current = NULL;
	state->max_row = 0;
	state->max_col = 0;

	if (runme == NULL) {
		state->enable_simulation = 1;
	} else {
		state->enable_simulation = 0;
	}

	vec_init(l);
	state->nodes = l;

/*** initialize TCL interpreter **********************************************/

	/* this needs to happen before we initialize the interpreter, because
	 * TCL does exciting things with threading that will break if we call
	 * popen ourselves */
	tcl_library_path = get_tcl_library_path();

	interp = Tcl_CreateInterp();
	if (interp == NULL) {
		fprintf(stderr, "failed to create interpreter!\n");
		exit(1);
	}

	if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
		err(1, "failed to initialize stubs\n");
	}

	Tcl_SetVar(interp, "argv0", argv[0], 0);
	Tcl_SetVar(interp, "argc", i2sstr(argc), 0);
	Tcl_Eval(interp, "set argv {}");
	for (int i = 1 ; i < argc ; i++) {
		Tcl_SetVar(interp, "argv", argv[i], TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
	}
	Tcl_SetVar(interp, "tcl_library", tcl_library_path, 0);
	Tcl_Eval(interp, __extension__ ({
		char buf[512];
		snprintf(buf, sizeof(buf), "source %s/init.tcl", tcl_library_path);
		buf;}));


	Tcl_CreateObjCommand(interp, "router",
			nocsim_create_router, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "PE",
			nocsim_create_PE, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "link",
			nocsim_create_link, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "current",
			nocsim_current, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "graphviz",
			nocsim_graphviz, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "step",
			nocsim_step_command, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "nodeinfo",
			nocsim_nodeinfo, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "findnode",
			nocsim_findnode, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "behavior",
			nocsim_set_behavior, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "randnode",
			nocsim_randnode, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);



/*** establish linked variables **********************************************/

#define link(typ, sym, ptr, flags) do { \
	typ* temp = (typ*) Tcl_Alloc(sizeof(typ)); \
	temp = (typ*) ptr; \
	Tcl_LinkVar(interp, sym, (char*) temp, flags); \
	} while (0);

	link(int, "RNG_seed", &(state->RNG_seed), TCL_LINK_INT | TCL_LINK_READ_ONLY);
	link(int, "num_PE", &(state->num_PE), TCL_LINK_INT | TCL_LINK_READ_ONLY);
	link(int, "num_router", &(state->num_router), TCL_LINK_INT | TCL_LINK_READ_ONLY);
	link(int, "num_node", &(state->num_node), TCL_LINK_INT | TCL_LINK_READ_ONLY);
	link(long, "flit_no", &(state->flit_no), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
	link(long, "tick", &(state->tick), TCL_LINK_WIDE_INT | TCL_LINK_READ_ONLY);
#undef link

	state->title = (char*) Tcl_Alloc(sizeof(char) * 512);
	snprintf(state->title, 512, "unspecified");
	Tcl_LinkVar(interp, "title", (char*) &(state->title), TCL_LINK_STRING);

/*** setup helper variables **************************************************/
	Tcl_SetVar(interp, ezcat("dir_", NOCSIM_DIRECTION_TO_STR(0)), "0", 0);
	Tcl_SetVar(interp, ezcat("dir_", NOCSIM_DIRECTION_TO_STR(1)), "1", 0);
	Tcl_SetVar(interp, ezcat("dir_", NOCSIM_DIRECTION_TO_STR(2)), "2", 0);
	Tcl_SetVar(interp, ezcat("dir_", NOCSIM_DIRECTION_TO_STR(3)), "3", 0);
	Tcl_SetVar(interp, ezcat("dir_", NOCSIM_DIRECTION_TO_STR(4)), "4", 0);

	Tcl_SetVar(interp, ezcat("type_", NOCSIM_NODE_TYPE_TO_STR(0)), "0", 0);
	Tcl_SetVar(interp, ezcat("type_", NOCSIM_NODE_TYPE_TO_STR(1)), "1", 0);

/*** main interpreter REPL ***************************************************/
	if (Tcl_EvalFile(interp, scriptfile) != TCL_OK) {
		print_tcl_error(interp);
		err(1, "unable to proceed, exiting with failure state");
	}

	if (runme != NULL) {
		if (Tcl_Eval(interp, runme) != TCL_OK) {
			print_tcl_error(interp);
			err(1, "unable to proceed, exiting with failure state");
		}

	}


/*** clean up ****************************************************************/
} /* void nocsim_interp(FILE* stream) */

#undef get_int
#undef interp_command
#undef req_args
