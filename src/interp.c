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

/*** config KEY VAL **********************************************************/
interp_command(nocsim_config) {
	nocsim_state* state = (nocsim_state*) data;
	char* key = NULL;
	char* val = NULL;

	req_args(3, "KEY VAL");

	key = Tcl_GetStringFromObj(argv[1], NULL);
	val = Tcl_GetStringFromObj(argv[2], NULL);

	nocsim_grid_config(state, key, val);

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

void nocsim_interp(FILE* stream) {
	nocsim_state* state;
	Tcl_Interp *interp;
	char* line;
	size_t length = 0;
	nodelist* l;
	int rc;

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
	state->title = "unspecified";

	vec_init(l);
	state->nodes = l;

/*** initialize TCL interpreter **********************************************/
	interp = Tcl_CreateInterp();
	if (interp == NULL) {
		fprintf(stderr, "failed to create interpreter!\n");
		exit(1);
	}

	Tcl_Eval(interp, "set argv0 \"nocsim\"");

	Tcl_CreateObjCommand(interp, "router",
			nocsim_create_router, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "PE",
			nocsim_create_PE, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "link",
			nocsim_create_link, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "config",
			nocsim_config, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);

	Tcl_CreateObjCommand(interp, "graphviz",
			nocsim_graphviz, (ClientData) state,
			(Tcl_CmdDeleteProc*) NULL);



/*** main interpreter REPL ***************************************************/
	Tcl_EvalFile(interp, "/dev/stdin");
	/* while (getline(&line, &length, stream) != -1) { */
	/*         rc = Tcl_Eval(interp, line); */
	/*         if (rc != TCL_OK) { */
	/*                 printf("TCL error: %s\n", Tcl_GetStringResult(interp)); */
	/*         } */
	/* } */

/*** clean up ****************************************************************/
} /* void nocsim_interp(FILE* stream) */

#undef get_int
#undef interp_command
#undef req_args
