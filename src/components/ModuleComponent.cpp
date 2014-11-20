/*
 * ModuleComponent.cpp
 *
 *  Created on: 9 nov. 2014
 *      Author: jfellus
 */

#include "ModuleComponent.h"
#include <ZoomableDrawingArea.h>



ModuleComponent::ModuleComponent(Module* module, const char* component_spec, std::string& text)
: SVGComponent(component_spec), text(text), module(module) {
	style = new ModuleComponentStyle();
	style->update(css_class);
}


//////////////////////
// MODULE COMPONENT //
//////////////////////

void ModuleComponent::render(Graphics& g) {
	ModuleComponentStyle* style = (ModuleComponentStyle*)this->style;
	g.set_opacity(style->opacity);

	double cursize = 0;
	for(uint i=0; !style->noglows && i<style->glows.size(); i++) {
		cursize += style->glows[i].size;
		g.circle(get_bounds().at_origin().augment(cursize/canvas->get_zoom()));
		if(i==0) {
			g.fill_alpha(style->glows[i].color, style->glows[i].alpha * style->opacity);
		} else {
			g.stroke_alpha(style->glows[i].color, style->glows[i].size/canvas->get_zoom(), style->glows[i].alpha * style->opacity);
		}

		// Blur last
		if(i==style->glows.size()-1 && style->bPretty) {
			cursize ++;
			float alpha = style->glows[i].alpha/2;
			while(alpha > 0.01) {
				alpha *= 0.8;
				g.circle(get_bounds().at_origin().augment(cursize/canvas->get_zoom()));
				g.stroke_alpha(style->glows[i].color, 4/canvas->get_zoom(), alpha * style->opacity);
				cursize += 2;
			}
		}
	}

	SVGComponent::render(g);

	g.set_color(style->text_color);
	g.set_font(style->font_size, style->font, style->font_style);
	Rectangle r = get_bounds().at_origin();
	if(style->flags & MODULECOMPONENTSTYLE_BOTTOM) { r.y += r.h; r.h = g.text_extents(text).h/2; r.y += 150;}
	g.text(text, r);
}
