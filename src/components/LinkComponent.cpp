/*
 * LinkComponent.cpp
 *
 *  Created on: 3 oct. 2014
 *      Author: jfellus
 */

#include "LinkComponent.h"
#include "../util/geom.h"
#include "../widget/ZoomableDrawingArea.h"

bool LinkComponent::hasPoint(double x, double y) {
	//double d = Line2D(src->center(),dst->center()).distance(Vector2D(x,y));
	//return d >= 0 && d*canvas->get_zoom() < 10;
	if(bSelected) {
		if(bezierHandle1 && bezierHandle1->hasPoint(x,y)) return true;
		if(bezierHandle2 && bezierHandle2->hasPoint(x,y)) return true;
	}
	return bezier_absolute().has_point(x,y, 10/canvas->get_zoom());
}

void LinkComponent::render(Graphics& g) {
	g.set_color(bSelected ? RGB_RED : RGB_BLACK);
	render_line(g, 1);
	render_arrow(g);
}

void LinkComponent::render_line(Graphics& g, double tickness) {
	Bezier bezier_abs = bezier_absolute();
	Rectangle r = bezier_abs.get_bounds(); r.augment(10);
	g.rectangle(r);
	g.rectangle(src->get_bounds().reverse());
	g.clip();
	g.rectangle(r);
	g.rectangle(dst->get_bounds().augment(3.25/canvas->get_zoom()).reverse());
	g.clip();

	g.bezier(bezier_abs);
	g.stroke_line_width_independent(tickness);

	g.reset_clip();
}

double LinkComponent::render_arrow(Graphics& g, double size) {
	Bezier bezier_abs = bezier_absolute();
	Vector2D inter;
	if(bezier_abs.intersect(dst->get_bounds(),inter)) {
		double loc = bezier_abs.location(inter);
		g.drawArrowEnd(inter, bezier_abs.gradient(loc)*100, size);
		return loc;
	}
	return -1;
}


void LinkComponent::translate(double dx, double dy) {
	if(bezierHandle1) {
		if(imoving==0) bezierHandle1->translate(dx,dy);
		else if(imoving==1) bezierHandle2->translate(dx,dy);
	}
	repaint();
}

void LinkComponent::click(double x, double y) {
	if(bezierHandle1 && bezierHandle1->hasPoint(x,y)) imoving = 0;
	else if(bezierHandle2 && bezierHandle2->hasPoint(x,y)) imoving = 1;
	else imoving = -1;
}

void LinkComponent::select(bool single) {
	if(!bSelected && single) 	{
		create_bezier_handles();
		canvas->topify(this);
	}
	Component::select(single);
}

void LinkComponent::unselect() {
	if(bezierHandle1) canvas->untopify(this);
	if(bezierHandle1) delete bezierHandle1;
	if(bezierHandle2) delete bezierHandle2;
	bezierHandle1 = 0;
	bezierHandle2 = 0;
	Component::unselect();
}

