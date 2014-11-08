/*
 * BezierHandle.cpp
 *
 *  Created on: 13 oct. 2014
 *      Author: jfellus
 */

#include "BezierHandle.h"
#include "LinkComponent.h"
#include "../widget/ZoomableDrawingArea.h"


BezierHandle::BezierHandle(LinkComponent* l, int i) : Component(false) {
	layer = 1000;
	link = l; this->i = i;
	ZoomableDrawingArea::cur()->add(this);
	update();
}

BezierHandle::~BezierHandle() {
	ZoomableDrawingArea::cur()->remove(this);
}

void BezierHandle::render(Graphics& g) {
	g.set_color(RGB(0.5,1.0,0));
	g.drawLine(i==0 ? link->src->center() : link->dst->center(), Vector2D(x,y));
	g.drawHandle(x,y);
}


void BezierHandle::translate(double dx, double dy) {
	x += dx; y += dy;
	Vector2D cs = link->src->center(); Vector2D cd = link->dst->center();
	if(i == 0) {
		link->b->x2 = (x - cs.x)/(cd.x-cs.x);
		link->b->y2 = (y - cs.y)/(cd.y-cs.y);
	} else {
		link->b->x3 = (x - cs.x)/(cd.x-cs.x);
		link->b->y3 = (y - cs.y)/(cd.y-cs.y);
	}
	Component::set_pos(x,y);
}

void BezierHandle::select(bool single) {
	return;
	link->select(single);
	Component::select(single);
}


Rectangle BezierHandle::get_bounds() {
	return Rectangle(x-15/canvas->get_zoom(), y-15/canvas->get_zoom(), 30/canvas->get_zoom(), 30/canvas->get_zoom());
}

void BezierHandle::update() {
	Vector2D cs = link->src->center(); Vector2D cd = link->dst->center();
	if(i == 0) {
		x = cs.x*(1-link->b->x2) + cd.x*link->b->x2;
		y = cs.y*(1-link->b->y2) + cd.y*link->b->y2;
	} else {
		x = cs.x*(1-link->b->x3) + cd.x*link->b->x3;
		y = cs.y*(1-link->b->y3) + cd.y*link->b->y3;
	}
}

