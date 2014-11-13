/*
 * Document.cpp
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#include "Document.h"
#include "../workbench/Workbench.h"
#include "../commands/CommandToggleTag.h"
#include "../commands/CommandAddTag.h"
#include "../commands/CommandRemoveTag.h"
#include "../commands/CommandOpenGroup.h"
#include "../commands/CommandCloseGroup.h"
#include "../commands/CommandChangeGroup.h"
#include "../commands/CommandUngroup.h"
#include "../commands/CommandGroup.h"
#include "../commands/CommandDelete.h"

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

void Document::remove_module(Module* m, bool bDeleteConnectedLinks) {
	for(uint i=0; i<m->in_links.size(); i++) {
		if(bDeleteConnectedLinks) {delete m->in_links[i]; i--;}
		else m->in_links[i]->detach(true);
	}
	for(uint i=0; i<m->out_links.size(); i++) {
		if(bDeleteConnectedLinks) {delete m->out_links[i]; i--;}
		else m->out_links[i]->detach(true);
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
	(new CommandDelete(this))->execute();
}


void Document::group_selection() {
	// No group with 0 element
	if(selected_modules.size()<1) return;

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
			else if(modules[i]->parent!=parent) {ERROR("No common parent : CANCEL !"); return;}
		}
	}

	// Add elements to group
	std::list<Module*> modules_to_add;
	for(uint i=0; i<modules.size(); i++) {
		if(modules[i]->visible && modules[i]->bSelected) {
			modules_to_add.push_back(modules[i]);
		}
	}

	// If finally we only added 0 element, group is cancelled
	if(modules_to_add.empty()) return;

	(new CommandGroup(this, parent, modules_to_add))->execute();
}

void Document::ungroup_selected() {
	(new CommandUngroup(this))->execute();
}

void Document::change_selection_group(Group* g) {
	(new CommandChangeGroup(this, g))->execute();
}

void Document::open_group(Group* g) {
	(new CommandOpenGroup(this, g))->execute();
}

void Document::close_group(Group* g) {
	(new CommandCloseGroup(this, g))->execute();
}


/////////////
// TAGGING //
/////////////

void Document::toggle_selection_tag(int i) 	{	(new CommandToggleTag(this, i))->execute(); }
void Document::add_selection_tag(int i) 	{	(new CommandAddTag(this, i))->execute();	}
void Document::remove_selection_tag(int i) 	{	(new CommandRemoveTag(this, i))->execute();	}



////////////
// EVENTS //
////////////

void Document::on_dbl_click(ISelectable* s, GdkEventButton* e) {
	Group* g = dynamic_cast<Group*>(s);
	if(g) open_group(g);
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

