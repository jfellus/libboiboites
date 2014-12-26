/*
 * GroupComponent.h
 *
 *  Created on: 31 oct. 2014
 *      Author: jfellus
 */

#ifndef GROUPCOMPONENT_H_
#define GROUPCOMPONENT_H_

#include <Component.h>
#include <components/SVGComponent.h>
#include "../module/Group.h"
#include "ModuleComponent.h"


namespace libboiboites {

///////////
// STYLE //
///////////

class GroupOpenComponentStyle : public IStyle {
public:
	std::string font = "Serif";
	uint font_size = 40;

	RGB color = RGB(0.7,0.7,0.7);
	RGB fill_color; bool bFillColor = false;
	RGB handle_fill_color; bool bFillHandleColor = false;
	RGB text_color; bool bTextColor = false;
	RGB selected_color; bool bSelectedColor = false;
	float alpha = 1;
	float opacity = 1;
	float padding = 37;

	int dashed = true;
	bool bRounded = false;

public:
	GroupOpenComponentStyle() {}
	virtual ~GroupOpenComponentStyle(){}

	virtual void default_style() {
		font = "Serif"; font_size = 40;
		color = RGB(0.7,0.7,0.7);
		bFillColor = bFillHandleColor = bTextColor = bSelectedColor = false;
		dashed = 1;
		bRounded = false;
		alpha = opacity = 1;
		padding = 37;
	}

	virtual const char* name() {return "group:open";}

	virtual void apply_style(CSSStyle* s) {
		for(uint i=0; i<s->items.size(); i++) {
			CSSStyle::Item* e = s->items[i];
			if(e->property=="font") font = e->value;
			else if(e->property=="font-size") fromString(e->value, font_size);
			else if(e->property=="selected-color") {bSelectedColor = true;	selected_color = e->value;	}
			else if(e->property=="color") color = e->value;
			else if(e->property=="fill-color") {bFillColor = true; fill_color = e->value; }
			else if(e->property=="handle-fill-color") {bFillHandleColor = true; handle_fill_color = e->value; }
			else if(e->property=="text-color") { bTextColor = true; text_color = e->value; }
			else if(e->property=="border-radius" && e->value!="none") {int v; fromString(e->value, v); if(v) bRounded = true; }
			else if(e->property=="border-radius" && e->value=="none") {bRounded = false; }
			else if(e->property=="dashed") {fromString(e->value, dashed);}
			else if(e->property=="alpha") {fromString(e->value, alpha);}
			else if(e->property=="opacity") {fromString(e->value, opacity);}
			else if(e->property=="padding") {fromString(e->value, padding);}
		}
	}
};






////////////////
// COMPONENTS //
////////////////

class GroupClosedComponent : public ModuleComponent {
public:
	Group* group = NULL;
	bool translating = false;

	class Style : public ModuleComponentStyle {
		virtual const char* name() {return "group:closed";}
	};

public:
	GroupClosedComponent(Group* g, const char* component_spec) : ModuleComponent(g, component_spec, g->text) {
		ready = false;
		style = new Style();
		style->update(css_class);
		this->group = g;
		ready = true;
	}

	virtual void translate(double dx, double dy, bool bFireEvent = false) {
		if(!visible) return;
		if(!translating && !group->translating) {
			translating = true;
			group->translate(dx,dy, bFireEvent);
			translating = false;
		}
		else SVGComponent::translate(dx,dy,bFireEvent);
	}

	virtual ~GroupClosedComponent() {}
};



class GroupOpenComponent : public Component {
public:
	Group* group = NULL;
public:
	bool translating = false;
	Rectangle handle_bounds;

public:
	GroupOpenComponent(Group* g) : Component(-1000) {
		ready = false;
		hide();
		set_selectable();
		this->group = g;
		style = new GroupOpenComponentStyle();
		style->update(css_class);
		ready = true;
	}

	virtual ~GroupOpenComponent() {}

	virtual void render(Graphics& g);

	virtual bool hasPoint(double x, double y) {
		return get_handle_bounds(get_bounds()).contains(x,y);
	}

	virtual void select(bool single) {
		if(single) Component::select(single);
	}

	virtual bool is_in(const Rectangle& r) {return false;}

	virtual bool hasPoint(double x, double y, bool bInside) {
		if(!bInside) return hasPoint(x,y);
		return get_bounds().contains(x,y);
		return false;
	}

	virtual void translate(double dx, double dy, bool bFireEvent = false) {
		if(!translating) {
			translating = true;
			group->translate(dx,dy, bFireEvent);
			translating = false;
		}
	}

	virtual void transform(Graphics& g) {} // NO TRANSFORM !

	virtual void dump(std::ostream& os) { os << "GroupComponent(" << group << ")";}

	virtual Rectangle get_bounds() {
		GroupOpenComponentStyle* style = (GroupOpenComponentStyle*)this->style;
		return group->get_children_bounds(true).augment(style->padding);
	}

	Rectangle get_handle_bounds(Rectangle bounds) {
		bounds.x += bounds.w - handle_bounds.w - 25;
		bounds.y -= handle_bounds.h + 25;
		bounds.w  = handle_bounds.w + 40;
		bounds.h = handle_bounds.h + 25;
		return bounds;
	}


	void compute_handle_bounds();

	void do_compute_handle_bounds();

};



}

#endif /* GROUPCOMPONENT_H_ */
