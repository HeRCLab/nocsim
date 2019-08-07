#ifndef NOCVIZ_TEXT_WIDGET_H
#define NOCVIZ_TEXT_WIDGET_H

#include "graph.h"
#include "datastore.h"

#include <agar/core.h>
#include <agar/gui.h>

typedef struct nocviz_text_widget {
	struct ag_widget _inherit;
	char* label_text;
	nocviz_graph* g;
	char* key;
	char* id;
	AG_Textbox* tb;
} NV_TextWidget;

#define NV_TEXTWIDGET(obj)           ((NV_TextWidget*)(obj))
#define NV_CTEXTWIDGET(obj)          ((const NV_TextWidget*)(obj))
#define NV_TEXTWIDGET_SELF()          NV_TEXTWIDGET( AG_OBJECT(0,"AG_Widget:NV_TextWidget:*") )
#define NV_TEXTWIDGET_PTR(n)          NV_TEXTWIDGET( AG_OBJECT((n),"AG_Widget:NV_TextWidget:*") )
#define NV_TEXTWIDGET_NAMED(n)        NV_TEXTWIDGET( AG_OBJECT_NAMED((n),"AG_Widget:NV_TextWidget:*") )
#define CONST_NV_TEXTWIDGET_SELF()   NV_CTEXTWIDGET( AG_CONST_OBJECT(0,"AG_Widget:NV_TextWidget:*") )
#define CONST_NV_TEXTWIDGHET_PTR(n)   NV_CTEXTWIDGET( AG_CONST_OBJECT((n),"AG_Widget:NV_TextWidget:*") )
#define CONST_NV_TEXTWIDGET_NAMED(n) NV_CTEXTWIDGET( AG_CONST_OBJECT_NAMED((n),"AG_Widget:NV_TextWidget:*") )

extern AG_WidgetClass NV_TextWidgetClass;
NV_TextWidget* NV_TextWidgetNew(void* parent, char* label, nocviz_graph* g, char* id, char* key);

#endif
