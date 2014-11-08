/*
 * ChangeGroupCreator.cpp
 *
 *  Created on: 31 oct. 2014
 *      Author: jfellus
 */

#include "ChangeGroupCreator.h"
#include "../module/Document.h"
#include "../module/GroupComponent.h"
#include "../widget/ZoomableDrawingArea.h"
#include "../workbench/Workbench.h"

ChangeGroupCreator::ChangeGroupCreator() {
	group = 0;
}

void ChangeGroupCreator::create(double x, double y) {
	Document::cur()->change_selection_group(group);
	canvas->update_layers();
	end();
}

void ChangeGroupCreator::end() {
	if(group) {
		if(group->component) group->component->remove_class("highlighted");
		if(group->component_open) group->component_open->remove_class("highlighted");
	}
	Creator::end();
}

void ChangeGroupCreator::render(Graphics& g) {
	g.set_color(RGB(0,0.5,0.8));
	g.circle(x,y,10/canvas->get_zoom());
	g.circle(x,y,5/canvas->get_zoom());
	g.stroke_line_width_independent(2);
}

void ChangeGroupCreator::on_mouse_move(GdkEventMotion* e) {
	Creator::on_mouse_move(e);
	if(group) {
		if(group->component) group->component->remove_class("highlighted");
		if(group->component_open) group->component_open->remove_class("highlighted");
	}
	Group* g = Document::cur()->get_group_at(x,y);
	STATUS(x << "," << y << " : " << g);
	if(g!=NULL) {
		STATUS(g);
		if(g->component) g->component->add_class("highlighted");
		if(g->component_open) g->component_open->add_class("highlighted");
	}
	group = g;
}

