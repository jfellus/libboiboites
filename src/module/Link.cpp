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
	unselect();
	Document::cur()->remove_link(this);
	if(src) vector_remove(src->out_links, this);
	if(dst) vector_remove(dst->in_links, this);
	delete component;
}

void Link::connect(Module* src, Module* dst) {
	if(!src || !dst) { ERROR("Connection error"); return; }
	if(this->src) vector_remove(this->src->out_links, this);
	if(this->dst) vector_remove(this->dst->in_links, this);
	this->src = src; src->out_links.push_back(this);
	this->dst = dst; dst->in_links.push_back(this);
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
	bAttached = true;
}

Link* Link::copy() {
	Link* l = new Link(src, dst);
	l->text = text;
	return l;
}

void Link::attach() {
	if(bAttached) return;
	bAttached = true;
	bDetachedSlave = false;
	if(component) component->show();
	Document::cur()->links.push_back(this);
}

void Link::detach(bool bSlave) {
	if(!bAttached) return;
	bAttached = false;
	bDetachedSlave = bSlave;
	unselect();
	if(component) component->hide();
	Document::cur()->remove_link(this);
}


