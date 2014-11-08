/*
 * ModuleBody.h
 *
 *  Created on: 29 oct. 2014
 *      Author: jfellus
 */

#ifndef MODULEBODY_H_
#define MODULEBODY_H_

#include "Body.h"
#include "../module/Module.h"
#include "DynEngine.h"

class ModuleBody : public Body {
public:
	double f_attraction = 0.00001;
	double f_repulsion = 100000;
	double f_collapse = 0.8;
	Module* module;
	std::vector<Body*> links;
public:
	ModuleBody(Module* module) : module(module) {
		pos.x = module->component->x;
		pos.y = module->component->y;
	}
	virtual ~ModuleBody() {}

	void addLink(Body* l) { links.push_back(l); }

	virtual void update_pos() {
		if(module->component) module->component->set_pos(pos);
	}

	virtual void override_speed(Vector2D& speed) {
		speed *= 0.3;								// FRICTION SAMPLE !!
		//if(pos.y<0 && (pos+speed).y>0) speed *= -1;   // BOUNCING SAMPLE !!
	}


	virtual Vector2D forces() {
		pos.x = module->component->x;
		pos.y = module->component->y;
		Vector2D modules_repulsion;
		Vector2D center;
		for(uint i=0; i<engine->bodies.size(); i++) {
			Body* b = engine->bodies[i];
			center+=b->pos;
			if(b==this) continue;
			if((pos-b->pos).norm() < 3000)
				modules_repulsion += (pos-b->pos).normalize()*(f_repulsion/(pos-b->pos).norm());
			else
				modules_repulsion += (pos-b->pos).normalize()*(f_repulsion/100000/pow((pos-b->pos).norm(),0.2));
		}
		center/=engine->bodies.size();
		Vector2D links_attraction;
		for(uint i=0; i<links.size(); i++) {
			Body* b = links[i];
			links_attraction += (b->pos-pos).normalize()*(f_attraction*pow((b->pos-pos).norm(),2));
		}
		Vector2D acc =
				links_attraction + modules_repulsion +
				(center - pos).normalize()*f_collapse;
		return acc;
	}
};

#endif /* MODULEBODY_H_ */
