#include "commands.h"

/******************************************************************************
 *
 * Implementation of node subcommands.
 *
 *****************************************************************************/

int nocviz_subcmd_node_create(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;

	Tcl_RequireArgs(interp, 3, "node create ID");

	id = Tcl_GetString(objv[2]);

	if (nocviz_graph_new_node(g, id) == NULL) {
		Tcl_SetResultf(interp, "failed to create node with ID: %s", id);
		return TCL_ERROR;
	}

	return TCL_OK;
}

int nocviz_subcmd_node_destroy(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;
	nocviz_node* n;

	Tcl_RequireArgs(interp, 3, "node destroy ID");

	id = Tcl_GetString(objv[2]);

	n = nocviz_graph_get_node(g, id);

	if (n == NULL) {
		Tcl_SetResultf(interp, "no such node: %s", id);
		return TCL_ERROR;
	}

	nocviz_graph_free_node(g, n);

	return TCL_OK;
}

int nocviz_subcmd_node_row(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;
	nocviz_node* n;
	int row;

	Tcl_RequireArgs(interp, 4, "node row ID ROW");

	id = Tcl_GetString(objv[2]);
	Tcl_ObjToInt(interp, objv[3], &row);

	n = nocviz_graph_get_node(g, id);

	if (n == NULL) {
		Tcl_SetResultf(interp, "no such node: %s", id);
		return TCL_ERROR;
	}

	n->row = row;

	return TCL_OK;
}

int nocviz_subcmd_node_col(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;
	nocviz_node* n;
	int col;

	Tcl_RequireArgs(interp, 4, "node col ID COL");

	id = Tcl_GetString(objv[2]);
	Tcl_ObjToInt(interp, objv[3], &col);

	n = nocviz_graph_get_node(g, id);

	if (n == NULL) {
		Tcl_SetResultf(interp, "no such node: %s", id);
		return TCL_ERROR;
	}

	n->col = col;

	return TCL_OK;
}

int nocviz_subcmd_node_title(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;
	nocviz_node* n;
	char* title;

	Tcl_RequireArgs(interp, 4, "node title ID TITLE");

	id = Tcl_GetString(objv[2]);
	title = Tcl_GetString(objv[3]);

	n = nocviz_graph_get_node(g, id);

	if (n == NULL) {
		Tcl_SetResultf(interp, "no such node: %s", id);
		return TCL_ERROR;
	}

	free(n->title);
	n->title = strdup(title);

	return TCL_OK;
}

int nocviz_subcmd_node_data(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	char* subcmd;

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 0, objv, "node data subcmd ...");
		return TCL_ERROR;
	}

	subcmd = Tcl_GetString(objv[2]);
	
	if (string_equals(subcmd, "set")) {
		return nocviz_subcmd_node_data_set(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "get")) {
		return nocviz_subcmd_node_data_get(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "fmt")) {
		return nocviz_subcmd_node_data_fmt(cdata, interp, objc, objv);

	} else {
		Tcl_SetResultf(interp, "no such node subcommand: %s", subcmd);
		return TCL_ERROR;
	}

}

int nocviz_subcmd_node_data_set(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;
	nocviz_node* n;
	char* key;
	char* val;

	Tcl_RequireArgs(interp, 6, "node data set ID KEY VAL");

	id = Tcl_GetString(objv[3]);
	key = Tcl_GetString(objv[4]);
	val = Tcl_GetString(objv[5]);

	n = nocviz_graph_get_node(g, id);

	if (n == NULL) {
		Tcl_SetResultf(interp, "no such node: %s", id);
		return TCL_ERROR;
	}

	nocviz_ds_set_kvp(n->ds, key, strdup(val));

	return TCL_OK;
}

int nocviz_subcmd_node_data_get(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;
	nocviz_node* n;
	char* key;
	char* res;

	Tcl_RequireArgs(interp, 5, "node data get ID KEY");

	id = Tcl_GetString(objv[3]);
	key = Tcl_GetString(objv[4]);

	n = nocviz_graph_get_node(g, id);

	if (n == NULL) {
		Tcl_SetResultf(interp, "no such node: %s", id);
		return TCL_ERROR;
	}

	res = nocviz_ds_get_kvp(n->ds, key);

	if (res == NULL) {
		Tcl_SetResultf(interp, "no such key '%s' for KVP of node '%s'", key, id);
		return TCL_ERROR;
	}

	dbprintf("KVP of key %s at node %s is %s\n", key, id, res);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(res, strlen(res)));

	return TCL_OK;
}

int nocviz_subcmd_node_data_fmt(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;
	nocviz_node* n;
	char* key;
	char* res;
	char* fmt;

	Tcl_RequireArgs(interp, 6, "node data fmt ID KEY FMT");

	id = Tcl_GetString(objv[3]);
	key = Tcl_GetString(objv[4]);
	fmt = Tcl_GetString(objv[5]);

	n = nocviz_graph_get_node(g, id);

	if (n == NULL) {
		Tcl_SetResultf(interp, "no such node: %s", id);
		return TCL_ERROR;
	}

	nocviz_ds_set_fmt(n->ds, key, strdup(fmt));

	return TCL_OK;
}
