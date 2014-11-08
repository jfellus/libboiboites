/*
 * BezierHandle.h
 *
 *  Created on: 13 oct. 2014
 *      Author: jfellus
 */

#ifndef BEZIERHANDLE_H_
#define BEZIERHANDLE_H_

#include "Component.h"

class LinkComponent;

class BezierHandle : public Component {
	LinkComponent* link;
	int i;
public:
	BezierHandle(LinkComponent* l, int i);
	virtual ~BezierHandle();

	virtual void render(Graphics& g);
	virtual void translate(double dx, double dy);

	virtual Rectangle get_bounds();
	virtual void select(bool single);

	void update();
	virtual void transform(Graphics& g) {} // No transform !

	virtual void dump(std::ostream& os) { os << "BezierHandle(" << link << " - " << i << ")";}
};

#endif /* BEZIERHANDLE_H_ */
