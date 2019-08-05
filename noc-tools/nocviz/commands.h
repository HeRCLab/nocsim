#ifndef NOCVIZ_COMMANDS_H
#define NOCVIZ_COMMANDS_H

#include "../3rdparty/khash.h"
#include "../3rdparty/vec.h"
#include "operations.h"
#include "datastore.h"
#include "../common/util.h"
#include "graph.h"

#include <tcl.h>
#include <stdio.h>

/******************************************************************************
 *
 * Implementation of TCL API to nocviz data structures.
 *
 *****************************************************************************/

int nocviz_command_node(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_create(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_destroy(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_row(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_col(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_title(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_data(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_data_set(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_data_get(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);

#endif
