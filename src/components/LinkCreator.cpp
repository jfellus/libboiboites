/*
 * LinkCreator.cpp
 *
 *  Created on: 6 oct. 2014
 *      Author: jfellus
 */

#include "LinkCreator.h"
#include "LinkComponent.h"
#include "../widget/ZoomableDrawingArea.h"
#include "../module/Document.h"


LinkCreator::LinkCreator() {
}

void LinkCreator::create(double x, double y) {
	if(!src) {
		Component* c = canvas->get_selectable_component_at(x,y);
		if(!c) return;
		Module* m = (Module*) c->get_user_data("Module");
		if(m) src = m;
		repaint();
	} else {
		Component* c = canvas->get_selectable_component_at(x,y);
		if(!c) return;
		Module* m = (Module*) c->get_user_data("Module");
		if(!m) return;
		dst = m;
		link = new Link(src, dst);
		end();
	}
}

void LinkCreator::render(Graphics& g) {
	g.set_color(src ? RGB_BLUE : RGB_RED);
	g.drawPoint(x,y);

	if(src) {
		g.drawLine(src->component->center(), Vector2D(x,y));
		g.set_color(RGB(0.5,0.5,1));
		g.drawRect(src->component->get_bounds());
	}
}



