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

/*** router ID ROW COL *******************************************************/
interp_command(nocsim_create_router) {
	UNUSED(data);

	char* id = NULL;
	int row = -1;
	int col = -1;

	req_args(4, "ID ROW COL");

	/* if (argc != 4) { */
	/*         Tcl_WrongNumArgs(interp, 0, argv, "ID ROW COL"); */
	/*         return TCL_ERROR; */
	/* } */

	id = Tcl_GetStringFromObj(argv[1], NULL);
	get_int(interp, argv[2], &row);
	get_int(interp, argv[3], &col);

	dbprintf("creating router id '%s' row, col = %i, %i\n", id, row, col);

	return TCL_OK;

}

/*** interpreter implementation **********************************************/

void nocsim_interp(FILE* stream) {
	nocsim_state* state;
	Tcl_Interp *interp;
	char* line;
	size_t length;
	nodelist* l;

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

	Tcl_CreateObjCommand(interp, "nocsim_create_router",
			nocsim_create_router, (ClientData) NULL,
			(Tcl_CmdDeleteProc*) NULL);

/*** main interpreter REPL ***************************************************/
	while (getline(&line, &length, stream) != -1) {
		Tcl_Eval(interp, line);
		printf("%s\n", Tcl_GetStringResult(interp));
	}

/*** clean up ****************************************************************/
} /* void nocsim_interp(FILE* stream) */

#undef get_int
#undef interp_command
#undef req_args
