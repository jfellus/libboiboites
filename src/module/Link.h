/*
 * Link.h
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#ifndef LINK_H_
#define LINK_H_

#include <components/LinkComponent.h>
#include "Module.h"
#include "Property.h"


class Link : public ISelectable, ISelectionListener, public IPropertiesElement {
public:
	LinkComponent* component;
	Module *src, *dst;
	std::string text;

	bool bAttached = false;

	bool bLock = false;


public:
	Link() {component = NULL;src=dst=NULL;}
	Link(Module* src, Module* dst);
	virtual ~Link();

	void connect(Module* src, Module* dst);


	virtual void select() {
		//if(src->has_selected_ancestor() || dst->has_selected_ancestor()) {unselect(); return;}
		component->select(false);
		ISelectable::select();
	}

	virtual void unselect() {
		if(component) component->unselect();
		ISelectable::unselect();
	}

	virtual void on_selection_event(ISelectable* s) {
		if(s!=component) return;
		//if(src->has_selected_ancestor() || dst->has_selected_ancestor()) {unselect(); return;}
		if(s->bSelected) ISelectable::select(); else ISelectable::unselect();
	}

	virtual void attach();
	virtual void detach(bool bSlave = false);

	virtual void toggle_class(const std::string& cls) {	component->toggle_class(cls);	}
	virtual void add_class(const std::string& cls) {	component->add_class(cls);	}
	virtual void remove_class(const std::string& cls) {	component->remove_class(cls);	}

	virtual void show() {component->unlock(); component->show();}
	virtual void hide() {unselect(); component->lock(); component->hide();}
	virtual void lock() { if(bLock) return; component->lock(); bLock = true;}
	virtual void unlock() { if(!bLock) return; component->unlock(); bLock = false;}

public:
	bool bDetachedSlave = false;
};





#endif /* LINK_H_ */
