#include "commands.h"

/* https://wiki.tcl-lang.org/page/Tcl+Handles */

int nocviz_command_node(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	char* subcmd;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 0, objv, "node subcmd ...");
		return TCL_ERROR;
	}

	subcmd = Tcl_GetString(objv[1]);

	if (string_equals(subcmd, "create")) {
		return nocviz_subcmd_node_create(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "destroy")) {
		return nocviz_subcmd_node_destroy(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "row")) {
		return nocviz_subcmd_node_row(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "col")) {
		return nocviz_subcmd_node_col(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "title")) {
		return nocviz_subcmd_node_title(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "data")) {
		return nocviz_subcmd_node_data(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "list")) {
		return nocviz_subcmd_node_list(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "match")) {
		return nocviz_subcmd_node_match(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "op")) {
		return nocviz_subcmd_node_op(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "color")) {
		return nocviz_subcmd_node_color(cdata, interp, objc, objv);

	} else {
		Tcl_SetResultf(interp, "no such subcommand: %s", subcmd);
		return TCL_ERROR;
	}
}

int nocviz_command_link(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	char* subcmd;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 0, objv, "link subcmd ...");
		return TCL_ERROR;
	}

	subcmd = Tcl_GetString(objv[1]);

	if (string_equals(subcmd, "create")) {
		return nocviz_subcmd_link_create(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "destroy")) {
		return nocviz_subcmd_link_destroy(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "undirected")) {
		return nocviz_subcmd_link_undirected(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "directed")) {
		return nocviz_subcmd_link_directed(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "direction")) {
		return nocviz_subcmd_link_direction(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "reverse")) {
		return nocviz_subcmd_link_reverse(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "find")) {
		return nocviz_subcmd_link_find(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "match")) {
		return nocviz_subcmd_link_match(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "data")) {
		return nocviz_subcmd_link_data(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "title")) {
		return nocviz_subcmd_link_title(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "color")) {
		return nocviz_subcmd_link_color(cdata, interp, objc, objv);

	} else {
		Tcl_SetResultf(interp, "no such subcommand: %s", subcmd);
		return TCL_ERROR;
	}
}

int nocviz_command_launch_gui(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_gui_handle* h = cdata;

	Tcl_RequireArgs(interp, 1, "launch_gui");

	gui_init(h, interp, h->graph);

	return TCL_OK;

}

int nocviz_command_op(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	UNUSED(cdata);
	char* subcmd;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 0, objv, "op subcmd ...");
		return TCL_ERROR;
	}

	subcmd = Tcl_GetString(objv[1]);

	if (string_equals(subcmd, "register")) {
		return nocviz_subcmd_op_register(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "unregister")) {
		return nocviz_subcmd_op_unregister(cdata, interp, objc, objv);

	} else {
		Tcl_SetResultf(interp, "no such subcommand: %s", subcmd);
		return TCL_ERROR;
	}

}

int nocviz_subcmd_op_register(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* opid;
	char* script;
	char* descr;
	nocviz_op* op;

	Tcl_RequireArgs(interp, 5, "op register OPID SCRIPT DESCRIPTION");

	opid = Tcl_GetString(objv[2]);
	script = Tcl_GetString(objv[3]);
	descr = Tcl_GetString(objv[4]);

	op = nocviz_op_init(script, descr);

	nocviz_ds_set_op(g->ds, opid, op);

	return TCL_OK;
}

int nocviz_subcmd_op_unregister(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* opid;

	nocviz_op* op;

	Tcl_RequireArgs(interp, 3, "op unregister OPID");

	opid = Tcl_GetString(objv[2]);

	op = nocviz_ds_del_op(g->ds, opid);

	if (op != NULL) {
		nocviz_op_free(op);
	}

	return TCL_OK;

}

int nocviz_command_data(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	char* subcmd;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 0, objv, "data subcmd ...");
		return TCL_ERROR;
	}

	subcmd = Tcl_GetString(objv[1]);
	
	if (string_equals(subcmd, "set")) {
		return nocviz_subcmd_data_set(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "get")) {
		return nocviz_subcmd_data_get(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "fmt")) {
		return nocviz_subcmd_data_fmt(cdata, interp, objc, objv);
		
	} else if (string_equals(subcmd, "keys")) {
		return nocviz_subcmd_data_keys(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "show")) {
		return nocviz_subcmd_data_show(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "delete")) {
		return nocviz_subcmd_data_delete(cdata, interp, objc, objv);

	} else {
		Tcl_SetResultf(interp, "no such subcommand: %s", subcmd);
		return TCL_ERROR;
	}

}

int nocviz_subcmd_data_set(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* key;
	char* val;

	Tcl_RequireArgs(interp, 4, "data set KEY VAL");

	key = Tcl_GetString(objv[2]);
	val = Tcl_GetString(objv[3]);

	nocviz_ds_set_kvp(g->ds, key, strdup(val));

	return TCL_OK;
}

int nocviz_subcmd_data_get(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* key;
	char* res;

	Tcl_RequireArgs(interp, 3, "data get KEY");

	key = Tcl_GetString(objv[2]);

	res = nocviz_ds_get_kvp(g->ds, key);

	if (res == NULL) {
		Tcl_SetResultf(interp, "no such key '%s'", key);
		return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, Tcl_NewStringObj(res, strlen(res)));

	return TCL_OK;
}

int nocviz_subcmd_data_fmt(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* key;
	char* fmt;

	Tcl_RequireArgs(interp, 4, "data fmt KEY FMT");

	key = Tcl_GetString(objv[2]);
	fmt = Tcl_GetString(objv[3]);

	nocviz_ds_set_fmt(g->ds, key, strdup(fmt));

	return TCL_OK;
}

int nocviz_subcmd_data_keys(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	const char* key;
	char* val;
	Tcl_Obj* listPtr;

	Tcl_RequireArgs(interp, 2, "data keys");

	listPtr = Tcl_NewListObj(0, NULL);

	nocviz_ds_foreach_kvp(g->ds, key, val,
		UNUSED(val);
		Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(key, strlen(key)))
	);

	Tcl_SetObjResult(interp, listPtr);

	return TCL_OK;
}

int nocviz_subcmd_data_show(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* section_name;
	strvec* section;

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 0, objv, "data show SECTION KEY1 ... KEYn");
		return TCL_ERROR;
	}

	section_name = Tcl_GetString(objv[2]);

	/* delete the section if it exists */
	if (nocviz_ds_get_section(g->ds, section_name) != NULL) {
		nocviz_ds_del_section(g->ds, section_name);
	}

	/* populate the section */
	section = nocviz_ds_new_section(g->ds, section_name);
	for (int i = 3 ; i < objc ; i++) {
		vec_push(section, strdup(Tcl_GetString(objv[i])));
	}

	return TCL_OK;
}


int nocviz_subcmd_data_delete(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* key;
	char* val;

	Tcl_RequireArgs(interp, 3, "data delete KEY");
	key = Tcl_GetString(objv[2]);

	val = nocviz_ds_del_kvp(g->ds, key);

	if (val != NULL) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(val, strlen(val)));
	}

	free(val);

	return TCL_OK;
}
