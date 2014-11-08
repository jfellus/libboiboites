/*
 * Body.cpp
 *
 *  Created on: 29 oct. 2014
 *      Author: jfellus
 */

#include "Body.h"
#include "DynEngine.h"

void Body::iteration() {
	compute_acc();
	speed += acc;
	override_speed(speed);
	pos += speed;
	if(engine->shakeQty!=0) pos += Vector2D::rand(engine->shakeQty);
	update_pos();
}
