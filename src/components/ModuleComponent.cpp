/*
 * ModuleComponent.cpp
 *
 *  Created on: 9 nov. 2014
 *      Author: jfellus
 */

#include "ModuleComponent.h"
#include <ZoomableDrawingArea.h>


namespace libboiboites {

static int display_mode = DISPLAY_INFO_ON_HOVER;
void ModuleComponent::set_display_mode(int dm) {
	display_mode = dm;
	ZoomableDrawingArea::cur()->repaint();
}
void ModuleComponent::toggle_display_mode() {
	display_mode = !display_mode;
	ZoomableDrawingArea::cur()->repaint();
}




ModuleComponent::ModuleComponent(Module* module, const char* component_spec, std::string& text, std::string& text2)
: SVGComponent(component_spec), text(text), text2(text2), module(module) {
	ready = false;
	style = new ModuleComponentStyle();
	style->update(css_class);
	ready = true;
}


//////////////////////
// MODULE COMPONENT //
//////////////////////

void ModuleComponent::render_glow(Graphics& g) {
	ModuleComponentStyle* style = (ModuleComponentStyle*)this->style;
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
}

void ModuleComponent::render(Graphics& g) {
	ModuleComponentStyle* style = (ModuleComponentStyle*)this->style;
	g.set_opacity(style->opacity);
	g.scale(_scale);

	render_glow(g);

	g.fill(RGB_RED);

	SVGComponent::render(g);

	render_text_1(g);
	if((display_mode & DISPLAY_INFO_ALWAYS) ||  has_class("hover")) render_text_2(g);
}

void ModuleComponent::render_text_1(Graphics& g) {
	ModuleComponentStyle* style = (ModuleComponentStyle*)this->style;
	Rectangle r = bounds;

	if(!style->bNotext) {
		g.set_color(style->text_color);
		g.set_font(style->font_size, style->font, style->font_style);
		if(style->flags & MODULECOMPONENTSTYLE_BOTTOM) { r.y += r.h; r.h = g.text_extents(text).h/2; r.y += 8;}
		if(!text2.empty()) r.y += 13;
		g.text(text, r);
	}

	g.set_color(style->text2_color);
	g.set_font(style->text2_font_size, style->text2_font, style->text2_font_style);
	r = bounds;
	if(style->flags & MODULECOMPONENTSTYLE_BOTTOM) { r.y += r.h; r.h = g.text_extents(text).h/2; r.y += 8;}
	g.text(text2, r);
}

void ModuleComponent::render_text_2(Graphics& g) {
	ModuleComponentStyle* style = (ModuleComponentStyle*)this->style;
	g.set_color(style->text2_color);
	g.set_font(style->text2_font_size, style->text2_font, style->text2_font_style);
	render_infos(g, !(display_mode & DISPLAY_INFO_ALWAYS));
}

void ModuleComponent::render_infos(Graphics& g, bool hover) {
}

void ModuleComponent::scale(float ds) { _scale*=ds; canvas->repaint(); }


}
