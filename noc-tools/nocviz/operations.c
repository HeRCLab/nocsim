#include "operations.h"

nocviz_op* nocviz_op_init(char* script, char* description) {
	nocviz_op* oper;

	oper = noctools_malloc(sizeof(nocviz_op));
	oper->script = noctools_malloc(sizeof(char) * strlen(script));
	strcpy(oper->script, script);

	oper->description = noctools_malloc(sizeof(char) * strlen(description));
	strcpy(oper->description, description);

	return oper;
}

void nocviz_op_free(nocviz_op* oper) {
	free(oper->script);
	free(oper->description);
	free(oper);
}


int nocviz_op_execute(nocviz_op* oper, Tcl_Interp* interp) {
	return Tcl_Eval(interp, oper->script);
}
