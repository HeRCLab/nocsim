#ifndef NOCVIZ_OPERATIONS_H
#define NOCVIZ_OPERATIONS_H

#include <tcl.h>

/******************************************************************************
 *
 * An operation is a user-defined action (a TCL command) to execute when the
 * user does something within the UI. Generally pressing a button. This is the
 * underlying implementation of operations and their storage format.
 *
 ******************************************************************************/

typedef struct nocviz_op_t {
	char* script;
	char* description;
} nocviz_op;

nocviz_op* nocviz_op_init(char* script, char* description);
init nocviz_op_execute(nocviz_op* oper, Tcl_Interp* interp);
void nocviz_op_free(nocviz_op* oper);

#endif
