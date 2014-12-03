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
	float opacity = 1;
	bool noglows = false;
	uint text2_font_size = 100;
	int text2_font_style = 0x001;
	RGB text2_color = RGB(0.5,0.5,0.5);
	std::string text2_font = "Serif";
	bool bNotext = false;


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
		opacity = 1;
		noglows = false;
		text2_color = RGB(0.5,0.5,0.5); text2_font = "Serif"; text2_font_size = 100; text2_font_style = 0x001;
		bNotext = false;
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
			else if(e->property=="text2-font-size") fromString(e->value, text2_font_size);
			else if(e->property=="text2-font-style" && e->value=="italic") text2_font_style |= 0x001;
			else if(e->property=="text2-font-weight" && e->value=="bold") text2_font_style |= 0x010;
			else if(e->property=="text2-font-style" && e->value!="italic") text2_font_style &= !(0x001);
			else if(e->property=="text2-font-weight" && e->value!="bold") text2_font_style &= !(0x010);
			else if(e->property=="text-align" && e->value=="bottom") flags |= MODULECOMPONENTSTYLE_BOTTOM;
			else if(e->property=="text-align" && e->value=="middle") flags &= !MODULECOMPONENTSTYLE_BOTTOM;
			else if(e->property=="text-color") text_color = e->value;
			else if(e->property=="glow") glows.push_back(Glow(e->value));
			else if(e->property=="noglow") noglows = (e->value=="true");
			else if(e->property=="pretty") fromString(e->value, bPretty);
			else if(e->property=="opacity") fromString(e->value, opacity);
			else if(e->property=="text" && e->value=="none") bNotext = true;
		}
	}
};

//////////////////////
// MODULE COMPONENT //
//////////////////////

class ModuleComponent : public SVGComponent {
public:
	std::string& text;
	std::string& text2;
	Module* module;

	float _scale = 1;

public:
	ModuleComponent(Module* module, const char* component_spec, std::string& text, std::string& text2 = *(new std::string));
	virtual ~ModuleComponent() {}

	virtual void render(Graphics& g);

	void scale(float ds);

	virtual Rectangle get_bounds() {
		if(!bounds) compute_bounds();
		return (bounds*(double)_scale) + Vector2D(x,y);
	}
};


#endif /* MODULECOMPONENT_H_ */
