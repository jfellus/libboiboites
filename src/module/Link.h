/*
 * Link.h
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#ifndef LINK_H_
#define LINK_H_

#include "../components/LinkComponent.h"
#include "Module.h"
#include "Property.h"


class Link : public ISelectable, ISelectionListener, public IPropertiesElement {
public:
	LinkComponent* component;
	Module *src, *dst;
	std::string text;

public:
	Link() {component = NULL;src=dst=NULL;}
	Link(Module* src, Module* dst);
	virtual ~Link();

	void connect(Module* src, Module* dst);


	virtual void select() {
		if(src->has_selected_ancestor() || dst->has_selected_ancestor()) {unselect(); return;}
		component->select(false);
		ISelectable::select();
	}

	virtual void unselect() {
		component->unselect();
		ISelectable::unselect();
	}

	virtual void on_selection_event(ISelectable* s) {
		if(s!=component) return;
		if(src->has_selected_ancestor() || dst->has_selected_ancestor()) {unselect(); return;}
		if(s->bSelected) ISelectable::select(); else ISelectable::unselect();
	}

	virtual void toggle_class(const std::string& cls) {	component->toggle_class(cls);	}
	virtual void add_class(const std::string& cls) {	component->add_class(cls);	}
	virtual void remove_class(const std::string& cls) {	component->remove_class(cls);	}

	virtual void show() {component->show();}
	virtual void hide() {unselect(); component->hide();}
};




class LinkLinkComponent;


///////////
// STYLE //
///////////

class LinkComponentStyle : public IStyle {
public:
	RGB color = RGB_BLACK;
	uint font_size = 200;
	std::string font = "Serif";
	int font_style = 0;
	int slashes = 0;
	int dashed = 0;
	bool bPretty = false;

	class Glow {
	public:
		double size = 0; RGB color = RGB_YELLOW; float alpha = 0.5;
		Glow(double size, const RGB& color, float alpha) : size(size), color(color), alpha(alpha) {}
		Glow(std::string& str) {
			std::istringstream iss(str);
			std::string scolor;
			iss >> size;	iss >> scolor; iss >> alpha;
			color = scolor;
		}
	};
	std::vector<Glow> glows;

public:
	LinkComponentStyle() {}
	virtual ~LinkComponentStyle(){}

	virtual void default_style() {
		color  = RGB_BLACK;
		font_size = 200; font_style = 0;
		font = "Serif";
		slashes = dashed = 0;
		glows.clear();
		bPretty = false;
	}

	virtual const char* name() {return "link";}

	virtual void apply_style(CSSStyle* s) {
		for(uint i=0; i<s->items.size(); i++) {
			CSSStyle::Item* e = s->items[i];
			if(e->property=="font") font = e->value;
			else if(e->property=="font-size") fromString(e->value, font_size);
			else if(e->property=="font-style" && e->value=="italic") font_style |= 0x001;
			else if(e->property=="font-weight" && e->value=="bold") font_style |= 0x010;
			else if(e->property=="font-style" && e->value!="italic") font_style &= !(0x001);
			else if(e->property=="font-weight" && e->value!="bold") font_style &= !(0x010);
			else if(e->property=="color") color = e->value;
			else if(e->property=="slashes") fromString(e->value, slashes);
			else if(e->property=="dashed") fromString(e->value, dashed);
			else if(e->property=="glow") glows.push_back(Glow(e->value));
			else if(e->property=="pretty") fromString(e->value, bPretty);
		}
	}
};



////////////////////
// LINK COMPONENT //
////////////////////

class LinkLinkComponent : public LinkComponent {
public:
	Link* link;
	int dashed = 0;
	std::string& text;
public:
	LinkLinkComponent(Link* l, Component* src, Component* dst, std::string& text);
	virtual ~LinkLinkComponent() {}
public:
	virtual void render(Graphics& g);
};


#endif /* LINK_H_ */
