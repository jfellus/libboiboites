/*
 * ModuleCreator.h
 *
 *  Created on: 9 oct. 2014
 *      Author: jfellus
 */

#ifndef MODULECREATOR_H_
#define MODULECREATOR_H_

#include <creator/Creator.h>
#include "../module/Module.h"
#include "../module/Group.h"

namespace libboiboites {

class ModuleCreator : public Creator {
public:
	Module* module = 0;
	Group* group = NULL;
public:
	ModuleCreator();
	virtual ~ModuleCreator() {}

	virtual void on_mouse_move(GdkEventMotion* e);

	virtual void create(double x, double y);
	virtual void render(Graphics& g);

	virtual void end();
};

}

#endif /* MODULECREATOR_H_ */
