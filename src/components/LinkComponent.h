/*
 * LinkComponent.h
 *
 *  Created on: 3 oct. 2014
 *      Author: jfellus
 */

#ifndef LINKCOMPONENT_H_
#define LINKCOMPONENT_H_

#include "Component.h"
#include "../util/geom.h"
#include <vector>
#include "BezierHandle.h"

class LinkComponent : public Component {
public:
	Component *src, *dst;
	Bezier* b;

	BezierHandle *bezierHandle1 = 0, *bezierHandle2 = 0;

private:
	int imoving = -1;

public:
	LinkComponent(Component* src, Component* dst) {
		selectionLayer = -1;
		connect(src, dst);
		b = new Bezier(0,0,0.2,0.2,0.8,0.8,1,1);
	}
	virtual ~LinkComponent() {
		if(bezierHandle1) delete bezierHandle1;
		if(bezierHandle2) delete bezierHandle2;
	}

	void connect(Component* src, Component* dst) {
		this->src = src;
		this->dst = dst;
	}

	virtual void transform(Graphics& g) {} // No transform !

	Bezier bezier_absolute() {
		Vector2D cs = src->center(); Vector2D cd = dst->center();
		return Bezier(cs.x, cs.y, 	cs.x*(1-b->x2) + cd.x*b->x2, cs.y*(1-b->y2) + cd.y*b->y2,
						cs.x*(1-b->x3) + cd.x*b->x3, cs.y*(1-b->y3) + cd.y*b->y3,   cd.x, cd.y);
	}

	virtual Rectangle get_bounds() {
		return bezier_absolute().get_bounds();
	}

	virtual void render(Graphics& g);
	virtual bool hasPoint(double x, double y);

	virtual void select(bool single);
	virtual void unselect();

	virtual void click(double x, double y);
	virtual void translate(double dx, double dy);


	void render_line(Graphics& g, double tickness = 1);
	double render_arrow(Graphics& g, double size = 1);
protected:
	void create_bezier_handles() {
		bezierHandle1 = new BezierHandle(this,0);
		bezierHandle2 = new BezierHandle(this,1);
	}

	virtual void dump(std::ostream& os) { os << "LinkComponent(" << src << " -> " << dst << ")";}
};

#endif /* LINKCOMPONENT_H_ */
