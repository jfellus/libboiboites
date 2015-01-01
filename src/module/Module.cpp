/*
 * Module.cpp
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#include "Module.h"
#include <components/SVGComponent.h>
#include "Document.h"
#include <ZoomableDrawingArea.h>


namespace libboiboites {

///////////////////////////
// MODULE IMPLEMENTATION //
///////////////////////////

Module::Module() {
	component = NULL;
	if(Document::cur()) Document::cur()->add_module(this);
	bAttached = true;
}

Module::~Module() {
	unselect();
	if(parent!=NULL) {
		Group* g = parent;
		g->remove(this);
		if(g->children.size()==0 && !g->bDeleted) delete g;
	}
	Document::cur()->remove_module(this);
	delete component;
}

void Module::create_component(const char* component_spec) {
	this->component_spec = component_spec;
	component = new ModuleComponent(this, component_spec, text, text2);
	component->set_selectable();
	component->set_user_data("Module", this);
	component->add_selection_listener(this);
	visible = true;
}

bool Module::is_descendant(Module* m) {
	Group* g = dynamic_cast<Group*>(m);
	if(!g) return false;
	if(parent==NULL) return false;
	else if(parent==g) return true;
	return parent->is_descendant(g);
}

bool Module::is_ancestor(Module* m) {
	return m->is_descendant(this);
}



std::ostream& operator<<(std::ostream& os, Module* a) {
	if(!a) os << "(null)";
	else a->dump(os);
	return os;
}

void Module::select() {
	//if(has_selected_ancestor()) {unselect(); return;}
	if(bSelected) return;
	if(component) component->select(false);
	ISelectable::select();
}

void Module::unselect() {
	if(!bSelected) return;
	if(component)  component->unselect();
	ISelectable::unselect();
}

void Module::on_selection_event(ISelectable* s) {
	if(s!=component) return;
	//if(s->bSelected && has_selected_ancestor()) {unselect(); ISelectable::unselect(); return;}
	if(s->bSelected)
		{ ISelectable::select(false); select();}
	else
		{ ISelectable::unselect();  unselect();}
}

bool Module::has_selected_ancestor() {
	if(!parent) return false;
	if(parent->bSelected) return true;
	return parent->has_selected_ancestor();
}



Module* Module::copy() {
	Module* m = new Module();
	m->text = text;
	m->text2 = text2;
	m->component_spec = component_spec;
	return m;
}


void Module::attach() {
	if(bAttached) return;
	bAttached = true;
	bDetachedSlave = false;
	if(parent) {
		parent->add(this);
		parent->attach();
	}
	component->show();
	Document* doc = Document::cur();
	doc->modules.push_back(this);
	for(uint j=0; j<in_links.size(); j++)
		if(in_links[j]->bDetachedSlave && in_links[j]->src->bAttached) in_links[j]->attach();
	for(uint j=0; j<out_links.size(); j++)
		if(out_links[j]->bDetachedSlave && out_links[j]->dst->bAttached) out_links[j]->attach();
	visible = true;
}

void Module::detach(bool bSlave) {
	if(!bAttached) return;
	bAttached = false;
	bDetachedSlave = bSlave;
	unselect();
	if(parent!=NULL) {
		Group* g = parent;
		if(g->children.size()==1 && g->children[0]==this && !g->bDeleted) g->detach(bSlave);
		g->remove(this);
		parent = g;
	}
	component->hide();
	Document::cur()->remove_module(this, false);
	visible = false;
}

}
