#include "gui.h"

/* update the graph widget to be consistent with the underlying nocviz graph
 * state */
unsigned int graph_update_handler(AG_Timer* to, AG_Event* event) {
	UNUSED(to);
	AG_Driver* dri = get_dri();
	AG_Window* win = AG_GetPointer(dri, "main_window");
	AG_Timer* new_to;

	nocviz_gui_params* p = AG_PTR_NAMED("gui_param");
	nocviz_graph* g_data = p->graph;

	/* require and update */
	if (nocviz_graph_is_dirty(g_data)) {
		graph_update(dri, g_data);
	}

	/* instantiate a new timer and start it running so it will call us
	 * later. */
	new_to = noctools_malloc(sizeof(AG_Timer));
	AG_InitTimer(new_to, "graph_update_event", AG_TIMER_AUTO_FREE);
	AG_AddTimer(win, new_to, 100, graph_update_handler, "%p(gui_param)", p);

	return 0;
}

void handle_vertex_selection(AG_Event* event) {
	AG_GraphVertex* vtx = AG_PTR(1);
	AG_Driver* dri = get_dri();
	AG_SetPointer(dri, "selected_node", vtx->userPtr);
	AG_Box* infobox;
	AG_Box* sectionbox;
	strvec* section;
	const char* sectionname;
	AG_Object* parent;
	nocviz_node* n = vtx->userPtr;
	char* key;
	unsigned int i;

	/* delete the existing info box and create a new one */
	infobox = AG_GetPointer(dri, "infobox_p");
	parent = AG_ObjectParent(AGOBJECT(infobox));
	AG_ObjectDelete(infobox);
	infobox = AG_BoxNew(parent, AG_BOX_VERT, AG_BOX_EXPAND);
	AG_SetPointer(dri, "infobox_p", infobox);

#ifdef EBUG
	sectionbox = AG_BoxNew(infobox, AG_BOX_VERT, AG_BOX_HFILL | AG_BOX_FRAME);
	AG_BoxSetLabelS(sectionbox, "DEBUG DEBUG DEBUG");
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "parent=%p", (void*) parent);
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "infobox_p=%p", (void*) infobox);
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "selected_node=%p", (void*) n);
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "node title=%s", n->title);
#endif

	nocviz_ds_foreach_section(n->ds, sectionname, section,
		sectionbox = AG_BoxNew(infobox, AG_BOX_VERT, AG_BOX_HFILL | AG_BOX_FRAME);
		AG_BoxSetLabel(sectionbox, "sectionname=%s", sectionname);
		vec_foreach(section, key, i) {
			AG_LabelNew(sectionbox, AG_LABEL_HFILL, "i=%u key=%s val=%s", 
					i, key, nocviz_ds_format(n->ds, key));
		}
	);

	AG_WidgetHide(infobox);
	AG_WidgetShow(infobox);

}

void graph_update(AG_Driver* dri, nocviz_graph* g_data) {
	AG_Graph* g_wid = AG_GetPointer(dri, "graph_p");
	AG_Object* g_parent;
	AG_GraphVertex* vtx1;
	nocviz_node* n;
	nocviz_link* l;
	nocviz_node* selected = AG_GetPointer(dri, "selected_node");
	int xOffs;
	int yOffs;

	dbprintf("dirty graph! performing update... \n");

	/* mark as clean */
	nocviz_graph_set_dirty(g_data, false);

	xOffs = g_wid->xOffs;
	yOffs = g_wid->yOffs;
	g_parent = AG_ObjectParent(AGOBJECT(g_wid));
	AG_ObjectDelete(g_wid);

	g_wid = AG_GraphNew(g_parent, AG_GRAPH_EXPAND);
	AG_SetPointer(dri, "graph_p", g_wid);
	AG_GraphSizeHint(g_wid, 800, 600);

	g_wid->xOffs = xOffs;
	g_wid->yOffs = yOffs;

	/* create all nodes */
	nocviz_graph_foreach_node(g_data, n,
		vtx1 = AG_GraphVertexNew(g_wid, n);
		AG_GraphVertexLabel(vtx1, "%s", n->title);
		AG_GraphVertexPosition(vtx1, n->row * 50, n->col * 50);
	);

/* #define nocviz_graph_foreach_link(g, linkvar, code) do { \ */

	/* create all links */
	nocviz_graph_foreach_link(g_data, l,
		if (l->type == NOCVIZ_LINK_UNDIRECTED) {
			AG_GraphEdgeNew(g_wid,
					AG_GraphVertexFind(g_wid, l->from),
					AG_GraphVertexFind(g_wid, l->to),
					NULL);
		} else {
			AG_DirectedGraphEdgeNew(g_wid,
					AG_GraphVertexFind(g_wid, l->from),
					AG_GraphVertexFind(g_wid, l->to),
					NULL);
		}
	);

	if (selected != NULL) {
		vtx1 = AG_GraphVertexFind(g_wid, selected);
		if (vtx1 != NULL) {
			vtx1->flags = AG_GRAPH_SELECTED;
		}
	}

	AG_AddEvent(g_wid, "graph-vertex-selected", handle_vertex_selection, NULL);

	AG_WidgetHide(g_wid);
	AG_WidgetShow(g_wid);


}
