#ifndef NOCVIZ_GUI_H
#define NOCVIZ_GUI_H

#include <agar/core.h>
#include <agar/gui.h>

#include <tcl.h>

#include <stdbool.h>

#include "datastore.h"
#include "graph.h"
#include "text_widget.h"

/******************************************************************************
 *
 * Implementation of the nocviz GUI.
 *
 * Global variables (attached to top level AG_Driver):
 *
 * graph_p  . . . . . . Pointer to AG_Graph widget.
 *
 * show_node_labels . . Pointer to int (1 to show labels, 0 to hide).
 *
 * show_edge_labels . . Pointer to int (1 to show labels, 0 to hide).
 *
 * selected_node  . . . Currently selected nocviz_node*. May not always be,
 *                      valid; this can be checked by using AG_VertexFind. If
 *                      no vertex is found in graph_p, this node has been
 *                      deleted and selected_node should be cleared to NULL.
 *
 * selected_link  . . . Currently selected nocviz_link*. May not always be,
 *                      valid; this can be checked by using AG_GraphEdgeFind.
 *                      If no edge is found in graph_p, this node has been
 *                      deleted and selected_link should be cleared to NULL.
 *
 * infobox_p  . . . . . Pointer to the top-level AG_Box of the info pane.
 *
 *
 * The main graph widget updates in the background every
 * NOCVIZ_GUI_GRAPH_UPDATE_INTERVAL many (Agar) ticks. At such times, if the
 * nocviz_graph* has it's dirty flag set, then graph_update() deletes and
 * re-generates the graph widget. This can also happens when a vertex is
 * selected, and when toggling edge/node labels.
 *
 * The "info panel" contains all information about the currently selected graph
 * node, edge, or the simulation (if no node or edge is selected). This updates
 * only when changing what is selected. This panel uses the custom
 * NV_TextWidget type which automatically polls the information in each section
 * of the corresponding datastore object.
 *
 * XXX: only node info works right now, not simulation wide or link.
 *
 *****************************************************************************/

/*** CONFIGURATION ***********************************************************/

#define NOCVIZ_GUI_GRAPH_DEFAULT_WIDTH 800
#define NOCVIZ_GUI_GRAPH_DEFAULT_HEIGHT 600
#define NOCVIZ_GUI_GRAPH_UPDATE_INTERVAL 50

/*** TYPES *******************************************************************/

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

/*** FUNCTIONS ***************************************************************/

/* GUI initializer, calls gui_main */
void gui_init(nocviz_gui_handle* h, Tcl_Interp* interp, nocviz_graph* graph);

/* GUI main thread */
void* gui_main(void* arg);

unsigned int graph_update_handler(AG_Timer* to, AG_Event* event);

void graph_update(AG_Driver* dri, nocviz_graph* g_data);

size_t PrintFmtHandle(AG_FmtString* fs, char* dst, size_t dstSize);

void handle_vertex_selection(AG_Event* event);

void handle_link_selection(AG_Event* event);

/*** UTILITIES ***************************************************************/

/* Helper macro for use within event handlers, traverses up the VFS tree to
 * grab the top-level driver object. This is needed since AG_ObjectRoot()
 * dosen't always traverse all the way up the tree in one go */
#define get_dri() (AG_Driver*) AG_ObjectFindParent(AG_SELF(), "agDrivers", NULL)


#endif
