/*
 * Selectable.h
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#ifndef SELECTABLE_H_
#define SELECTABLE_H_

#include "ISelectionListener.h"

class ISelectable {
public:
	bool bSelected = false;
	std::vector<ISelectionListener*> selectionListeners;
public:
	ISelectable() {}
	virtual ~ISelectable() {}

	bool is_selected() {return bSelected;}

	void add_selection_listener(ISelectionListener* sl) {selectionListeners.push_back(sl);}

	virtual void select(bool single = true) {
		if(bSelected) return;
		bSelected = true;
		for(uint i=0; i<selectionListeners.size(); i++) {
			selectionListeners[i]->on_selection_event(this);
		}
	}

	virtual void unselect() {
		if(!bSelected) return;
		bSelected = false;
		for(uint i=0; i<selectionListeners.size(); i++) {
			selectionListeners[i]->on_selection_event(this);
		}
	}

	virtual void on_dbl_click(GdkEventButton* e) {
		for(uint i=0; i<selectionListeners.size(); i++) {
			selectionListeners[i]->on_dbl_click(this, e);
		}
	}

	virtual void toggle_select() {
		if(bSelected) unselect();
		else select();
	}

};

#endif /* SELECTABLE_H_ */
