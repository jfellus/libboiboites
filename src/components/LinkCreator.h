/*
 * LinkCreator.h
 *
 *  Created on: 6 oct. 2014
 *      Author: jfellus
 */

#ifndef LINKCREATOR_H_
#define LINKCREATOR_H_


#include "Creator.h"
#include "../module/Link.h"
#include "../module/Document.h"

class LinkComponent;

class LinkCreator : public Creator {
protected:
	Module *src=0, *dst=0;
	Link* link = 0;
public:
	LinkCreator();
	virtual ~LinkCreator() {}

	virtual void create(double x, double y);

	virtual void render(Graphics& g);
};

#endif /* LINKCREATOR_H_ */
