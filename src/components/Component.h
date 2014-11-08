/*
 * Component.h
 *
 *  Created on: 2 oct. 2014
 *      Author: jfellus
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "../util/geom.h"
#include "../graphics/Graphics.h"
#include <gtk/gtk.h>
#include "../util/selection/ISelectable.h"
#include "style/IStyle.h"

class ZoomableDrawingArea;
class BoundingBox;

class Component : public ISelectable {
	struct user_data {std::string type_info; void* p;};
public:
	double x,y;

	bool visible = false;
protected:
	ZoomableDrawingArea* canvas;
	BoundingBox* selectionBox = 0;
	bool auto_addToCanvas = true;

	struct user_data user_data;

	IStyle* style = NULL;
	std::string css_class;

public:
	int selectionLayer = 0;
	float layer = 0;

public:
	Component(int layer);
	Component(bool auto_addToCanvas = true);
	virtual ~Component();

	virtual void set_canvas(ZoomableDrawingArea* c) {canvas = c;}

	void set_pos(double x, double y) { this->x = x; this->y = y; repaint();}
	void set_pos(const Vector2D& p) { set_pos(p.x, p.y);}
	void center(const Vector2D& p) { Rectangle r = get_bounds(); set_pos(p.x - r.w/2, p.y - r.h/2);}
	void set_selectable(bool b = true);

	void add_class(const std::string& c);
	void remove_class(const std::string& c);
	bool has_class(const std::string& c);
	void toggle_class(const std::string& c) {if(has_class(c)) remove_class(c); else add_class(c);}

	virtual void render(Graphics& g) = 0;

	virtual void show() { visible=true; repaint();}
	virtual void hide() { visible=false; repaint();}

	virtual Vector2D center() { return get_bounds().center();}

	virtual void transform(Graphics& g) {
		g.translate(x,y);
	}

	virtual void translate(double dx, double dy) {
		this->x += dx; this->y += dy; repaint();
	}

	virtual void draw(Graphics& g);

	virtual void select(bool single);
	virtual void unselect();

	virtual Rectangle get_bounds() { return Rectangle();}
	virtual bool hasPoint(double x, double y) {return get_bounds().contains(x,y); }
	virtual void click(double x, double y) {}

	void set_user_data(const std::string& type_info, void* data) {user_data.p = data; user_data.type_info = type_info;}
	void* get_user_data(const std::string& type_info) {return user_data.type_info==type_info ? user_data.p : NULL;}

	void repaint();

	virtual void dump(std::ostream& os) { os << "Component(" << (void*)this << ")";}
	friend std::ostream& operator<<(std::ostream& os, Component* a);
};

std::ostream& operator<<(std::ostream& os, Component* a);


#endif /* COMPONENT_H_ */
