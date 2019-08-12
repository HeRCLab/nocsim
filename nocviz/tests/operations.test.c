/* test suite for operations */

#include "../operations.h"
#include <tcl.h>

int main() {

	nocviz_op* op;
	Tcl_Interp* interp;
	int x;

	/* test the we can safely allocate and de-allocate an operation */
	op = nocviz_op_init("abc", "123");

	if (strcmp(op->script, "abc") != 0) {
		return 1;
	}

	if (strcmp(op->description, "123") != 0) {
		return 2;
	}

	nocviz_op_free(op);

	/* test that we can execute the script via TCL */

	interp = Tcl_CreateInterp();

	op = nocviz_op_init("expr 1 + 1", "dummy");

	if (nocviz_op_execute(op, interp) != TCL_OK) { return 3; }

	if (Tcl_GetIntFromObj(interp, Tcl_GetObjResult(interp), &x) != TCL_OK) { return 4; }

	if (x != 2) { return 5; }

	nocviz_op_free(op);

	return 0;
}
