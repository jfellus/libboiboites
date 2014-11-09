/*
 * ModuleComponent.h
 *
 *  Created on: 9 nov. 2014
 *      Author: jfellus
 */

#ifndef MODULECOMPONENT_H_
#define MODULECOMPONENT_H_

#include <style/IStyle.h>
#include <graphics/Color.h>
#include <components/SVGComponent.h>
#include "../module/Module.h"

///////////
// STYLE //
///////////

#define MODULECOMPONENTSTYLE_BOTTOM 0x001

class ModuleComponentStyle : public IStyle {
public:
	std::string font = "Serif";
	uint font_size = 200;
	uint flags = 0;
	int font_style = 0;
	RGB text_color = RGB_BLACK;
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
	ModuleComponentStyle() {}
	virtual ~ModuleComponentStyle(){}

	virtual void default_style() {
		font = "Serif"; font_size = 200; flags = 0;
		glows.clear();
		text_color = RGB_BLACK;
		bPretty = false;
		font_style = 0;
	}

	virtual const char* name() {return "module";}

	virtual void apply_style(CSSStyle* s) {
		for(uint i=0; i<s->items.size(); i++) {
			CSSStyle::Item* e = s->items[i];
			if(e->property=="font") font = e->value;
			else if(e->property=="font-size") fromString(e->value, font_size);
			else if(e->property=="font-style" && e->value=="italic") font_style |= 0x001;
			else if(e->property=="font-weight" && e->value=="bold") font_style |= 0x010;
			else if(e->property=="font-style" && e->value!="italic") font_style &= !(0x001);
			else if(e->property=="font-weight" && e->value!="bold") font_style &= !(0x010);
			else if(e->property=="text-align" && e->value=="bottom") flags |= MODULECOMPONENTSTYLE_BOTTOM;
			else if(e->property=="text-align" && e->value=="middle") flags &= !MODULECOMPONENTSTYLE_BOTTOM;
			else if(e->property=="text-color") text_color = e->value;
			else if(e->property=="glow") glows.push_back(Glow(e->value));
			else if(e->property=="pretty") fromString(e->value, bPretty);
		}
	}
};

//////////////////////
// MODULE COMPONENT //
//////////////////////

class ModuleComponent : public SVGComponent {
public:
	std::string& text;
	Module* module;

public:
	ModuleComponent(Module* module, const char* component_spec, std::string& text);
	virtual ~ModuleComponent() {}

	virtual void render(Graphics& g);
};


#endif /* MODULECOMPONENT_H_ */
