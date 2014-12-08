/*
 * LinkReconnectCreator.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef LINKRECONNECTCREATOR_H_
#define LINKRECONNECTCREATOR_H_

#include <creator/Creator.h>
#include "../module/Link.h"

namespace libboiboites {

class LinkReconnectCreator : public Creator {
public:
	Link* link = 0;
	bool bModifySrc = false;
	Component* dummy;
public:
	LinkReconnectCreator(Link* link);
	virtual ~LinkReconnectCreator() {}

	virtual void start(ZoomableDrawingArea* a);
	virtual void create(double x, double y);
	virtual void on_mouse_move(GdkEventMotion* e);

	virtual void end();

	virtual void render(Graphics& g);
};


}

#endif /* LINKRECONNECTCREATOR_H_ */
