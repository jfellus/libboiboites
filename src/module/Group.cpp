/*
 *  * Group.cpp
 *
 *  Created on: 20 oct. 2014
 *      Author: jfellus
 */



#include "Group.h"
#include "Document.h"
#include "../widget/ZoomableDrawingArea.h"
#include "../workbench/Workbench.h"
#include "../components/SVGComponent.h"
#include "GroupComponent.h"











////////////////////////////////
// GROUP CLASS IMPLEMENTATION //
////////////////////////////////


Group::Group() {
	text = "new_group";

	set_property("timescale", "no"); // TODO : MOVE TO promethe-specific stuff
	set_property("name", text);

	add_properties_listener(this);
}

Group::~Group() {
	bDeleted = true;
	open();
	while(!children.empty()) delete children[0];
	if(component_open) {delete component_open; component_open = 0;}
}

void Group::ungroup() {
	open();
	for(uint i=0; i<children.size(); i++) {
		children[i]->parent = NULL;
		if(parent) parent->add(children[i]);
	}
	children.clear();
	delete this;
}

void Group::create_component(const char* component_spec) {
	component = new GroupClosedComponent(this, component_spec);
	component->set_selectable();
	component->set_user_data("Module", this);
	component->add_selection_listener(this);
	component->layer = 0.5;
	visible = true;
}


void Group::add(Module* m) {
	if(!m) throw "Null module";

	if(m->parent) {	vector_remove(m->parent->children, m);	}
	Group* mg = dynamic_cast<Group*>(m);
	if(mg) mg->close();
	children.push_back(m);
	m->parent = this;
	Vector2D c = center();
	component->set_pos(c.x, c.y);
	if(component) m->set_layer(component->layer + 1);
}

void Group::remove(Module* m) {
	vector_remove(children, m);
	m->parent = NULL;
	//if(parent) parent->add(m);
}

void Group::open() {
	for(uint i=0; i<children.size(); i++) children[i]->show();
	for(uint i=0; i<Document::cur()->links.size(); i++) {
		Link* l = Document::cur()->links[i];
		if(l->src->parent == this && l->dst->parent == this) l->show();
		else if(l->src->parent == this) {l->component->src = l->src->component; l->show();}
		else if(l->dst->parent == this) {l->component->dst = l->dst->component; l->show();}
	}
	component->hide();
	component_open->show();
	opened = true;
	Workbench::cur()->unselect_all();
	component_open->select(true);
}

void Group::close() {
	Rectangle r = get_children_bounds();
	for(uint i=0; i<children.size(); i++) children[i]->hide();
	opened = false;
	for(uint i=0; i<Document::cur()->links.size(); i++) {
		Link* l = Document::cur()->links[i];
		if(l->src->is_descendant(this) && l->dst->is_descendant(this)) l->hide();
		else if(l->src->is_descendant(this)) {l->component->src = component; l->show();}
		else if(l->dst->is_descendant(this)) {l->component->dst = component; l->show();}
	}
	component->show();
	component_open->hide();
	component->center(r.center());
	visible = true;
}

void Group::translate(double dx, double dy) {
	if(!translating) {
		translating = true;
		for(uint i=0; i<children.size(); i++) {
			children[i]->translate(dx, dy);
		}
		component->set_pos(component->x + dx, component->y + dy);
		translating = false;
	}
}

void Group::hide() {
	component->hide();
	component_open->hide();
	for(uint i=0; i<children.size(); i++) children[i]->hide();
	visible = false;
}

void Group::show() {
	if(opened) {
		open();
		for(uint i=0; i<children.size(); i++) children[i]->show();
	} else {
		close();
	}
	visible = true;
}

Rectangle Group::get_bounds() {
	if(opened) {
		Rectangle r = get_children_bounds();
		Rectangle r2 = component_open->get_handle_bounds(r);
		r.add(r2); r.augment(370);
		return r;
	}
	else return component->get_bounds();
}

void Group::on_dbl_click(ISelectable* s, GdkEventButton* e) {
	if(dynamic_cast<GroupOpenComponent*>(s)==component_open) {
		close();
		Workbench::cur()->unselect_all();
		component->select(true);
	}
	else Module::on_dbl_click(s,e);
}

void Group::on_selection_event(ISelectable* s) {
	if(dynamic_cast<GroupOpenComponent*>(s)==component_open) {
		if(s->bSelected && has_selected_ancestor()) {unselect(); return;}
		if(s->bSelected) {
			for(uint i=0; i<children.size(); i++) children[i]->unselect();
			for(uint i=0; i<Document::cur()->links.size(); i++) {
				Link* l = Document::cur()->links[i];
				if(l->src->is_descendant(this) || l->dst->is_descendant(this)) l->unselect();
			}
			ISelectable::select();
		} else ISelectable::unselect();
	}
	Module::on_selection_event(s);
}

void Group::select() {
	component_open->select(false);
	Module::select();
}

void Group::unselect() {
	component_open->unselect();
	for(uint i=0; i<children.size(); i++) children[i]->unselect();
	Module::unselect();
}


void Group::toggle_class(const std::string& cls) {
	Module::toggle_class(cls);
	if(component_open) component_open->toggle_class(cls);
	if(CSSDefinitions::is_recursive(cls)) {
		for(uint i=0; i<children.size(); i++) children[i]->toggle_class(cls);
	}
}
void Group::add_class(const std::string& cls) {
	Module::add_class(cls);
	if(component_open) component_open->add_class(cls);
	if(CSSDefinitions::is_recursive(cls)) {
		for(uint i=0; i<children.size(); i++) children[i]->add_class(cls);
	}
}
void Group::remove_class(const std::string& cls) {
	Module::remove_class(cls);
	if(component_open) component_open->remove_class(cls);
	if(CSSDefinitions::is_recursive(cls)) {
		for(uint i=0; i<children.size(); i++) children[i]->remove_class(cls);
	}
}

void Group::set_layer(int l) {
	Module::set_layer(l);
	component_open->layer = component->layer + 0.5;
	for(uint i=0; i<children.size(); i++) {
		children[i]->set_layer(component->layer + 1);
	}
}

void Group::realize() {
	create_component("group_closed");
	component->hide();

	component_open = new GroupOpenComponent(this);
	component_open->add_selection_listener(this);
	component_open->layer = -0.5;
}

