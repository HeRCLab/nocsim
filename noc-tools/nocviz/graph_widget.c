#include "gui.h"

/* update the graph widget to be consistent with the underlying nocviz graph
 * state */
unsigned int graph_update_handler(AG_Timer* to, AG_Event* event) {
	UNUSED(to);
	AG_Driver* dri = get_dri();
	AG_Window* win = AG_GetPointer(dri, "main_window");
	AG_Graph* g = AG_GetPointer(dri, "graph_p");
	AG_Timer* new_to;

	nocviz_gui_params* p = AG_PTR_NAMED("gui_param");
	nocviz_graph* g_data = p->graph;

	/* require and update */
	if (nocviz_graph_is_dirty(g_data)) {
		graph_update(dri, g_data);
	}

	/* instantiate a new timer and start it running so it will call us
	 * later. */
	new_to = noctools_malloc(sizeof(AG_Timer)); AG_InitTimer(new_to,
			"graph_update_event", AG_TIMER_AUTO_FREE);
	AG_AddTimer(win, new_to, 100, graph_update_handler, "%p(gui_param)", p);

	return 0;
}

void graph_update(AG_Driver* dri, nocviz_graph* g_data) {
	AG_Window* win = AG_GetPointer(dri, "main_window");
	AG_Graph* g_wid = AG_GetPointer(dri, "graph_p");
	AG_Object* g_parent;
	AG_GraphVertex* vtx1;
	nocviz_node* n;
	int xOffs;
	int yOffs;

	dbprintf("dirty graph! performing update... \n");

	/* mark as clean */
	nocviz_graph_set_dirty(g_data, false);

	xOffs = g_wid->xOffs;
	yOffs = g_wid->yOffs;
	g_parent = AG_ObjectParent(g_wid);
	AG_ObjectDelete(g_wid);

	g_wid = AG_GraphNew(g_parent, AG_GRAPH_EXPAND);
	AG_SetPointer(dri, "graph_p", g_wid);
	AG_GraphSizeHint(g_wid, 800, 600);

	g_wid->xOffs = xOffs;
	g_wid->yOffs = yOffs;

	nocviz_graph_foreach_node(g_data, n,
		vtx1 = AG_GraphVertexNew(g_wid, n);
		AG_GraphVertexLabel(vtx1, "%s", n->title);
		AG_GraphVertexPosition(vtx1, n->row * 50, n->col * 50);
	);

	AG_WidgetHide(g_wid);
	AG_WidgetShow(g_wid);


}
