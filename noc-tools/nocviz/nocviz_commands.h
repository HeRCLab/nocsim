#ifndef NOCVIZ_COMMANDS_H
#define NOCVIZ_COMMANDS_H

#include "../3rdparty/khash.h"
#include "../3rdparty/vec.h"
#include "operations.h"
#include "datastore.h"
#include "../common/util.h"

#include <tcl.h>
#include <stdio.h>

/******************************************************************************
 *
 * Implementation of TCL API to nocviz data structures.
 *
 *****************************************************************************/

static int nocviz_command_node(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);

#endif
