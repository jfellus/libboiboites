/*
 * ModuleCreator.cpp
 *
 *  Created on: 9 oct. 2014
 *      Author: jfellus
 */

#include "ModuleCreator.h"
#include <ZoomableDrawingArea.h>
#include "../module/Document.h"

namespace libboiboites {

ModuleCreator::ModuleCreator() {
	module = 0;
}

void ModuleCreator::create(double x, double y) {
	module = new Module();
	comp = module->component;
	comp->set_pos(x,y);
	end();
}

void ModuleCreator::end() {
	if(module && group) group->add(module);
	Document::cur()->update_links_layers();
	Document::cur()->fire_change_event();
	canvas->update_layers();
	if(group && group->component) group->component->remove_class("highlighted");
	if(group && group->component_open) group->component_open->remove_class("highlighted");
	Creator::end();
}

void ModuleCreator::render(Graphics& g) {
	g.set_color(RGB_RED);
	g.drawPoint(x,y);
}

void ModuleCreator::on_mouse_move(GdkEventMotion* e) {
	Creator::on_mouse_move(e);
	if(group) {
		if(group->component) group->component->remove_class("highlighted");
		if(group->component_open) group->component_open->remove_class("highlighted");
	}
	Group* g = Document::cur()->get_group_at(x,y);
	if(g!=NULL) {
		if(g->component) g->component->add_class("highlighted");
		if(g->component_open) g->component_open->add_class("highlighted");
	}
	group = g;
}

}
