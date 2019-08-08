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
#include <stdbool.h>

/******************************************************************************
 *
 * Implementation of TCL API to nocviz data structures.
 *
 *****************************************************************************/

/*** FUNCTIONS ***************************************************************/

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
int nocviz_subcmd_link_data_set(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_data_get(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_data_fmt(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_data_keys(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_data_show(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_op(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
int nocviz_subcmd_link_title(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);

int nocviz_command_launch_gui(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);

/*** UTILITIES ***************************************************************/

/* retrieve a nocviz_node* by an ID specified in a TCL string object, or return
 * TCL_ERROR */
#define get_node_from_obj(__interp, __graph, __obj) __extension__ ({ \
		nocviz_node* __node; \
		char* __id = Tcl_GetString(__obj); \
		__node = nocviz_graph_get_node(__graph, __id); \
		if (__node == NULL) { \
			Tcl_SetResultf(__interp, "no such node '%s'", __id); \
			return TCL_ERROR; \
		} \
		__node;\
	});

/* get a nocviz_link* by it's endpoint IDs specified in a pair of TCL string
 * objects, or return TCL_ERROR */
#define get_link_from_objs(__interp, __graph, __obj1, __obj2) __extension__ ({ \
		nocviz_node* __node1 = get_node_from_obj(__interp, __graph, __obj1); \
		nocviz_node* __node2 = get_node_from_obj(__interp, __graph, __obj2); \
		nocviz_link* __link = nocviz_graph_get_link(__graph, \
				__node1->id, __node2->id); \
		if (__link == NULL) { \
			Tcl_SetResultf(__interp, "could not find link '%s' <--> '%s'", \
				__node1->id, __node2->id); \
			return TCL_ERROR; \
		} \
		__link;\
	});

/* either evaluate to true/false if the regex matches or does not, or throw
 * a TCL error if one occurs */
#define tcl_regex_matches(__interp, __pattern, __str) __extension__ ({ \
		Tcl_Obj* __obj_res; \
		int __int_res; \
		int __status; \
		char* __str_res; \
		bool __res; \
		__status = Tcl_Evalf(__interp, "regexp {%s} {%s}", __pattern, __str); \
		__obj_res = Tcl_GetObjResult(__interp); \
		if (__status != TCL_OK) { \
			__str_res = Tcl_GetString(__obj_res); \
			Tcl_SetResultf(__interp, "error 001 processing regex: %s", __str_res); \
			return TCL_ERROR; \
		} \
		__status = Tcl_GetIntFromObj(__interp, __obj_res, &__int_res); \
		if (__status != TCL_OK) { \
			__obj_res = Tcl_GetObjResult(__interp); \
			__str_res = Tcl_GetString(__obj_res); \
			Tcl_SetResultf(__interp, "error 002 processing regex: %s", __str_res); \
			return TCL_ERROR; \
		} \
		__res = (__int_res == 1); \
		__res; \
	})


#endif
