#include "gui.h"

void gui_init(nocviz_gui_handle* h, Tcl_Interp* interp, nocviz_graph* graph) {
	nocviz_gui_params* p = noctools_malloc(sizeof(nocviz_gui_params));
	AG_Thread th;

	p->interp = interp;
	p->graph = graph;

	AG_ThreadCreate(&th, gui_main, p);

	h->thread = th;
	h->params = p;
}

void QuitApplication(AG_Event *event) {
	UNUSED(event);
	AG_DestroyGraphics();
	AG_Destroy();
	AG_Quit();
	AG_ThreadExit(NULL);
}

/* If the debugger is enabled, the "Launch Debugger" menu item will use this
 * to display the LibAgar debugging window */
#ifdef ENABLE_DEBUGGER
void LaunchDebugger(AG_Event *event) {
	AG_Driver* dri;
	AG_Window* db_window;
	AG_Window* win;

	/* Fetch the top-level driver object, which is also the parent of the
	 * main window. We will attach the debugger window to this object. */
	dri = (AG_Driver*) get_dri();

	win = AG_GetPointer(dri, "main_window");

	db_window = AG_GuiDebugger(win);
	AG_WindowShow(db_window);
}
#endif

/* void ToggleEdgeLabels(AG_Event* event) { */
/*         AG_Driver* dri = get_dri(); */
/*         nocsim_state* state = AG_PTR(1); */
/*         AG_Box* box = AG_PTR(2); */
/*  */
/*         int* show_edge_labels; */
/*         show_edge_labels = AG_GetPointer(dri, "show_edge_labels"); */
/*  */
/*         if (*show_edge_labels == 0) { */
/*                 *show_edge_labels = 1; */
/*         } else { */
/*                 *show_edge_labels = 0; */
/*         } */
/*  */
/*         graph_update(state, dri, box); */
/*  */
/* } */
/*  */
/* void ToggleNodeLabels(AG_Event* event) { */
/*         AG_Driver* dri = get_dri(); */
/*         nocsim_state* state = AG_PTR(1); */
/*         AG_Box* box = AG_PTR(2); */
/*  */
/*         int* show_node_labels; */
/*         show_node_labels = AG_GetPointer(dri, "show_node_labels"); */
/*  */
/*         if (*show_node_labels == 0) { */
/*                 *show_node_labels = 1; */
/*         } else { */
/*                 *show_node_labels = 0; */
/*         } */
/*  */
/*         graph_update(state, dri, box); */
/*  */
/* } */


void* gui_main(void* arg) {
	nocviz_gui_params* p = arg;

	AG_Window *win;
	AG_Graph* g;
	AG_Driver* dri;
	AG_MenuItem* temp;
	AG_Menu* menu;
	AG_Console* cons;
	AG_Button *btn;
	AG_Textbox *tb;
	AG_Box* box;
	AG_Pane* inner_pane;
	AG_Pane* infopane;

	int show_node_labels = 1;
	int show_edge_labels = 1;

	/* Initialize LibAgar */
	if (AG_InitCore(NULL, 0) == -1 ||
			AG_InitGraphics(0) == -1)
		return NULL;
	win = AG_WindowNew(0);
	AG_WindowSetCaptionS (win, "nocviz-gui");
	AG_WindowSetGeometry(win, -1, -1, 1100, 900);

	/* setup the state handler and edge creation vertex variables */
	dri = (AG_Driver*) AG_ObjectRoot(win);

	AG_SetPointer(dri, "main_window", win);

	menu = AG_MenuNew(win, 0);

	inner_pane = AG_PaneNewHoriz(win, AG_PANE_DIV1FILL | AG_PANE_EXPAND);

	/* instantiate the graph */
	g = AG_GraphNew(inner_pane->div[1], AG_GRAPH_EXPAND);
	AG_GraphSizeHint(g, 800, 600);
	AG_SetPointer(dri, "graph_p", g);

	/* global flags for edge/node labels */
	AG_SetPointer(dri, "show_node_labels", &show_node_labels);
	AG_SetPointer(dri, "show_edge_labels", &show_edge_labels);

	/* instantiate the "File" menu dropdown */
	AG_MenuItem* menu_file = AG_MenuNode(menu->root, "File", NULL);

	/* instantiate the contents of the File menu */
	{
		AG_MenuAction(menu_file, "Quit", NULL, QuitApplication, NULL);

#ifdef ENABLE_DEBUGGER
		AG_MenuSeparator(menu_file);
		AG_MenuAction(menu_file, "Launch Debugger", NULL, LaunchDebugger, NULL);
#endif
	}

	/* instantiate the "View" menu dropdown */
	AG_MenuItem* menu_view = AG_MenuNode(menu->root, "View", NULL);

	{
		/* AG_MenuAction(menu_view, "Toggle Edge Labels", NULL, ToggleEdgeLabels, "%p,%p", state, inner_pane->div[1]); */
		/* AG_MenuAction(menu_view, "Toggle Node Labels", NULL, ToggleNodeLabels, "%p,%p", state, inner_pane->div[1]); */
	}

	/* split up node info view and simulation info view */
	infopane = AG_PaneNewVert(inner_pane->div[0], AG_PANE_EXPAND);
	AG_PaneMoveDividerPct(infopane, 50);

	/* info view area */
	box = AG_BoxNew(
			AG_ScrollviewNew(infopane->div[0], AG_SCROLLVIEW_BY_MOUSE | AG_SCROLLVIEW_EXPAND),
			AG_BOX_VERT, AG_BOX_EXPAND);
	AG_SetPointer(dri, "infobox_p", box);

	/* this function will populate the info box */
	/* AG_AddEvent(g, "graph-vertex-selected", HandleVertexSelection, NULL); */
	/* AG_AddEvent(g, "graph-edge-selected", HandleEdgeSelection, "%p(state)", state); */

	/* simulation info view */
	box = AG_BoxNew(
			AG_ScrollviewNew(infopane->div[1], AG_SCROLLVIEW_BY_MOUSE | AG_SCROLLVIEW_EXPAND),
			AG_BOX_VERT, AG_BOX_EXPAND);
	AG_SetPointer(dri, "siminfo_p", box);
	/* simulation_update(state, dri); */

	AG_WindowShow(win);

	/* AG_AddEvent(&g->wid.obj, "graph_update_event", update_graph_widget, "%p(gui_param)", p); */
	/* AG_SchedEvent(&g->wid.obj, &g->wid.obj, 1000, "graph_update_event", "%p(gui_param)", p); */
	/* AG_SchedEvent(&g->wid.obj, &g->wid.obj, 100, "graph_update_event", ""); */
	AG_AddEvent(g, "graph_update_event", update_graph_widget, NULL);
	AG_SchedEvent(win, g, 100, "graph_update_event", "%p,%s", g, "graph_update_event");

	AG_EventLoop();

	return NULL;
}
