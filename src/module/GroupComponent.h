/*
 * GroupComponent.h
 *
 *  Created on: 31 oct. 2014
 *      Author: jfellus
 */

#ifndef GROUPCOMPONENT_H_
#define GROUPCOMPONENT_H_

#include "../components/Component.h"
#include "../components/SVGComponent.h"
#include "Group.h"

///////////
// STYLE //
///////////

class GroupOpenComponentStyle : public IStyle {
public:
	std::string font = "Serif";
	uint font_size = 400;

	RGB color = RGB(0.7,0.7,0.7);
	RGB fill_color; bool bFillColor = false;
	RGB handle_fill_color; bool bFillHandleColor = false;
	RGB text_color; bool bTextColor = false;
	RGB selected_color; bool bSelectedColor = false;
	float alpha = 1;

	int dashed = true;
	bool bRounded = false;

public:
	GroupOpenComponentStyle() {}
	virtual ~GroupOpenComponentStyle(){}

	virtual void default_style() {
		font = "Serif"; font_size = 400;
		color = RGB(0.7,0.7,0.7);
		bFillColor = bFillHandleColor = bTextColor = bSelectedColor = false;
		dashed = 1;
		bRounded = false;
		alpha = 1;
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
		style = new Style();
		style->update(css_class);
		this->group = g;
	}

	virtual void translate(double dx, double dy) {
		if(!visible) return;
		if(!translating && !group->translating) {
			translating = true;
			group->translate(dx,dy);
			translating = false;
		}
		else SVGComponent::translate(dx,dy);
	}

	virtual ~GroupClosedComponent() {}
};



class GroupOpenComponent : public Component {
public:
	Group* group = NULL;
public:
	bool translating = false;
public:
	GroupOpenComponent(Group* g) : Component(-1000) {
		hide();
		set_selectable();
		this->group = g;
		style = new GroupOpenComponentStyle();
		style->update(css_class);
	}

	virtual ~GroupOpenComponent() {}

	virtual void render(Graphics& g);

	virtual bool hasPoint(double x, double y) {
		return get_handle_bounds(get_bounds()).contains(x,y);
	}

	virtual bool hasPoint(double x, double y, bool bInside) {
		if(!bInside) return hasPoint(x,y);
		return get_bounds().contains(x,y);
	}

	virtual void translate(double dx, double dy) {
		if(!translating) {
			translating = true;
			group->translate(dx,dy);
			translating = false;
		}
	}

	virtual void transform(Graphics& g) {} // NO TRANSFORM !

	virtual void dump(std::ostream& os) { os << "GroupComponent(" << group << ")";}

	virtual Rectangle get_bounds() {
		return group->get_children_bounds(true).augment(400);
	}

	Rectangle get_handle_bounds(Rectangle bounds) {
		GroupOpenComponentStyle* style = (GroupOpenComponentStyle*)this->style;
		Graphics g;
		g.set_font(style->font_size, style->font);
		Rectangle e = g.text_extents(group->text);
		float f = e.w/bounds.w;
		if(f>0.7) {
			g.set_font(style->font_size / f *0.7, style->font);
			e = g.text_extents(group->text);
		}
		bounds.x += bounds.w - e.w - 250;
		bounds.y -= e.h + 250;
		bounds.w  = e.w + 400;
		bounds.h = e.h + 250;
		return bounds;
	}

};


#endif /* GROUPCOMPONENT_H_ */
