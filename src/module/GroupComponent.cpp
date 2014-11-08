/*
 * GroupComponent.cpp
 *
 *  Created on: 31 oct. 2014
 *      Author: jfellus
 */

#include "GroupComponent.h"
#include "../widget/ZoomableDrawingArea.h"


void GroupOpenComponent::render(Graphics& g) {
	GroupOpenComponentStyle* style = (GroupOpenComponentStyle*)this->style;
	Rectangle r = get_bounds();
	Rectangle rhandle = get_handle_bounds(r);
	if(r.is_empty()) return;

	bool bSelected = this->bSelected;

	if(bSelected) {
		g.set_color(style->bSelectedColor ? style->selected_color : RGB_RED);
	} else {
		g.set_color(style->color);
	}

	if(style->dashed) g.dash(style->dashed);
	if(style->bRounded) g.drawRoundedRect(r);
	else g.drawRect(r);

	if(style->bFillColor) {
		if(style->bRounded) g.rounded_rectangle(r);
		else g.rectangle(r);
		if(bSelected) {
			g.fill_alpha(style->bSelectedColor ? style->selected_color : RGB_RED, style->alpha);
		} else {
			g.fill_alpha(style->fill_color, style->alpha);
		}
	}

	if(style->bFillHandleColor) {
		g.rectangle(rhandle);
		if(bSelected) {
			g.fill_alpha(style->bSelectedColor ? style->selected_color : RGB_RED, 0.8);
		} else {
			g.fill_alpha(style->handle_fill_color, 0.8);
		}
	}

	if(!group->text.empty()) {
		if(style->bTextColor) {	g.set_color(style->text_color); }
		g.set_font(style->font_size, style->font);
		Rectangle rr = g.text_extents(group->text);
		float f = rr.w/r.w;
		if(f>0.7) {
			g.set_font(style->font_size / f *0.7, style->font);
		}
		g.text(group->text, rhandle);
	}

}
