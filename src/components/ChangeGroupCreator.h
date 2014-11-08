/*
 * ChangeGroupCreator.h
 *
 *  Created on: 31 oct. 2014
 *      Author: jfellus
 */

#ifndef CHANGEGROUPCREATOR_H_
#define CHANGEGROUPCREATOR_H_

#include "Creator.h"
#include "../module/Group.h"

class ChangeGroupCreator : public Creator {
public:
	Group* group = NULL;
public:
	ChangeGroupCreator();
	virtual ~ChangeGroupCreator() {}

	virtual void end();
	virtual void create(double x, double y);
	virtual void render(Graphics& g);
	virtual void on_mouse_move(GdkEventMotion* e);

};

#endif /* CHANGEGROUPCREATOR_H_ */
