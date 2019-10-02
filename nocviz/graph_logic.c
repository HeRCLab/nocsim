#include "gui.h"

/* handle the user selecting a vertex, remember that the userPtr field of
 * the vertices is a pointer to the corresponding nocviz_node.
 *
 * XXX: it is possible that if a node is deleted while this function is
 * executing, it may attempt to read from unallocated memory.
 *
 * */
void handle_vertex_selection(AG_Event* event) {
	/* AddEvent appends automatic arguments to the end */
	nocviz_node* vtx = AG_PTR(2);
	nocviz_graph* g_data = AG_PTR_NAMED("nocviz_graph");
	AG_Driver* dri = get_dri();
	AG_Box* infobox;
	AG_Box* sectionbox;
	strvec* section;
	const char* sectionname;
	AG_Object* parent;
	char* key;
	unsigned int i;



	/* vtx->userPtr should be safe now */
	AG_SetPointer(dri, "selected_node", vtx);
	/* invalidate */
	AG_SetPointer(dri, "selected_link", NULL);

	/* delete the existing info box and create a new one */
	infobox = AG_GetPointer(dri, "infobox_p");
	parent = AG_ObjectParent(AGOBJECT(infobox));
	AG_ObjectDelete(infobox);
	infobox = AG_BoxNew(parent, AG_BOX_VERT, AG_BOX_EXPAND);
	AG_SetPointer(dri, "infobox_p", infobox);

	AG_SetStyle(infobox, "color", "rgb(196,196,196)");

#ifdef EBUG
	sectionbox = AG_BoxNew(infobox, AG_BOX_VERT, AG_BOX_HFILL | AG_BOX_FRAME);
	AG_BoxSetLabelS(sectionbox, "DEBUG DEBUG DEBUG");
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "parent=%p", (void*) parent);
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "infobox_p=%p", (void*) infobox);
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "selected_node=%p", (void*) vtx);
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "node title=%s", vtx->title);
#endif

	/* generate the info panel contents */
	nocviz_ds_foreach_section(vtx->ds, sectionname, section,
		sectionbox = AG_BoxNew(infobox, AG_BOX_VERT, AG_BOX_HFILL | AG_BOX_FRAME);
		AG_SetStyle(sectionbox, "color", "rgb(196,196,196)");
		AG_BoxSetLabel(sectionbox, "%s", sectionname);
		vec_foreach(section, key, i) {
			/* NV_TextWidget comes from text_widget.{c,h}, and
			 * will automatically keep polling the given node ID
			 * on it's own */
			NV_TextWidgetNew(sectionbox, key, g_data, vtx->id, key);
		}
	);

	/* workaround to force the widget to redraw immediately */
	AG_WidgetHide(infobox);
	AG_WidgetShow(infobox);

}

void handle_link_selection(AG_Event* event) {
	/* AddEvent appends automatic arguments to the end */
	nocviz_link* edge = AG_PTR(2);
	nocviz_graph* g_data = AG_PTR_NAMED("nocviz_graph");
	AG_Driver* dri = get_dri();
	AG_Box* infobox;
	AG_Box* sectionbox;
	strvec* section;
	const char* sectionname;
	AG_Object* parent;
	char* key;
	unsigned int i;

	/* link->userPtr should be safe now */
	AG_SetPointer(dri, "selected_link", edge);

	/* invalidate */
	AG_SetPointer(dri, "selected_node", NULL);

	/* delete the existing info box and create a new one */
	infobox = AG_GetPointer(dri, "infobox_p");
	parent = AG_ObjectParent(AGOBJECT(infobox));
	AG_ObjectDelete(infobox);
	infobox = AG_BoxNew(parent, AG_BOX_VERT, AG_BOX_EXPAND);
	AG_SetPointer(dri, "infobox_p", infobox);

	AG_SetStyle(infobox, "color", "rgb(196,196,196)");

#ifdef EBUG
	sectionbox = AG_BoxNew(infobox, AG_BOX_VERT, AG_BOX_HFILL | AG_BOX_FRAME);
	AG_BoxSetLabelS(sectionbox, "DEBUG DEBUG DEBUG");
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "parent=%p", (void*) parent);
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "infobox_p=%p", (void*) infobox);
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "selected_link=%p", (void*) edge);
	AG_LabelNew(sectionbox, AG_LABEL_HFILL, "link title=%s", edge->title);
#endif

	/* generate the info panel contents */
	nocviz_ds_foreach_section(edge->ds, sectionname, section,
		sectionbox = AG_BoxNew(infobox, AG_BOX_VERT, AG_BOX_HFILL | AG_BOX_FRAME);
		AG_BoxSetLabel(sectionbox, "%s", sectionname);
		vec_foreach(section, key, i) {

			AG_SetStyle(sectionbox, "color", "rgb(196,196,196)");

			/* NV_TextWidget comes from text_widget.{c,h}, and
			 * will automatically keep polling the given node ID
			 * on it's own */
			NV_TextWidgetNew(sectionbox,
					key,
					g_data,
					edge->from->id,
					key)->id2 = strdup(edge->to->id);
			/* ->id2 is how NV_TextWidget knows we are referring
			 * to a link rather than a node */

		}
	);

	/* workaround to force the widget to redraw immediately */
	AG_WidgetHide(infobox);
	AG_WidgetShow(infobox);

}
