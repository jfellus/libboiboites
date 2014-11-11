/*
 * Document.cpp
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#include "Document.h"
#include "../workbench/Workbench.h"


static Document* _cur = 0;
Document* Document::cur() {	return _cur;}


//////////////////
// CONSTRUCTION //
//////////////////

Document::Document() {
	_cur = this;
}


///////////////
// ACCESSORS //
///////////////

Group* Document::get_group_at(double x, double y) {
	Group* winner = 0;
	for(uint i=0; i<modules.size(); i++) {
		Group* g = dynamic_cast<Group*>(modules[i]);
		if(!g) continue;
		if((g->is_opened() && g->component_open->hasPoint(x,y, true))
				|| (!g->is_opened() && g->component->hasPoint(x,y))) {
			if(winner==0 || winner->is_ancestor(g)) winner = g;
		}
	}
	return winner;
}

Module* Document::add_module(Module* m) {
	VECTOR_ASSERT_UNICITY(modules, m);
	modules.push_back(m);
	m->add_properties_listener(this);
	m->add_selection_listener(this);
	return m;
}

Link* Document::add_link(Link* l) {
	VECTOR_ASSERT_UNICITY(links, l);
	links.push_back(l);
	l->add_properties_listener(this);
	l->add_selection_listener(this);
	return l;
}

void Document::remove_module(Module* m) {
	for(uint i=0; i<links.size(); i++) {
		if(links[i]->src==m || links[i]->dst==m) {delete links[i];i--;}
	}
	vector_remove(modules, m);
}

void Document::remove_link(Link* l) {
	vector_remove(links, l);
}



///////////////
// SELECTION //
///////////////

void Document::unselect_all() {
	unselect_all_modules();
	unselect_all_links();
}

void Document::unselect_all_modules() {
	while(!selected_modules.empty()) { selected_modules[0]->unselect();}
}

void Document::unselect_all_links() {
	while(!selected_links.empty()) { selected_links[0]->unselect(); }
}



/////////////////////
// TRANSFORMATIONS //
/////////////////////


void Document::delete_selection() {
	while(!selected_modules.empty()) {
		Module* m = selected_modules[0];
		m->unselect();
		vector_remove(selected_modules, m);
		vector_remove(modules, m);
		delete m;
	}
	while(!selected_links.empty()) {
		Link* l = selected_links[0];
		l->unselect();
		vector_remove(selected_links, l);
		vector_remove(links, l);
		delete l;
	}
	fire_change_event();
}


void Document::group_selection() {
	// No group with 0 element
	if(selected_modules.size()<1) return;

	Group* g = new Group();
	g->realize();

	// Close any selected group
	for(uint i=0; i<modules.size(); i++) {
		Group* mg = dynamic_cast<Group*>(modules[i]);
		if(mg && mg->visible && mg->bSelected) mg->close();
	}

	// Grouping is only allowed if selected items have a common parent
	Group* parent = NULL; bool parentFound = false;
	for(uint i=0; i<modules.size(); i++) {
		if(modules[i]->visible && modules[i]->bSelected) {
			if(!parentFound) {parent = modules[i]->parent; parentFound = true;}
			else if(modules[i]->parent!=parent) {ERROR("No common parent : CANCEL !"); delete g; return;}
		}
	}

	// Add elements to group
	for(uint i=0; i<modules.size(); i++) {
		if(modules[i]->visible && modules[i]->bSelected) {
			g->add(modules[i]);
		}
	}

	// If finally we only added 0 element, group is cancelled
	if(g->children.size()<1) {delete g; return;}

	if(parent) parent->add(g);
	g->close();

	update_links_layers();
	fire_change_event();
}

void Document::ungroup_selected() {
	for(uint i=0; i<selected_modules.size(); i++) {
		Group* g = dynamic_cast<Group*>(selected_modules[i]);
		if(!g) continue;
		g->ungroup();
	}

	update_links_layers();
	fire_change_event();
}

void Document::change_selection_group(Group* g) {
	for(uint i=0; i<selected_modules.size(); i++) {
		if(selected_modules[i]->parent) {
			selected_modules[i]->parent->remove(selected_modules[i]);
		}
		if(g) g->add(selected_modules[i]);
	}
	update_links_layers();
	fire_change_event();
}



/////////////
// TAGGING //
/////////////

void Document::toggle_selection_tag(int i) {
	std::string cls = TOSTRING("tag_" << i);
	for(uint i=0; i<selected_modules.size(); i++) selected_modules[i]->toggle_class(cls);
	for(uint i=0; i<selected_links.size(); i++) selected_links[i]->toggle_class(cls);
	fire_change_event();
}

void Document::add_selection_tag(int i) {
	std::string cls = TOSTRING("tag_" << i);
	for(uint i=0; i<selected_modules.size(); i++) selected_modules[i]->add_class(cls);
	for(uint i=0; i<selected_links.size(); i++) selected_links[i]->add_class(cls);
	fire_change_event();
}

void Document::remove_selection_tag(int i) {
	std::string cls = TOSTRING("tag_" << i);
	for(uint i=0; i<selected_modules.size(); i++) selected_modules[i]->remove_class(cls);
	for(uint i=0; i<selected_links.size(); i++) selected_links[i]->remove_class(cls);
	fire_change_event();
}



////////////
// EVENTS //
////////////

void Document::on_dbl_click(ISelectable* s, GdkEventButton* e) {
	Group* g = dynamic_cast<Group*>(s);
	if(g) g->open();
}

void Document::on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val) {
	for(uint i=0; i<propertiesListeners.size(); i++) propertiesListeners[i]->on_property_change(m, name, val);
}

void Document::on_selection_event(ISelectable* s) {
	Module* m = dynamic_cast<Module*>(s);
	if(m) {
		if(s->bSelected) selected_modules.push_back(m);
		else vector_remove(selected_modules, m);
	}
	Link* l = dynamic_cast<Link*>(s);
	if(l) {
		if(s->bSelected) selected_links.push_back(l);
		else vector_remove(selected_links, l);
	}
}


///////////////
// INTERNALS //
///////////////

void Document::update_links_layers() {
	for(uint i=0; i<links.size(); i++) {
		Link* l = links[i];
		l->component->layer = MIN(l->src->get_layer(), l->dst->get_layer()) + 0.1;
	}
}

void Document::fire_change_event() {
	for(uint i=0; i<change_listeners.size(); i++)
		change_listeners[i]->on_document_change();
}

