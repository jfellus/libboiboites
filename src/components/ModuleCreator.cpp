/*
 * ModuleCreator.cpp
 *
 *  Created on: 9 oct. 2014
 *      Author: jfellus
 */

#include "ModuleCreator.h"
#include "../widget/ZoomableDrawingArea.h"
#include "../module/Document.h"

ModuleCreator::ModuleCreator() {
	module = 0;
}

void ModuleCreator::create(double x, double y) {
	module = new Module();
	comp = module->component;
	comp->set_pos(x,y);
	end();
}

void ModuleCreator::render(Graphics& g) {
	g.set_color(RGB_RED);
	g.drawPoint(x,y);
}

