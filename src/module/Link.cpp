/*
 * Link.cpp
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#include "Link.h"
#include "Document.h"
#include <ZoomableDrawingArea.h>
#include "../components/LinkLinkComponent.h"




Link::Link(Module* src, Module* dst) {
	connect(src, dst);
}

Link::~Link() {
	Document::cur()->remove_link(this);
	delete component;
}

void Link::connect(Module* src, Module* dst) {
	if(!src || !dst) { ERROR("Connection error"); return; }
	this->src = src;
	this->dst = dst;
	if(!component) {
		component = new LinkLinkComponent(this, src->component, dst->component, text);
		component->set_selectable();
		component->set_user_data("Link", this);
		component->add_selection_listener(this);
		if(Document::cur()) Document::cur()->add_link(this);
	}
	component->src = src->component;
	component->dst = dst->component;
	component->layer = MIN(src->get_layer(), dst->get_layer());
}


