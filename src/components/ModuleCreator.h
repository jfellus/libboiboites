/*
 * ModuleCreator.h
 *
 *  Created on: 9 oct. 2014
 *      Author: jfellus
 */

#ifndef MODULECREATOR_H_
#define MODULECREATOR_H_

#include "Creator.h"
#include "../module/Module.h"

class ModuleCreator : public Creator {
public:
	Module* module = 0;
public:
	ModuleCreator();
	virtual ~ModuleCreator() {}

	virtual void create(double x, double y);
	virtual void render(Graphics& g);
};

#endif /* MODULECREATOR_H_ */
