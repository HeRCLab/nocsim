#include "commands.h"

int nocviz_subcmd_link_create(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_node* src;
	nocviz_node* dest;
	nocviz_link* link;

	Tcl_RequireArgs(interp, 4, "link create SRC_ID DEST_ID");

	/* we retrieve src and dest to give a more detailed error in case
	 * one of them does not exist */
	src = get_node_from_obj(interp, g, objv[2]);
	dest = get_node_from_obj(interp, g, objv[3]);

	link = nocviz_graph_new_link(g, src->id, dest->id, NOCVIZ_LINK_DIRECTED);

	if (link == NULL) {
		Tcl_SetResultf(interp,
			"Failed to create link from node '%s' to node '%s'",
			src->id, dest->id);
		return TCL_ERROR;
	}

	return TCL_OK;

}

int nocviz_subcmd_link_destroy(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;

	Tcl_RequireArgs(interp, 4, "link destroy ID1 ID2");

	link = get_link_from_objs(interp, g, objv[2], objv[3]);

	nocviz_graph_free_link(g, link);

	return TCL_OK;
}

int nocviz_subcmd_link_undirected(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;

	Tcl_RequireArgs(interp, 4, "link undirected ID1 ID2");

	link = get_link_from_objs(interp, g, objv[2], objv[3]);

	link->type = NOCVIZ_LINK_UNDIRECTED;

	return TCL_OK;
}

int nocviz_subcmd_link_directed(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;

	Tcl_RequireArgs(interp, 4, "link directed ID1 ID2");

	link = get_link_from_objs(interp, g, objv[2], objv[3]);

	link->type = NOCVIZ_LINK_DIRECTED;

	return TCL_OK;

}

int nocviz_subcmd_link_reverse(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;

	Tcl_RequireArgs(interp, 4, "link reverse ID1 ID2");

	link = get_link_from_objs(interp, g, objv[2], objv[3]);

	nocviz_graph_reverse_link(g, link);

	return TCL_OK;

}

int nocviz_subcmd_link_direction(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;
	Tcl_Obj* listPtr;

	Tcl_RequireArgs(interp, 4, "link direction ID1 ID2");

	link = get_link_from_objs(interp, g, objv[2], objv[3]);

	listPtr = Tcl_NewListObj(0, NULL);

	Tcl_ListObjAppendElement(interp, listPtr,
			Tcl_NewStringObj(link->from->id, strlen(link->from->id)));
	Tcl_ListObjAppendElement(interp, listPtr,
			Tcl_NewStringObj(link->to->id, strlen(link->to->id)));

	Tcl_SetObjResult(interp, listPtr);

	return TCL_OK;

}

int nocviz_subcmd_link_find(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;
	nocviz_node* node1 = NULL;
	nocviz_node* node2 = NULL;
	Tcl_Obj* listPtr;

	if ((objc != 3) && (objc != 4)) {
		Tcl_WrongNumArgs(interp, 0, objv, "link find ID / link find ID1 ID2");
		return TCL_ERROR;
	}

	listPtr = Tcl_NewListObj(0, NULL);

	node1 = get_node_from_obj(interp, g, objv[2]);
	if (objc > 3) {
		node2 = get_node_from_obj(interp, g, objv[3]);
	}

	if (objc > 3) {
		/* case where we know both endpoints */
		link = nocviz_graph_get_link(g, node1->id, node2->id);
		if (link != NULL) {
			Tcl_ListObjAppendElement(interp, listPtr,
					Tcl_NewStringObj(node1->id, strlen(node1->id)));
			Tcl_ListObjAppendElement(interp, listPtr,
					Tcl_NewStringObj(node2->id, strlen(node2->id)));
		}

	} else {
		/* case where we know only one endpoint */

		nocviz_graph_node_foreach_link(node1, link,
			Tcl_ListObjAppendElement(interp, listPtr,
				Tcl_NewStringObj(link->from->id, strlen(link->from->id)));
			Tcl_ListObjAppendElement(interp, listPtr,
				Tcl_NewStringObj(link->to->id, strlen(link->to->id)));
		);
	}

	Tcl_SetObjResult(interp, listPtr);

	return TCL_OK;

}

int nocviz_subcmd_link_match(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;
	char* val;
	char* key;
	char* pattern;
	Tcl_Obj* listPtr;

	Tcl_RequireArgs(interp, 4, "link direction KEY PATTERN");

	key = Tcl_GetString(objv[2]);
	pattern = Tcl_GetString(objv[3]);

	listPtr = Tcl_NewListObj(0, NULL);

	nocviz_graph_foreach_link(g, link,
		val = nocviz_ds_get_kvp(link->ds, key);
		if (val == NULL) { continue; }
		if (tcl_regex_matches(interp, pattern, val)) {
			Tcl_ListObjAppendElement(interp, listPtr,
				Tcl_NewStringObj(link->from->id, strlen(link->from->id)));
			Tcl_ListObjAppendElement(interp, listPtr,
				Tcl_NewStringObj(link->to->id, strlen(link->to->id)));
		}
	);

	return TCL_ERROR;
}

int nocviz_subcmd_link_data(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	char* subcmd;

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 0, objv, "link data subcmd ...");
		return TCL_ERROR;
	}

	subcmd = Tcl_GetString(objv[2]);
	
	if (string_equals(subcmd, "set")) {
		return nocviz_subcmd_link_data_set(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "get")) {
		return nocviz_subcmd_link_data_get(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "fmt")) {
		return nocviz_subcmd_link_data_fmt(cdata, interp, objc, objv);
		
	} else if (string_equals(subcmd, "keys")) {
		return nocviz_subcmd_link_data_keys(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "show")) {
		return nocviz_subcmd_link_data_show(cdata, interp, objc, objv);

	} else {
		Tcl_SetResultf(interp, "no such link subcommand: %s", subcmd);
		return TCL_ERROR;
	}

}

int nocviz_subcmd_link_data_set(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;
	char* key;
	char* val;

	Tcl_RequireArgs(interp, 7, "link data set ID1 ID2 KEY VAL");

	link = get_link_from_objs(interp, g, objv[3], objv[4]);

	key = Tcl_GetString(objv[5]);
	val = Tcl_GetString(objv[6]);

	nocviz_ds_set_kvp(link->ds, key, strdup(val));

	return TCL_OK;
}

int nocviz_subcmd_link_data_get(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;
	char* key;
	char* res;

	Tcl_RequireArgs(interp, 6, "link data get ID1 ID2 KEY");

	link = get_link_from_objs(interp, g, objv[3], objv[4]);
	key = Tcl_GetString(objv[5]);

	res = nocviz_ds_get_kvp(link->ds, key);

	if (res == NULL) {
		Tcl_SetResultf(interp, "no such key '%s' for KVP of link '%s' <--> '%s'",
			key, link->from->id, link->to->id);
		return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, Tcl_NewStringObj(res, strlen(res)));

	return TCL_OK;
}

int nocviz_subcmd_link_data_fmt(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* key;
	char* fmt;
	nocviz_link* link;

	Tcl_RequireArgs(interp, 7, "link data fmt ID1 ID2 KEY FMT");

	link = get_link_from_objs(interp, g, objv[3], objv[4]);
	key = Tcl_GetString(objv[5]);
	fmt = Tcl_GetString(objv[6]);

	nocviz_ds_set_fmt(link->ds, key, strdup(fmt));

	return TCL_OK;
}

int nocviz_subcmd_link_data_keys(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	nocviz_link* link;
	const char* key;
	char* val;
	Tcl_Obj* listPtr;

	Tcl_RequireArgs(interp, 5, "node data keys ID1 ID2");

	link = get_link_from_objs(interp, g, objv[3], objv[4]);

	listPtr = Tcl_NewListObj(0, NULL);

	nocviz_ds_foreach_kvp(link->ds, key, val,
		UNUSED(val);
		Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(key, strlen(key)))
	);

	Tcl_SetObjResult(interp, listPtr);

	return TCL_OK;
}

int nocviz_subcmd_link_data_show(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	char* section_name;
	strvec* section;
	nocviz_link* link;

	if (objc < 6) {
		Tcl_WrongNumArgs(interp, 0, objv, "link data show ID1 ID2 SECTION KEY1 ... KEYn");
		return TCL_ERROR;
	}

	link = get_link_from_objs(interp, g, objv[3], objv[4]);
	section_name = Tcl_GetString(objv[5]);

	/* delete the section if it exists */
	if (nocviz_ds_get_section(link->ds, section_name) != NULL) {
		nocviz_ds_del_section(link->ds, section_name);
	}

	/* populate the section */
	section = nocviz_ds_new_section(link->ds, section_name);
	for (int i = 6 ; i < objc ; i++) {
		vec_push(section, strdup(Tcl_GetString(objv[i])));
	}

	return TCL_OK;
}

int nocviz_subcmd_link_op(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 0, objv, "link op subcmd...");
		return TCL_ERROR;
	}

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;
}
