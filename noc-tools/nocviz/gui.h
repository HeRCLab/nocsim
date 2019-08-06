#ifndef NOCVIZ_GUI_H
#define NOCVIZ_GUI_H

#include <agar/core.h>
#include <agar/gui.h>

#include <tcl.h>

#include <stdbool.h>

#include "datastore.h"
#include "graph.h"

/******************************************************************************
 *
 * Implementation of the nocviz GUI.
 *
 *****************************************************************************/

/* singleton for GUI parameters */
typedef struct nocviz_gui_params_t {
	/* main TCL interpreter where the nocviz namespace was initialized */
	Tcl_Interp* interp;

	/* main graph structure */
	nocviz_graph* graph;

} nocviz_gui_params;

typedef struct nocviz_gui_handle_t {
	nocviz_gui_params* params;
	AG_Thread thread;
	bool initialized;
	nocviz_graph* graph;
} nocviz_gui_handle;

/* GUI initializer, calls gui_main */
void gui_init(nocviz_gui_handle* h, Tcl_Interp* interp, nocviz_graph* graph);

/* GUI main thread */
void* gui_main(void* arg);

unsigned int graph_update_handler(AG_Timer* to, AG_Event* event);

void graph_update(AG_Driver* dri, nocviz_graph* g_data);

/* Helper macro for use within event handlers, traverses up the VFS tree to
 * grab the top-level driver object. This is needed since AG_ObjectRoot()
 * dosen't always traverse all the way up the tree in one go */
#define get_dri() (AG_Driver*) AG_ObjectFindParent(AG_SELF(), "agDrivers", NULL)


#endif
