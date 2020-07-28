#ifndef NOCVIZ_GRAPH_WIDGET_H
#define NOCVIZ_GRAPH_WIDGET_H

#include "graph.h"
#include "datastore.h"

#include <agar/core.h>
#include <agar/gui.h>

#include <agar/math/m.h>
#include <agar/math/m_bezier_primitives.h>

typedef struct nocviz_graph_widget {
	struct ag_widget _inherit;
	nocviz_graph* g;
	int xOffs;
	int yOffs;
	int wPre;
	int hPre;
	AG_Rect r;
	unsigned long flags;
#define NV_GRAPH_SCROLL 0x01
#define NV_GRAPH_PANNING 0x02
} NV_GraphWidget;

#define NV_GRAPHWIDGET(obj)           ((NV_GraphWidget*)(obj))
#define NV_CGRAPHWIDGET(obj)          ((const NV_GraphWidget*)(obj))
#define NV_GRAPHWIDGET_SELF()          NV_GRAPHWIDGET( AG_OBJECT(0,"AG_Widget:NV_GraphWidget:*") )
#define NV_GRAPHWIDGET_PTR(n)          NV_GRAPHWIDGET( AG_OBJECT((n),"AG_Widget:NV_GraphWidget:*") )
#define NV_GRAPHWIDGET_NAMED(n)        NV_GRAPHWIDGET( AG_OBJECT_NAMED((n),"AG_Widget:NV_GraphWidget:*") )
#define CONST_NV_GRAPHWIDGET_SELF()   NV_CGRAPHWIDGET( AG_CONST_OBJECT(0,"AG_Widget:NV_GraphWidget:*") )
#define CONST_NV_GRAPHWIDGHET_PTR(n)   NV_CGRAPHWIDGET( AG_CONST_OBJECT((n),"AG_Widget:NV_GraphWidget:*") )
#define CONST_NV_GRAPHWIDGET_NAMED(n) NV_CGRAPHWIDGET( AG_CONST_OBJECT_NAMED((n),"AG_Widget:NV_GraphWidget:*") )

extern AG_WidgetClass NV_GraphWidgetClass;
NV_GraphWidget* NV_GraphWidgetNew(void* parent, nocviz_graph* g);
void NV_GraphSizeHint(NV_GraphWidget* gw, int w, int h);

#endif
