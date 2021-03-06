/* This module contains considerable code derived from AG_Graph, which
 * is distributed under the following license:
 *
 * Copyright (c) 2007-2019 Julien Nadeau Carriere <vedge@csoft.net>,
 * 2019 Charles A. Daniels, <charles@cdaniels.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Modifications made to this code are covered by the nocviz/nocsim project
 * license.
 */

#include "graph_widget.h"

NV_GraphWidget* NV_GraphWidgetNew(void* parent, nocviz_graph* g) {
	NV_GraphWidget* gw;

	gw = noctools_malloc(sizeof(NV_GraphWidget));

	gw->g = g;

	AG_ObjectInit(gw, &NV_GraphWidgetClass);

	AG_ExpandHoriz(gw);
	AG_ExpandVert(gw);

	AG_ObjectAttach(parent, gw);

	return gw;
}

static int MouseOverVertex(nocviz_node *vtx, NV_GraphWidget* gw, int x, int y) {
	return (abs(x - vtx->x + gw->xOffs) <= (vtx->w >> 1) &&
	        abs(y - vtx->y + gw->yOffs) <= (vtx->h >> 1));
}

static int MouseOverEdge(nocviz_link *edge, NV_GraphWidget* gw, int x, int y) {
	return ((abs(x - edge->hx + gw->xOffs) < 20) && (abs(y - edge->hy + gw->yOffs) < 20));
}

static void KeyDown(AG_Event *event) {
	NV_GraphWidget *gw = NV_GRAPHWIDGET_SELF();
	const int keysym = AG_INT(1);
	const int scrollIncr = 10;

	switch (keysym) {
	case AG_KEY_LEFT:
		gw->xOffs -= scrollIncr;
		break;
	case AG_KEY_RIGHT:
		gw->xOffs += scrollIncr;
		break;
	case AG_KEY_UP:
		gw->yOffs -= scrollIncr;
		break;
	case AG_KEY_DOWN:
		gw->yOffs += scrollIncr;
		break;
	case AG_KEY_0:
		gw->xOffs = 0;
		gw->yOffs = 0;
		break;
	}
	AG_Redraw(gw);
}

static void SelectVertex(NV_GraphWidget *gw, nocviz_node* vtx) {
	vtx->flags |= NOCVIZ_NODE_SELECTED;
	AG_PostEvent(gw, "graph-vertex-selected", "%p", vtx);
	AG_Redraw(gw);
}

static void UnselectVertex(NV_GraphWidget *gw, nocviz_node* vtx) {
	vtx->flags &= ~(NOCVIZ_NODE_SELECTED);
	AG_PostEvent(gw, "graph-vertex-unselected", "%p", vtx);
	AG_Redraw(gw);
}

static void SelectEdge(NV_GraphWidget *gw, nocviz_link* edge) {
	edge->flags |= NOCVIZ_LINK_SELECTED;
	AG_PostEvent(gw, "graph-edge-selected", "%p", edge);
	AG_Redraw(gw);
}

static void UnselectEdge(NV_GraphWidget *gw, nocviz_link* edge) {
	edge->flags &= ~(NOCVIZ_LINK_SELECTED);
	AG_PostEvent(gw, "graph-edge-unselected", "%p", edge);
	AG_Redraw(gw);
}


static void MouseButtonDown(AG_Event *event) {
	NV_GraphWidget *gw = NV_GRAPHWIDGET_SELF();
	const int button = AG_INT(1);
	const int x = AG_INT(2);
	const int y = AG_INT(3);
	const AG_KeyMod kmod = AG_GetModState(gw);
	nocviz_node *vtx;
	nocviz_link* edge;

	if (!AG_WidgetIsFocused(gw))
		AG_WidgetFocus(gw);

	switch (button) {
	case AG_MOUSE_MIDDLE:
		gw->flags |= NV_GRAPH_PANNING;
		break;
	case AG_MOUSE_LEFT:
		if (kmod & (AG_KEYMOD_CTRL | AG_KEYMOD_SHIFT)) {

			/* TODO: handle edge multi selection */

			nocviz_graph_foreach_node(gw->g, vtx,
				if (!MouseOverVertex(vtx, gw, x,y)) {
					continue;
				}
				if (vtx->flags & NOCVIZ_NODE_SELECTED) {
					UnselectVertex(gw, vtx);
				} else {
					SelectVertex(gw, vtx);
				}
				);

			nocviz_graph_foreach_link(gw->g, edge,
				if (!MouseOverEdge(edge, gw, x,y)) {
					continue;
				}
				if (edge->flags & NOCVIZ_LINK_SELECTED) {
					UnselectEdge(gw, edge);
				} else {
					SelectEdge(gw, edge);
				}
				);

		} else {

			nocviz_graph_foreach_node(gw->g, vtx,
				if (MouseOverVertex(vtx, gw, x, y)) {
					SelectVertex(gw, vtx);
				} else {
					UnselectVertex(gw, vtx);
				}
			);

			nocviz_graph_foreach_link(gw->g, edge,
				if (MouseOverEdge(edge, gw, x, y)) {
					SelectEdge(gw, edge);
				} else {
					UnselectEdge(gw, edge);
				}
			);
		}
	/* case AG_MOUSE_RIGHT: */
		/* handle right click menus */
	}
}


static void MouseButtonUp(AG_Event *event)
{
	NV_GraphWidget *gw = NV_GRAPHWIDGET_SELF();
	const int button = AG_INT(1);

	switch (button) {
		case AG_MOUSE_MIDDLE:
			gw->flags &= ~(NV_GRAPH_PANNING);
			break;
	}
}

static void MouseMotion(AG_Event *event) {
	NV_GraphWidget *gw = NV_GRAPHWIDGET_SELF();
	const int x = AG_INT(1);
	const int y = AG_INT(2);
	const int dx = AG_INT(3);
	const int dy = AG_INT(4);

	nocviz_node* vtx;
	nocviz_link* edge;

	if (gw->flags & NV_GRAPH_PANNING) {
		gw->xOffs -= dx;
		gw->yOffs -= dy;
		AG_Redraw(gw);
		return;
	}

	nocviz_graph_foreach_node(gw->g, vtx,
		if (MouseOverVertex(vtx, gw, x, y)) {
			vtx->flags |= NOCVIZ_NODE_HOVER;
		} else {
			vtx->flags &= ~(NOCVIZ_NODE_HOVER);
		}
	);

	nocviz_graph_foreach_link(gw->g, edge,
		if (MouseOverEdge(edge, gw, x, y)) {
			edge->flags |= NOCVIZ_LINK_HOVER;
		} else {
			edge->flags &= ~(NOCVIZ_LINK_HOVER);
		}
	);

}

static void Init(void* obj) {
	NV_GraphWidget* gw = obj;

	AGWIDGET(gw)->flags |= AG_WIDGET_FOCUSABLE;

	gw->xOffs = 0;
	gw->yOffs = 0;
	gw->hPre = 0;
	gw->wPre = 0;
	gw->flags = 0;
	gw->g->gw = gw;

	AG_SetEvent(gw, "key-down", KeyDown, NULL);
	AG_SetEvent(gw, "mouse-button-down", MouseButtonDown, NULL);
	AG_SetEvent(gw, "mouse-button-up", MouseButtonUp, NULL);
	AG_SetEvent(gw, "mouse-motion", MouseMotion, NULL);

}

static void Destroy(void *p) {
	NV_GraphWidget* gw = p;
	gw->g->gw = NULL;
	free(gw);
}

void NV_GraphSizeHint(NV_GraphWidget* gw, int w, int h) {
	gw->wPre = w;
	gw->hPre = h;
}

static void SizeRequest(void* obj, AG_SizeReq* r) {
	NV_GraphWidget* gw = obj;
	r->w = gw->wPre;
	r->h = gw->hPre;
}

static int SizeAllocate(void* obj, const AG_SizeAlloc *a) {
	NV_GraphWidget* gw = obj;

	gw->r.x = 0;
	gw->r.y = 0;
	gw->r.w = a->w;
	gw->r.h = a->h;

	/* TODO: handle keeping nodes on screen with x/y Offs */

	return 0;
}

static void Draw(void* obj) {
	NV_GraphWidget* gw = obj;
	nocviz_node* vtx;
	nocviz_link* edge;
	AG_Rect r;
	AG_Color c;
	AG_Color outline_c;
	int xOffs = gw->xOffs;
	int yOffs = gw->yOffs;
	AG_Driver* dri = AG_ObjectFindParent(gw, "agDrivers", NULL);
	int* show_node_labels;
	int* show_edge_labels;

	AG_PushClipRect(gw, &gw->r);

	/* Draw the bounding box */
	AG_ColorRGB_8(&c, 128,128,128);
	AG_DrawRectOutline(gw, &gw->r, &c);

	AG_ColorBlack(&c);
	AG_TextColor(&c);

	AG_ColorRGB_8(&outline_c, 128, 255, 128);

	show_node_labels = AG_GetPointer(dri, "show_node_labels");
	show_edge_labels = AG_GetPointer(dri, "show_edge_labels");

	/* draw the links */
	nocviz_graph_foreach_link(gw->g, edge,

		/* label coordinates */
		int lx;
		int ly;
		AG_Color edgecolor;

		int curve = edge->curve;

		if (edge->flags & NOCVIZ_NODE_HOVER) {
			edgecolor = outline_c;
		} else {
			edgecolor = edge->c;
		}

		if (curve == 0) {
			AG_DrawLine(gw,
				edge->from->x - xOffs,
				edge->from->y - yOffs,
				edge->to->x - xOffs,
				edge->to->y - yOffs,
				&edgecolor);
			lx = (edge->from->x + edge->to->x) / 2;
			ly = (edge->from->y + edge->to->y) / 2;
		} else {

			double xv = edge->from->x - edge->to->x;
			double yv = edge->from->y - edge->to->y;
			double v_length = sqrt(xv * xv + yv * yv);

			int xc1 = (int) ((yv / v_length) * (float) curve + (float) edge->from->x);
			int yc1 = (int) ((-xv / v_length) * (float) curve + (float) edge->from->y);
			int xc2 = (int) ((yv / v_length) * (float) curve + (float) edge->to->x);
			int yc2 = (int) ((-xv / v_length) * (float) curve + (float) edge->to->y);

			M_DrawBezier2(AGWIDGET(gw),
				edge->from->x - xOffs, edge->from->y - yOffs,
				xc1 - xOffs, yc1 - yOffs,
				xc2 - xOffs, yc2 - yOffs,
				edge->to->x - xOffs, edge->to->y - yOffs,
				10, &edgecolor);

			lx = (xc1 + xc2) / 2;
			ly = (yc1 + yc2) / 2;
		}

		edge->hx = lx;
		edge->hy = ly;

		lx -= xOffs;
		ly -= yOffs;

		/* if the title text has changed or we don't have a surface
		 * yet, update the surface */
		if ((edge->surface_dirty != 0) || (edge->label_surface < 0)) {
			if (edge->label_surface >= 0) {
				AG_WidgetUnmapSurface(gw, edge->label_surface);
				edge->label_surface = -1;
			}
			edge->label_surface = AG_WidgetMapSurface(gw,
				AG_TextRender(edge->title));
			edge->surface_dirty = 0;
		}

		if ((edge->label_surface >= 0) && (*show_edge_labels == 1)) {
			AG_WidgetBlitSurface(gw, edge->label_surface, lx, ly);
		}

	);

	/* draw the nodes */
	nocviz_graph_foreach_node(gw->g, vtx,
		vtx->x = vtx->col * vtx->w * 2;
		vtx->y = vtx->row * vtx->h * 2;
		r.x = vtx->x - xOffs - (vtx->w >> 1);
		r.y = vtx->y - yOffs - (vtx->h >> 1);
		r.h = vtx->h;
		r.w = vtx->w;
		AG_DrawRect(gw, &r, &vtx->c);
		if (vtx->flags & NOCVIZ_NODE_HOVER) {
			AG_DrawRectOutline(gw, &r, &outline_c);

		}

		/* if the title text has changed or we don't have a surface
		 * yet, update the surface */
		if ((vtx->surface_dirty != 0) || (vtx->label_surface < 0)) {
			if (vtx->label_surface >= 0) {
				AG_WidgetUnmapSurface(gw, vtx->label_surface);
				vtx->label_surface = -1;
			}
			vtx->label_surface = AG_WidgetMapSurface(gw,
				AG_TextRender(vtx->title));
			vtx->surface_dirty = 0;
		}

		if ((vtx->label_surface >= 0) && (*show_node_labels == 1)) {
			AG_WidgetBlitSurface(gw, vtx->label_surface, r.x, r.y);
		}
	);

	AG_PopClipRect(gw);

}

AG_WidgetClass NV_GraphWidgetClass = {
	{
		"AG_Widget:NV_GraphWidget",
		sizeof(NV_GraphWidget),
		{ 0,0 },
		Init,
		NULL,		/* reset */
		Destroy,
		NULL,		/* load */
		NULL,		/* save */
		NULL,		/* edit */
		"NV_GraphWidget",
		&agWidgetClass._inherit,
		{{0}, {0}, {0}}
	},
	Draw,
	SizeRequest,
	SizeAllocate
};

