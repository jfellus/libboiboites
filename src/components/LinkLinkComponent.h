/*
 * LinkLinkComponent.h
 *
 *  Created on: 9 nov. 2014
 *      Author: jfellus
 */

#ifndef LINKLINKCOMPONENT_H_
#define LINKLINKCOMPONENT_H_

#include <style/IStyle.h>
#include <graphics/Color.h>
#include <components/LinkComponent.h>
#include "../module/Link.h"

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
	float opacity = 1;
	bool noglows = false;

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
		opacity = 1;
		noglows = false;
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
			else if(e->property=="noglow") noglows = (e->value=="true");
			else if(e->property=="pretty") fromString(e->value, bPretty);
			else if(e->property=="opacity") fromString(e->value, opacity);
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
	float _scale = 1;
public:
	LinkLinkComponent(Link* l, Component* src, Component* dst, std::string& text);
	virtual ~LinkLinkComponent() {}
public:
	virtual void render(Graphics& g);

	void scale(float s);
};


#endif /* LINKLINKCOMPONENT_H_ */
