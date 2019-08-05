#include "gui.h"

/* update the graph widget to be consistent with the underlying nocviz graph
 * state */
void update_graph_widget(AG_Event* event) {
	AG_Driver* dri = get_dri();
	AG_Window* win = AG_GetPointer(dri, "main_window");
	AG_Graph* g = AG_GetPointer(dri, "graph_p");

	printf("graph update event!!!\n");
	AG_SchedEvent(win, g, 100, "graph_update_event", "%p,%s", g, "graph_update_event");
}
