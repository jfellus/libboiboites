/*
 * Document.h
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include "../util/utils.h"
#include "../util/selection/ISelectionListener.h"
#include "Module.h"
#include "Link.h"
#include "Group.h"
#include "GroupComponent.h"

class IModuleSelectionListener {
public:
	IModuleSelectionListener(){}
	virtual ~IModuleSelectionListener(){}
	virtual void on_module_selected(Module* m, bool bSelected) = 0;
};
class ILinkSelectionListener {
public:
	ILinkSelectionListener(){}
	virtual ~ILinkSelectionListener(){}
	virtual void on_link_selected(Link* m, bool bSelected) = 0;
};


class Document : public ISelectionListener, public IPropertiesListener {
public:
	std::vector<Module*> modules;
	std::vector<Link*> links;

	std::vector<IModuleSelectionListener*> module_selection_listeners;
	std::vector<ILinkSelectionListener*> link_selection_listeners;
	std::vector<IPropertiesListener*> propertiesListeners;

	std::vector<Module*> selected_modules;
	std::vector<Link*> selected_links;

	static Document* cur();
public:
	Document();
	virtual ~Document() {}

	Module* add_module(Module* m) {
		VECTOR_ASSERT_UNICITY(modules, m);
		modules.push_back(m);
		m->add_selection_listener(this);
		m->add_properties_listener(this);
		return m;
	}

	Link* add_link(Link* l) {
		VECTOR_ASSERT_UNICITY(links, l);
		links.push_back(l);
		l->add_selection_listener(this);
		l->add_properties_listener(this);
		return l;
	}

	Module* get_module(int i) {return modules[i];}
	Link* get_link(int i) {return links[i];}
	int get_modules_count(int i) {return modules.size();}
	int get_links_count(int i) {return links.size();}

	void remove_module(Module* m) {
		for(uint i=0; i<links.size(); i++) {
			if(links[i]->src==m || links[i]->dst==m) {delete links[i];i--;}
		}
		vector_remove(modules, m);
	}

	void remove_link(Link* l) {
		vector_remove(links, l);
	}

	virtual void on_selection_event(ISelectable* s) {
		Module* m = dynamic_cast<Module*>(s);
		if(m) {
			if(s->bSelected) selected_modules.push_back(m); else vector_remove(selected_modules, m);
			fire_module_selected_event(m,s->bSelected);
		}
		Link* l = dynamic_cast<Link*>(s);
		if(l) {
			if(s->bSelected) selected_links.push_back(l); else vector_remove(selected_links, l);
			fire_link_selected_event(l, s->bSelected);
		}
	}

	virtual void on_dbl_click(ISelectable* s, GdkEventButton* e) {
		Group* g = dynamic_cast<Group*>(s);
		if(g) {
			g->open();
		}
	}

	void add_module_selection_listener(IModuleSelectionListener* l) { module_selection_listeners.push_back(l);}
	void add_link_selection_listener(ILinkSelectionListener* l) { link_selection_listeners.push_back(l);}
	void add_properties_listener(IPropertiesListener* l) { propertiesListeners.push_back(l);}

	void unselect_all();

	void delete_selection() {
		for(uint i=0; i<links.size(); i++) {
			if(links[i]->bSelected) {delete links[i]; i--;}
		}
		for(uint i=0; i<modules.size(); i++) {
			if(modules[i]->bSelected) { delete modules[i]; i--; }
		}
		unselect_all();
	}

	void group_selection() {
		// No group 0 element
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
	}

	void ungroup_selected() {
		for(uint i=0; i<selected_modules.size(); i++) {
			Group* g = dynamic_cast<Group*>(selected_modules[i]);
			if(!g) continue;
			g->ungroup();
		}

		update_links_layers();
	}

	void change_selection_group(Group* g) {
		for(uint i=0; i<selected_modules.size(); i++) {
			if(selected_modules[i]->parent) {
				selected_modules[i]->parent->remove(selected_modules[i]);
			}
			if(g) g->add(selected_modules[i]);
		}
		update_links_layers();
	}

	Group* get_group_at(double x, double y) {
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


	void toggle_selection_tag(int i) {
		std::string cls = TOSTRING("tag_" << i);
		for(uint i=0; i<selected_modules.size(); i++) selected_modules[i]->toggle_class(cls);
		for(uint i=0; i<selected_links.size(); i++) selected_links[i]->toggle_class(cls);
	}

	void add_selection_tag(int i) {
		std::string cls = TOSTRING("tag_" << i);
		for(uint i=0; i<selected_modules.size(); i++) selected_modules[i]->add_class(cls);
		for(uint i=0; i<selected_links.size(); i++) selected_links[i]->add_class(cls);
	}

	void remove_selection_tag(int i) {
		std::string cls = TOSTRING("tag_" << i);
		for(uint i=0; i<selected_modules.size(); i++) selected_modules[i]->remove_class(cls);
		for(uint i=0; i<selected_links.size(); i++) selected_links[i]->remove_class(cls);
	}

	virtual void on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val) {
		for(uint i=0; i<propertiesListeners.size(); i++) propertiesListeners[i]->on_property_change(m, name, val);
	}


	void update_links_layers() {
		for(uint i=0; i<links.size(); i++) {
			Link* l = links[i];
			l->component->layer = MIN(l->src->get_layer(), l->dst->get_layer()) + 0.1;
		}
	}

protected:
	void fire_module_selected_event(Module* m, bool bSelected) {
		for(uint i=0; i<module_selection_listeners.size(); i++)
			module_selection_listeners[i]->on_module_selected(m, bSelected);
	}

	void fire_link_selected_event(Link* l, bool bSelected) {
		for(uint i=0; i<link_selection_listeners.size(); i++)
			link_selection_listeners[i]->on_link_selected(l, bSelected);
	}

	virtual void dump(std::ostream& os) {
		os << "Document (" << modules.size() << " modules & " << links.size() << " links)";
	}


	friend std::ostream& operator<<(std::ostream& os, Document* a);
};

std::ostream& operator<<(std::ostream& os, Document* a);


#endif /* DOCUMENT_H_ */
