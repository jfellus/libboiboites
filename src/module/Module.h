/*
 * Module.h
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#ifndef MODULE_H_
#define MODULE_H_


#include <Component.h>
#include <vector>
#include "Property.h"
#include <util/selection/ISelectable.h>
#include <util/selection/ISelectionListener.h>
#include <components/SVGComponent.h>


namespace libboiboites {

class Group;
class IPropertiesListener;
class Link;

class Module : public ISelectable, public ISelectionListener, public IPropertiesElement {
public:
	class IChangeListener {
	public:
		IChangeListener() {}
		virtual ~IChangeListener() {}
		virtual void on_module_change(Module* m) = 0;
	};

public:
	Component* component;
	Group* parent = NULL;

	std::string text;
	std::string text2;
	std::string component_spec;


	bool visible = false;
	bool bAttached = false;

	std::vector<Link*> in_links;
	std::vector<Link*> out_links;

	bool bLock = false;

	std::vector<IChangeListener*> changeListeners;

public:
	Module();
	virtual ~Module();

	bool is_descendant(Module* m);
	bool is_ancestor(Module* m);

	virtual Rectangle get_bounds() { return component->get_bounds(); }


	void add_change_listener(IChangeListener* l) {changeListeners.push_back(l);}


	virtual void toggle_class(const std::string& cls) {	component->toggle_class(cls);	}
	virtual void add_class(const std::string& cls) {	component->add_class(cls);	}
	virtual void remove_class(const std::string& cls) {	component->remove_class(cls);	}


	virtual void show() {		component->unlock(); component->show();		visible = true;	}
	virtual void hide() {		unselect();	 component->lock();	component->hide();		visible = false;	}
	virtual void lock() { if(bLock) return; component->lock(); bLock = true;}
	virtual void unlock() { if(!bLock) return; component->unlock(); bLock = false;}


	virtual void translate(double dx, double dy, bool bFireEvent) {
		if(component) component->set_pos(component->x + dx, component->y + dy, bFireEvent);
	}

	virtual void select();
	virtual void unselect();
	virtual void on_selection_event(ISelectable* s);

	virtual void on_dbl_click(ISelectable* s, GdkEventButton* e) {
		if(s!=component) return;
		ISelectable::on_dbl_click(e);
	}

	bool has_selected_ancestor();


	virtual Module* copy();


	virtual void attach();
	virtual void detach(bool bSlave = false);

	virtual void set_layer(int l) { if(component) component->layer = l; }
	float get_layer() {return component->layer;}

	inline Vector2D get_pos() { if(!component) return Vector2D(); return Vector2D(component->x, component->y); }

public:
	virtual void create_component(const char* component_spec);
	friend std::ostream& operator<<(std::ostream& os, Module* a);
	virtual void dump(std::ostream& os) { os << "Module " << (void*)this; }


	virtual void on_parent_change(Group* newparent) {}
	void fire_change_event() {for(uint i=0; i<changeListeners.size(); i++) changeListeners[i]->on_module_change(this);}

public:
	bool bDetachedSlave = false;
};

std::ostream& operator<<(std::ostream& os, Module* a);





}

#endif /* MODULE_H_ */

