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

/*** step ********************************************************************/
interp_command(nocsim_step_command) {
	UNUSED(interp);
	UNUSED(argc);
	UNUSED(argv);

	nocsim_state* state = (nocsim_state*) data;

	if (state->enable_simulation == 1) {
		nocsim_step(state);
	}

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

	Tcl_CreateObjCommand(interp, "graphviz",
			nocsim_graphviz, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);


	Tcl_CreateObjCommand(interp, "step",
			nocsim_step_command, (ClientData) state,
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
