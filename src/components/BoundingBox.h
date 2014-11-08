
#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_

class BoundingBox;

#include "Component.h"
#include "../graphics/svg.h"
#include <vector>

class BoundingBox : public Component {
protected:
	std::vector<Component*>* selection;
	Rectangle bounds;
	Component* component = 0;
public:
	BoundingBox() : Component(false) {selection = 0;}
	BoundingBox(std::vector<Component*>* sel) : Component(false) {set_bounds(sel);}
	BoundingBox(Component* c) : Component(false) {set_bounds(c);}
	BoundingBox(const Rectangle& r) : Component(false) {set_bounds(r);}
	virtual ~BoundingBox() {}


	void set_bounds(const Rectangle& r) {
		selection = 0;
		bounds = r;
		component = 0;
	}

	void set_bounds(std::vector<Component*>* selection) {
		this->selection = selection;
		bounds = Rectangle();
		component = 0;
	}

	void set_bounds(Component* c) {
		component = c;
		bounds = Rectangle();
		selection = 0;
	}


	virtual Rectangle get_bounds() {
		if(!selection) {
			if(!component) return bounds;
			else return component->get_bounds();
		}
		if(selection->empty()) return Rectangle();
		Rectangle r = (*selection)[0]->get_bounds();
		for(uint i=1; i<selection->size(); i++) {
			Component* c = (*selection)[i];
			r.add(c->get_bounds());
		}
		return r;
	}

	virtual void render(Graphics& g) {
		Rectangle r = get_bounds();
		if(r.is_empty()) return;
		g.set_color(RGB(0.5,0.5,1));
		g.drawRect(r);
		g.drawHandle(r.x,r.y);
		g.drawHandle(r.x+r.w,r.y);
		g.drawHandle(r.x,r.y+r.h);
		g.drawHandle(r.x+r.w,r.y+r.h);
	}

	virtual void dump(std::ostream& os) { os << "BoundingBox(" << component << ")";}
};



#endif /* BOUNDING_BOX_H_ */
