/*
 * ISelectionListener.h
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#ifndef ISELECTIONLISTENER_H_
#define ISELECTIONLISTENER_H_

#include <gdk/gdk.h>

class ISelectable;

class ISelectionListener {
public:
	ISelectionListener() {}
	virtual ~ISelectionListener() {}

	virtual void on_selection_event(ISelectable* s) = 0;
	virtual void on_dbl_click(ISelectable* s, GdkEventButton* e) {};
};

#endif /* ISELECTIONLISTENER_H_ */
