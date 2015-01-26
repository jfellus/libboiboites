/*
 * LinkLinkComponent.cpp
 *
 *  Created on: 9 nov. 2014
 *      Author: jfellus
 */

#include "LinkLinkComponent.h"
#include <ZoomableDrawingArea.h>


namespace libboiboites {

//////////////////////////
// STYLE IMPLEMENTATION //
//////////////////////////


LinkLinkComponent::LinkLinkComponent(Link* l, Component* src, Component* dst, std::string& text) : LinkComponent(src, dst), link(l), text(text) {
	ready = false;
		style = new LinkComponentStyle();
		style->update(css_class);
	ready = true;
}

bool LinkComponentStyle::bText2_force = false;
bool LinkComponentStyle::bText_force = false;
void LinkLinkComponent::render(Graphics& g) {
	LinkComponentStyle* style = (LinkComponentStyle*)this->style;
	if(!style) return;

	g.set_opacity(style->opacity);

	if(!style->glows.empty() && !style->noglows) {
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
			g.stroke_alpha(style->glows[i].color, (i==0? 2:1)*style->glows[i].size/canvas->get_zoom(), style->glows[i].alpha * style->opacity);
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
					g.stroke_alpha(style->glows[i].color, 4/canvas->get_zoom(), alpha * style->opacity);
					cursize += 2;
				}
			}
		}
	}

	g.set_color(style->color);
	g.set_font(style->font_size, style->font, style->font_style);
	if(style->dashed > 0) g.dash(style->dashed);
	render_line(g, link->bSelected ? style->thickness_selected : style->thickness);
	double t1 = render_arrow(g, _scale * (link->bSelected ? style->arrow_size_selected : style->arrow_size ));


	Bezier b = bezier_absolute();
	double t2 = -1;

	if(style->slashes) {
		if(t2==-1) t2 = b.intersect_location(src->get_bounds());
		if(t2==-1) t2 = 0;
		g.draw_slashes(style->slashes,  b.get((t1+t2)/2), b.get((t1+t2)/2 - 0.01));
	}

	g.scale(_scale);

	if(!link->text.empty() || !link->text2.empty()) {
		if(t2==-1) t2 = b.intersect_location(src->get_bounds());
		if(t2==-1) t2 = 0;
		Vector2D p = b.get((t1+t2)/2);
		if(!link->text.empty() && (style->bText || LinkComponentStyle::bText_force)) {
			Rectangle r(p.x, p.y+6, 0,0);
			r.x /= _scale; r.y /= _scale;
			g.text(link->text, r);
		}
		if(!link->text2.empty() && (style->bText2 || LinkComponentStyle::bText2_force)) {
			Rectangle r(p.x, p.y-6, 0,0);
			r.x /= _scale; r.y /= _scale;
			g.set_font(style->font_size_text2, style->font_text2, style->font_style_text2);
			g.text(link->text2, r);
		}
	}
}

void LinkLinkComponent::scale(float s) {_scale *=s; canvas->repaint();}


}
