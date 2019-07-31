#ifndef NOCVIZ_OPERATIONS_H
#define NOCVIZ_OPERATIONS_H

#include <string.h>
#include <tcl.h>

#include "../common/util.h"

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

/* Copies the given strings, since op_free will free them. Caller must free the
 * copies on it's side, if any. */
nocviz_op* nocviz_op_init(char* script, char* description);
void nocviz_op_free(nocviz_op* oper);
int nocviz_op_execute(nocviz_op* oper, Tcl_Interp* interp);

#endif
