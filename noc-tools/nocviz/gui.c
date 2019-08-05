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

void* gui_main(void* arg) {
	nocviz_gui_params* p = arg;

	AG_Window *win;
	if (AG_InitCore(NULL, 0) == -1 ||
			AG_InitGraphics(0) == -1)
		return NULL;
	win = AG_WindowNew(AG_WINDOW_MAIN);
	AG_LabelNew(win, 0, "Hello, world!");
	AG_WindowShow(win);
	AG_EventLoop();

	return NULL;
}
