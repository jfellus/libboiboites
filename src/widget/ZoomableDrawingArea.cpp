/*
 * ZoomableDrawingArea.cpp
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#include "ZoomableDrawingArea.h"
#include "../util/utils.h"
#include "../components/BoundingBox.h"
#include "../components/LinkCreator.h"
#include "../components/ModuleCreator.h"
#include <typeinfo>
#include "../workbench/Workbench.h"

typedef struct _callbacks {
	static gboolean _on_mouse_move (GtkWidget *widget, GdkEventMotion  *event, gpointer   user_data) {
		return ((ZoomableDrawingArea*)user_data)->on_mouse_move(event);
	}

	static gboolean _on_click (GtkWidget *widget, GdkEventButton  *event, gpointer   user_data) {
		return((ZoomableDrawingArea*)user_data)->on_click(event);
	}

	static gboolean _on_unclick (GtkWidget *widget, GdkEventButton  *event, gpointer   user_data) {
		return ((ZoomableDrawingArea*)user_data)->on_unclick(event);
	}

	static gboolean _on_scroll (GtkWidget *widget, GdkEventScroll  *event, gpointer   user_data) {
		return ((ZoomableDrawingArea*)user_data)->on_scroll(event);
	}

	static gboolean _on_realize (GtkWidget *widget, gpointer   user_data) {
		((ZoomableDrawingArea*)user_data)->on_realizes();
		return true;
	}

	static gboolean _on_draw (GtkWidget    *widget, cairo_t *cr,  gpointer      user_data) {
		return ((ZoomableDrawingArea*)user_data)->on_draw(cr);
	}

	static gboolean _on_key (GtkWidget *widget, GdkEventKey  *event, gpointer   user_data) {
		return ((ZoomableDrawingArea*)user_data)->on_key(event);
	}
} _callbacks;

static ZoomableDrawingArea* _cur = 0;
ZoomableDrawingArea* ZoomableDrawingArea::cur() {
	return _cur;
}


ZoomableDrawingArea::ZoomableDrawingArea(Widget* w) : Widget(gtk_drawing_area_new()){
	_cur = this;
	gtk_widget_show(widget);
	isDragging = false;
	_zoom = 1;
	offsetx = offsety = 0;
	draggedComponent = 0;
	set_events(GDK_ALL_EVENTS_MASK);
	gtk_widget_set_can_focus(widget, TRUE);
	g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(_callbacks::_on_draw), this);
	g_signal_connect(G_OBJECT(widget), "scroll-event", G_CALLBACK(_callbacks::_on_scroll), this);
	g_signal_connect(G_OBJECT(widget), "motion-notify-event", G_CALLBACK(_callbacks::_on_mouse_move), this);
	g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(_callbacks::_on_click), this);
	g_signal_connect(G_OBJECT(widget), "button-release-event", G_CALLBACK(_callbacks::_on_unclick), this);
	g_signal_connect(G_OBJECT(widget), "key-press-event", G_CALLBACK(_callbacks::_on_key), this);
	g_signal_connect(G_OBJECT(widget), "realize", G_CALLBACK(_callbacks::_on_realize), this);
	grab_focus();

	selectionBox = new BoundingBox(&selection);
	multi_selectionBox = new BoundingBox(Rectangle());
}

ZoomableDrawingArea::~ZoomableDrawingArea() {

}


void ZoomableDrawingArea::on_realizes() {
	grab_focus();
	zoom_all();
}

bool ZoomableDrawingArea::on_key(GdkEventKey* event) {
	if(event->state & GDK_CONTROL_MASK) {
		if(event->keyval==GDK_KEY_a) select_all();

		double a = 10;
		if(event->state & GDK_SHIFT_MASK) a *= 5;
		if(event->keyval==GDK_KEY_Left) move(a,0);
		if(event->keyval==GDK_KEY_Right) move(-a,0);
		if(event->keyval==GDK_KEY_Up) move(0,a);
		if(event->keyval==GDK_KEY_Down) move(0, -a);
	} else {
		if(event->keyval==GDK_KEY_z) zoom_all();


		double a = 3;
		if(event->state & GDK_SHIFT_MASK) a *= 5;
		if(event->keyval==GDK_KEY_Left) translate_selection(-a/_zoom, 0);
		if(event->keyval==GDK_KEY_Right) translate_selection(a/_zoom, 0);
		if(event->keyval==GDK_KEY_Up) translate_selection(0,-a/_zoom);
		if(event->keyval==GDK_KEY_Down) translate_selection(0,a/_zoom);
	}

	for(uint i=0; i<keylisteners.size(); i++) {
		IKeyListener* kl = keylisteners[i];
		if(event->keyval==kl->key && ((event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK | GDK_SUPER_MASK)) == kl->mask)) kl->on_key();
	}

	grab_focus();
	return TRUE;
}

bool ZoomableDrawingArea::on_dbl_click(GdkEventButton* e) {
	if(selection.empty()) return false;
	selection[0]->on_dbl_click(e);
	return false;
}


bool ZoomableDrawingArea::on_click(GdkEventButton* event) {
	if(event->type == GDK_2BUTTON_PRESS) return on_dbl_click(event);
	if(creator) {
		creator->on_click(event);
		return true;
	}

	if(event->button==1) {
		drag_start_x = mouse_to_doc_x(event->x);
		drag_start_y = mouse_to_doc_y(event->y);

		if(event->state & GDK_SHIFT_MASK) draggedComponent = 0;
		else {
			draggedComponent = get_selectable_component_at(drag_start_x, drag_start_y);
			if(draggedComponent) draggedComponent->click(drag_start_x, drag_start_y);
			if(!draggedComponent || !draggedComponent->is_selected()) {
				unselect_all();
				if(draggedComponent) select(draggedComponent, true);
			}
		}
	}

	grab_focus();
	return true;
}

bool ZoomableDrawingArea::on_unclick(GdkEventButton* event) {
	mousePosDoc.x = mouse_to_doc_x(event->x);
	mousePosDoc.y = mouse_to_doc_y(event->y);

	if(creator) {
		creator->on_unclick(event);
		return true;
	}

	if(event->button==1) {
		multi_selectionBox->set_bounds(Rectangle());

		if(isDragging && !draggedComponent) {
			select(drag_start_x, drag_start_y, mousePosDoc.x, mousePosDoc.y);
		}

		if(!isDragging && (event->state & GDK_SHIFT_MASK)) {
			draggedComponent = get_selectable_component_at(mousePosDoc.x, mousePosDoc.y);
			if(draggedComponent) toggle_select(draggedComponent);
		}

		draggedComponent = 0;
		isDragging = false;
		repaint();
	}

	grab_focus();
	return true;
}



bool ZoomableDrawingArea::on_mouse_move(GdkEventMotion* event) {
	mousePosDoc.x = mouse_to_doc_x(event->x);
	mousePosDoc.y = mouse_to_doc_y(event->y);

	if((event->state & GDK_BUTTON2_MASK) && (moveMode == MOVE_MOUSE_MIDDLE_BUTTON)) {
		move(event->x - oldx_mouse, event->y - oldy_mouse);
	}

	else if(creator) { creator->on_mouse_move(event); }

	else if(event->state & GDK_BUTTON1_MASK) {
		isDragging = true;
		if(has_selection() && !(event->state & GDK_SHIFT_MASK)) {
			translate_selection(mousePosDoc.x-oldx, mousePosDoc.y-oldy);
		} else {
			multi_selectionBox->set_bounds(Rectangle(drag_start_x, drag_start_y,mousePosDoc.x-drag_start_x, mousePosDoc.y-drag_start_y));
		}
		repaint();
	}
	oldx = mousePosDoc.x; oldy = mousePosDoc.y;
	oldx_mouse = event->x; oldy_mouse = event->y;
	for(uint i=0; i<mouseListeners.size(); i++) mouseListeners[i]->on_mouse_move(mousePosDoc);
	return true;
}

bool ZoomableDrawingArea::on_scroll(GdkEventScroll* event) {
	double dx, dy;
	gdk_event_get_scroll_deltas((const GdkEvent*)event, &dx, &dy);
	if(event->state & GDK_CONTROL_MASK) zoom(-0.1 * dy, event->x, event->y);
	else if(moveMode==MOVE_MODE_SCROLL) move(20*dx, 20*dy);
	return true;
}

void ZoomableDrawingArea::on_first_draw() {
	zoom_all();
	bFirstDraw = false;
}

bool ZoomableDrawingArea::on_draw(cairo_t* cr) {
	if(bFirstDraw) on_first_draw();
	Graphics g(cr, this);
	g.fill_all(RGB_WHITE);
	g.translate(offsetx, offsety);
	g.scale(_zoom);

	g.set_color(RGB_BLACK);
	for(uint i=0; i<components.size(); i++) {
		components[i]->draw(g);
	}

	if(selectionRenderingMode==SELECTION_ShowSingleBoundingBox && selectionBox) selectionBox->draw(g);
	if(multi_selectionBox) multi_selectionBox->draw(g);
	if(creator) creator->draw(g);

	return FALSE;
}



void ZoomableDrawingArea::zoom(float fzoom, double cx, double cy) {
	cx =  (cx - offsetx)/ _zoom ;
	cy =  (cy - offsety)/ _zoom ;
	double oldzoom = _zoom;
	_zoom *= 1 + fzoom;
	offsetx -= cx*(_zoom - oldzoom);
	offsety -= cy*(_zoom - oldzoom);
	repaint();
}


static bool _drawingLayerComp(Component* c1, Component* c2) {
	return c1->layer < c2->layer;
}

static bool _selectionLayerComp(Component* c1, Component* c2) {
	return c1->selectionLayer < c2->selectionLayer;
}

void ZoomableDrawingArea::update_layers() {
	std::sort (components.begin(), components.end(), _drawingLayerComp);
	repaint();
}

void ZoomableDrawingArea::add(Component* c) {
	components.push_back(c);
	c->set_canvas(this);
	update_layers();
}

void ZoomableDrawingArea::remove(Component* c) {
	remove_selectable(c);
	if(c->bSelected) remove_selection(c);
	vector_remove(components,c); c->set_canvas(0); repaint();
}

void ZoomableDrawingArea::add_selectable(Component* c) {
	components_selectables.push_back(c);
	std::sort (components_selectables.begin(), components_selectables.end(), _selectionLayerComp);
}

void ZoomableDrawingArea::remove_selectable(Component* c) {vector_remove(components_selectables,c);}

void ZoomableDrawingArea::topify(Component* c) {
	components_topified.push_back(c);
}
void ZoomableDrawingArea::untopify(Component* c) {
	vector_remove(components_topified,c);
}


Component* ZoomableDrawingArea::get_selectable_component_at(double x, double y) {
	for(int i=components_topified.size()-1; i>=0; i--) {
		Component* c = components_topified[i];
		if(c->visible && c->hasPoint(x,y)) return c;
	}
	for(int i=components_selectables.size()-1; i>=0; i--) {
		Component* c = components_selectables[i];
		if(c->visible && c->hasPoint(x,y)) return c;
	}
	return 0;
}

void ZoomableDrawingArea::select_all() {
	isSelecting = true;
	for(uint i=0; i<components_selectables.size(); i++) if(components_selectables[i]->visible) select(components_selectables[i], false);
	isSelecting = false;
	if(Workbench::cur()) Workbench::cur()->update();
}

void ZoomableDrawingArea::select(Component* c, bool single) {
	c->select(single);
}

void ZoomableDrawingArea::toggle_select(Component* c) {
	c->toggle_select();
}


void ZoomableDrawingArea::select(double x1, double y1, double x2, double y2) {
	isSelecting = true;
	Rectangle r(x1,y1,x2-x1,y2-y1);
	for(uint i=0; i<components_topified.size(); i++) {
		Component* c = components_topified[i];
		if(!c->visible) continue;
		if(c->get_bounds().is_in(r)) select(c, false);
	}
	for(uint i=0; i<components_selectables.size(); i++) {
		Component* c = components_selectables[i];
		if(!c->visible) continue;
		if(c->get_bounds().is_in(r)) select(c, false);
	}
	isSelecting = false;
	if(Workbench::cur()) Workbench::cur()->update();
}

void ZoomableDrawingArea::unselect(Component* c) {
	c->unselect();
}

void ZoomableDrawingArea::unselect_all() {
	isSelecting = true;
	while(!components_topified.empty()) untopify(components_topified[0]);
	while(!selection.empty()) selection[0]->unselect();
	selectionBox->set_bounds(Rectangle());
	isSelecting = false;
	repaint();
	if(Workbench::cur()) Workbench::cur()->update();
}

void ZoomableDrawingArea::add_selection(Component* c) {
	selection.push_back(c);
	selectionBox->set_bounds(&selection);
	repaint();
}

void ZoomableDrawingArea::remove_selection(Component* c) {
	vector_remove(selection, c);
	selectionBox->set_bounds(&selection);
	if(selection.empty()) selectionBox->set_bounds(Rectangle());
	repaint();
}


void ZoomableDrawingArea::translate_selection(double dx, double dy) {
	for(uint i=0; i<selection.size(); i++) {
		selection[i]->translate(dx,dy);
	}
	grab_focus();
}


void ZoomableDrawingArea::zoom_all() {
	if(components.empty()) return;
	Rectangle r = components[0]->get_bounds();
	for(uint i=1; i<components.size(); i++) {
		if(components[i]->visible) r.add(components[i]->get_bounds());
	}
	zoom(r);
}

void ZoomableDrawingArea::zoom(Component* c) {
	zoom(c->get_bounds());
}

void ZoomableDrawingArea::zoom(const Rectangle& r) {
	double w = get_width()-20;
	double h = get_height()-20;
	if(!w || !h) return;
	_zoom = MIN(h/r.h, w/r.w);

	offsetx= -r.x*_zoom + 10 + MAX(0,(w - r.w*_zoom)/2);
	offsety= -r.y*_zoom + 10 + MAX(0,(h - r.h*_zoom)/2);
	repaint();
}

void ZoomableDrawingArea::start_creator(Creator* c) {creator = c; c->start(this); repaint();}
void ZoomableDrawingArea::end_creator() {
	delete creator;
	creator = NULL; repaint();
}


