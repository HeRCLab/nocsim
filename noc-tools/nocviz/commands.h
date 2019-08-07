#ifndef NOCVIZ_COMMANDS_H
#define NOCVIZ_COMMANDS_H

#include "../3rdparty/khash.h"
#include "../3rdparty/vec.h"
#include "operations.h"
#include "datastore.h"
#include "../common/util.h"
#include "graph.h"
#include "gui.h"

#include <tcl.h>
#include <stdio.h>

/******************************************************************************
 *
 * Implementation of TCL API to nocviz data structures.
 *
 *****************************************************************************/

/* node commands */
int nocviz_command_node(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_create(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_destroy(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_row(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_col(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_title(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_data(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_data_set(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_data_get(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_data_fmt(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_data_keys(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_data_show(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_list(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_match(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_node_op(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);

/* link commands */
int nocviz_command_link(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_create(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_destroy(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_undirected(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_directed(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_direction(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_reverse(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_find(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_match(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_data(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_op(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);

int nocviz_command_launch_gui(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);

#endif
