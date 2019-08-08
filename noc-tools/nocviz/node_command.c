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
	nocviz_graph_set_dirty(g, true);

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
	nocviz_graph_set_dirty(g, true);

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
	nocviz_graph_set_dirty(g, true);

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

	} else if (string_equals(subcmd, "keys")) {
		return nocviz_subcmd_node_data_keys(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "show")) {
		return nocviz_subcmd_node_data_show(cdata, interp, objc, objv);

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

int nocviz_subcmd_node_data_keys(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;
	nocviz_node* n;
	const char* key;
	char* val;
	Tcl_Obj* listPtr;

	Tcl_RequireArgs(interp, 4, "node data keys ID");

	id = Tcl_GetString(objv[3]);

	n = nocviz_graph_get_node(g, id);

	if (n == NULL) {
		Tcl_SetResultf(interp, "no such node: %s", id);
		return TCL_ERROR;
	}

	listPtr = Tcl_NewListObj(0, NULL);

	nocviz_ds_foreach_kvp(n->ds, key, val,
		UNUSED(val);
		Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(key, strlen(key)))
	);

	Tcl_SetObjResult(interp, listPtr);

	return TCL_OK;
}

int nocviz_subcmd_node_data_show(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* id;
	char* section_name;
	nocviz_node* n;
	strvec* section;

	if (objc < 5) {
		Tcl_WrongNumArgs(interp, 0, objv, "node data show ID SECTION KEY1 ... KEYn");
		return TCL_ERROR;
	}

	id = Tcl_GetString(objv[3]);
	section_name = Tcl_GetString(objv[4]);

	n = nocviz_graph_get_node(g, id);

	if (n == NULL) {
		Tcl_SetResultf(interp, "no such node: %s", id);
		return TCL_ERROR;
	}

	/* delete the section if it exists */
	if (nocviz_ds_get_section(n->ds, section_name) != NULL) {
		nocviz_ds_del_section(n->ds, section_name);
	}

	/* populate the section */
	section = nocviz_ds_new_section(n->ds, section_name);
	for (int i = 5 ; i < objc ; i++) {
		vec_push(section, strdup(Tcl_GetString(objv[i])));
	}

	return TCL_OK;
}

int nocviz_subcmd_node_list(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_node* node;
	Tcl_Obj* listPtr;

	Tcl_RequireArgs(interp, 2, "node list");

	listPtr = Tcl_NewListObj(0, NULL);

	nocviz_graph_foreach_node(g, node,
		Tcl_ListObjAppendElement(interp, listPtr,
				Tcl_NewStringObj(node->id, strlen(node->id)));
	);

	Tcl_SetObjResult(interp, listPtr);

	return TCL_OK;


}

int nocviz_subcmd_node_match(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_node* node;
	Tcl_Obj* listPtr;
	char* val;
	char* key;
	char* pattern;

	Tcl_RequireArgs(interp, 4, "node match KEY PATTERN");

	key = Tcl_GetString(objv[2]);
	pattern = Tcl_GetString(objv[3]);

	listPtr = Tcl_NewListObj(0, NULL);

	nocviz_graph_foreach_node(g, node,
		val = nocviz_ds_get_kvp(node->ds, key);
		if (val == NULL) {continue;}
		dbprintf("checking match for key=%s val=%s\n", key, val);
		if (tcl_regex_matches(interp, pattern, val)) {
			dbprintf("\tmatched %s\n", pattern);
			Tcl_ListObjAppendElement(interp, listPtr,
				Tcl_NewStringObj(node->id, strlen(node->id)));
		}
	);

	Tcl_SetObjResult(interp, listPtr);

	return TCL_OK;

}

int nocviz_subcmd_node_op(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 0, objv, "node op subcmd...");
		return TCL_ERROR;
	}

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;
}

int nocviz_subcmd_node_color(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_node* n;
	int r, gr, b, a;

	if ((objc != 6) && (objc != 7)) {
		Tcl_WrongNumArgs(interp, 0, objv, "node color ID R G B / node color ID R G B A");
		return TCL_ERROR;
	}

	n = get_node_from_obj(interp, g, objv[2]);
	r = get_int_from_obj(interp, objv[3]);
	gr = get_int_from_obj(interp, objv[4]);
	b = get_int_from_obj(interp, objv[5]);

	a = 255;
	if (objc == 7) {
		a = get_int_from_obj(interp, objv[6]);
	}

	AG_ColorRGBA_8(&n->c, r,gr,b, a);
	nocviz_graph_color_set_dirty(g, true);

	return TCL_OK;
}
