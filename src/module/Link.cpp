/*
 * Link.cpp
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#include "Link.h"
#include "Document.h"
#include "../widget/ZoomableDrawingArea.h"




Link::Link(Module* src, Module* dst) {
	connect(src, dst);
}

Link::~Link() {
	Document::cur()->remove_link(this);
	delete component;
}

void Link::connect(Module* src, Module* dst) {
	if(!src || !dst) { ERROR("Connection error"); return; }
	component = new LinkLinkComponent(this, src->component, dst->component, text);
	this->src = src;
	this->dst = dst;
	component->set_selectable();
	component->set_user_data("Link", this);
	component->add_selection_listener(this);
	component->layer = MIN(src->get_layer(), dst->get_layer());
	if(Document::cur()) Document::cur()->add_link(this);
}




//////////////////////////
// STYLE IMPLEMENTATION //
//////////////////////////


LinkLinkComponent::LinkLinkComponent(Link* l, Component* src, Component* dst, std::string& text) : LinkComponent(src, dst), link(l), text(text) {
		style = new LinkComponentStyle();
		style->update(css_class);
}


void LinkLinkComponent::render(Graphics& g) {
	LinkComponentStyle* style = (LinkComponentStyle*)this->style;

	if(!style->glows.empty()) {
		Vector2D a = src->center(),b = dst->center();
		Vector2D normal = (b-a).normalize().normal();
		Rectangle r = bezier_absolute().get_bounds(); r.augment(1000/canvas->get_zoom());
		double cursize = 0;
		for(uint i=0; i<style->glows.size(); i++) {
			g.reset_clip();
			Circle2D csrc(src->get_bounds().augment(cursize/canvas->get_zoom()));
			Circle2D cdst(dst->get_bounds().augment(cursize/canvas->get_zoom()));
			g.rectangle(r);
			g.mask_circle(csrc);
			g.rectangle(r);
			g.mask_circle(cdst);
			Vector2D n = normal*(cursize/canvas->get_zoom());
			g.line(Line2D(src->center()-n, dst->center()-n));
			g.line(Line2D(src->center()+n, dst->center()+n));
			g.stroke_alpha(style->glows[i].color, (i==0? 2:1)*style->glows[i].size/canvas->get_zoom(), style->glows[i].alpha);
			cursize += (i==0? 1.5:1)*style->glows[i].size;

			// Blur last
			if(i==style->glows.size()-1  && style->bPretty) {
				cursize -= 0.5*style->glows[i].size;
				float alpha = style->glows[i].alpha/2;
				while(alpha > 0.01) {
					alpha *= 0.8;
					Vector2D n = normal*(cursize/canvas->get_zoom());
					g.line(Line2D(src->center()-n, dst->center()-n));
					g.line(Line2D(src->center()+n, dst->center()+n));
					g.stroke_alpha(style->glows[i].color, 4/canvas->get_zoom(), alpha);
					cursize += 2;
				}
			}
		}
	}

	g.set_color(style->color);
	g.set_font(style->font_size, style->font, style->font_style);
	if(style->dashed > 0) g.dash(style->dashed);
	render_line(g, link->bSelected ? 3 : 1);
	double t1 = render_arrow(g, link->bSelected ? 3.5 : 1.5);


	Bezier b = bezier_absolute();
	double t2 = -1;

	if(style->slashes) {
		if(t2==-1) t2 = b.intersect_location(src->get_bounds());
		if(t2==-1) t2 = 0;
		g.draw_slashes(style->slashes,  b.get((t1+t2)/2), b.get((t1+t2)/2 - 0.01));
	}

	if(!link->text.empty()) {
		if(t2==-1) t2 = b.intersect_location(src->get_bounds());
		if(t2==-1) t2 = 0;
		Vector2D p = b.get((t1+t2)/2);
		Rectangle r(p.x, p.y+60, 0,0);
		g.text(link->text, r);
	}
}

