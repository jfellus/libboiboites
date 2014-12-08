/*
 * LinkReconnectCreator.cpp
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#include "LinkReconnectCreator.h"
#include <ZoomableDrawingArea.h>
#include <Component.h>
#include "../commands/CommandLinkReconnect.h"


namespace libboiboites {

LinkReconnectCreator::LinkReconnectCreator(Link* link) {
	this->link = link;
	link->hide();
	dummy = new DummyComponent();
}

void LinkReconnectCreator::start(ZoomableDrawingArea* a) {
	Creator::start(a);
	Vector2D src = link->src->component->center();
	Vector2D dst = link->dst->component->center();
	bModifySrc = (src.dist(Vector2D(x,y)) < dst.dist(Vector2D(x,y)));
	if(bModifySrc) link->component->connect(dummy,link->dst->component); else link->component->connect(link->src->component, dummy);
	dummy->x = x;
	dummy->y = y;
	link->select();
}

void LinkReconnectCreator::end() {
	if(dummy) {delete dummy; dummy = 0;}
	link->connect(link->src, link->dst);
	link->show();
	canvas->update_layers();
	Creator::end();
}

void LinkReconnectCreator::create(double x, double y) {
	if(bModifySrc) {
		Component* c = canvas->get_selectable_component_at(x,y);
		if(!c) return;
		Module* m = (Module*) c->get_user_data("Module");
		if(m) (new CommandLinkReconnect(link, m, link->dst))->execute();
		end();
	} else {
		Component* c = canvas->get_selectable_component_at(x,y);
		if(!c) return;
		Module* m = (Module*) c->get_user_data("Module");
		if(m) (new CommandLinkReconnect(link, link->src, m))->execute();
		end();
	}
}

void LinkReconnectCreator::render(Graphics& g) {
	dummy->x = x;
	dummy->y = y;

	link->component->render(g);
}

void LinkReconnectCreator::on_mouse_move(GdkEventMotion* e) {
	Creator::on_mouse_move(e);
	canvas->repaint();
}

}
