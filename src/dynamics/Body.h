/*
 * Body.h
 *
 *  Created on: 29 oct. 2014
 *      Author: jfellus
 */

#ifndef BODY_H_
#define BODY_H_

#include "../util/utils.h"
#include "../util/geom.h"

class DynEngine;

class Body {
public:
	Vector2D pos;
	Vector2D speed;
	Vector2D acc;
	double mass = 1;
	DynEngine* engine = NULL;
public:
	Body(double mass = 1) : mass(mass) {}
	virtual ~Body() {}

	virtual void update_pos() {}
	virtual Vector2D forces() { return Vector2D(0,0); }

	void compute_acc() {
		Vector2D F = forces();
		acc = F / mass;
	}

	virtual void override_speed(Vector2D& speed) {}

	virtual void iteration();
};

#endif /* BODY_H_ */
