/*
 * ZoomableDrawingArea.h
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#ifndef ZOOMABLEDRAWINGAREA_H_
#define ZOOMABLEDRAWINGAREA_H_

#include <gtk/gtk.h>
#include "../graphics/svg.h"
#include "../components/Component.h"
#include "../components/Creator.h"
#include <vector>
#include <algorithm>
#include "Widget.h"

class Component;
class BoundingBox;

enum MoveMode {
	MOVE_MODE_SCROLL,
	MOVE_MOUSE_MIDDLE_BUTTON
};

enum SelectionRenderingMode {
	SELECTION_ShowComponentsBoundingBoxes,
	SELECTION_ShowSingleBoundingBox,
	SELECTION_Colored,
};

class IKeyListener {
public:
	void (*fn)(void);
	guint key; guint mask;
public:
	IKeyListener(int key, int mask, void (*fn)(void)){this->key = key; this->mask = mask; this->fn = fn;}
	virtual ~IKeyListener(){}
	void on_key() {fn();}
};

class IMouseListener {
public:
	IMouseListener() {}
	virtual ~IMouseListener(){}
	virtual void on_mouse_move(const Vector2D& docPos) {}
};



class ZoomableDrawingArea : public Widget {

	std::vector<Component*> components;
	std::vector<Component*> components_selectables;
	std::vector<Component*> components_topified;
	std::vector<Component*> selection;
	std::vector<IKeyListener*> keylisteners;
	std::vector<IMouseListener*> mouseListeners;

	Creator* creator = 0;

	BoundingBox* selectionBox = 0;
	BoundingBox* multi_selectionBox = 0;
	Component* draggedComponent = 0;
	double drag_start_x = 0, drag_start_y = 0;
	bool bFirstDraw = true;
	Vector2D mousePosDoc;

	double _zoom;
	double offsetx, offsety;
	double oldx_mouse=0, oldy_mouse=0, oldx=0, oldy=0;

	bool isDragging;

	enum MoveMode moveMode = MOVE_MOUSE_MIDDLE_BUTTON;
public:
	enum SelectionRenderingMode selectionRenderingMode = SELECTION_Colored;
	bool isSelecting = false;

public:
	ZoomableDrawingArea(Widget* win);
	virtual ~ZoomableDrawingArea();
	static ZoomableDrawingArea* cur();

	double get_zoom() { return _zoom; }

	void add(Component* c);
	void remove(Component* c);

	void add_selectable(Component* c);
	void remove_selectable(Component* c);
	void topify(Component* c);
	void untopify(Component* c);

	void add_key_listener(IKeyListener* kl) {keylisteners.push_back(kl);}
	void add_key_listener(IMouseListener* kl) {mouseListeners.push_back(kl);}

	double mouse_to_doc_x(double x) {return (x-offsetx)/_zoom;}
	double mouse_to_doc_y(double y) {return (y-offsety)/_zoom;}
	double doc_to_mouse_x(double x) {return x*_zoom + offsetx; }
	double doc_to_mouse_y(double y) {return y*_zoom + offsety; }

	Vector2D get_mouse_pos_doc() {return mousePosDoc;}

	void zoom(float fzoom) {_zoom *= (1 + fzoom); repaint();}
	void zoom_reset() {	_zoom = 1;	offsetx = offsety = 0;	repaint(); }
	void move(double dx, double dy) {offsetx += dx; offsety += dy; repaint();}
	void zoom(float fzoom, double cx, double cy);

	void zoom_all();
	void zoom(Component* c);
	void zoom(const Rectangle& r);

	void translate_selection(double dx, double dy);

	virtual void repaint() {if(!isSelecting) Widget::repaint();}
	void update_layers();


	void select(Component* c, bool single);
	void select(double x1, double y1, double x2, double y2);
	void unselect(Component* c);
	void unselect_all();
	void toggle_select(Component* c);
	void select_all();
	bool has_selection() {return !selection.empty();}

	Component* get_selectable_component_at(double x, double y);


	void start_creator(Creator* c);
	void end_creator();


	void add_selection(Component* c);
	void remove_selection(Component* c);
protected:
	friend struct _callbacks;
	virtual bool on_click(GdkEventButton* event);
	virtual bool on_dbl_click(GdkEventButton* event);
	virtual bool on_unclick(GdkEventButton* event);
	virtual bool on_draw(cairo_t* cr);
	virtual bool on_key(GdkEventKey* event);
	virtual bool on_mouse_move(GdkEventMotion* event);
	virtual bool on_scroll(GdkEventScroll* event);
	virtual void on_first_draw();
public:
	virtual void on_realizes();
};



#endif /* ZOOMABLEDRAWINGAREA_H_ */
