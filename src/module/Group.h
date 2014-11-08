/*
 * Group.h
 *
 *  Created on: 20 oct. 2014
 *      Author: jfellus
 */

#ifndef GROUP_H_
#define GROUP_H_

#include "../util/utils.h"
#include "Module.h"

class GroupOpenComponent;

class Group : public Module, public IPropertiesListener {
public:
	std::vector<Module*> children;
	GroupOpenComponent* component_open = NULL;
	bool opened = false;

public:
	bool translating = false;
	bool bDeleted = false;
public:
	Group();
	virtual ~Group();

	bool is_opened() {return opened;}

	void add(Module* m);
	void remove(Module* m);
	void open();
	void close();
	void ungroup();
	virtual void hide();

	virtual void realize();

	Rectangle get_children_bounds(bool onlyVisible = false) {
		Rectangle r;
		for(uint i=0; i<children.size(); i++) {
			if(!onlyVisible || children[i]->visible) r.add(children[i]->get_bounds());
		}
		return r;
	}

	virtual Rectangle get_bounds();

	virtual void translate(double dx, double dy);

	virtual void show();

	Vector2D center() {
		Vector2D c;
		for(uint i=0; i<children.size(); i++) c+=children[i]->component->center();
		return c/=children.size();
	}

	virtual void select();
	virtual void unselect();

	virtual void toggle_class(const std::string& cls);
	virtual void add_class(const std::string& cls);
	virtual void remove_class(const std::string& cls);


	virtual void set_layer(int l);

	virtual void on_selection_event(ISelectable* s);

	virtual void on_dbl_click(ISelectable* s, GdkEventButton* e);

	virtual void on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val) {
		if(name=="name") { text = val; }
	}

	virtual void dump(std::ostream& os) { os << "Group " << (void*)this << " (" << children.size() << ")"; }
	virtual void create_component(const char* component_spec);
};


#endif /* GROUP_H_ */
