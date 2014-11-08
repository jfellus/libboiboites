/*
 * Component.cpp
 *
 *  Created on: 2 oct. 2014
 *      Author: jfellus
 */



#include "../widget/ZoomableDrawingArea.h"
#include "Component.h"
#include "BoundingBox.h"

Component::Component(int layer) {
	this->auto_addToCanvas = true;
	 x=y=0; canvas = 0;
	 selectionBox = 0;
	 this->layer = layer;
	 if(ZoomableDrawingArea::cur()) ZoomableDrawingArea::cur()->add(this);
	 visible = true;
}

Component::Component(bool auto_addToCanvas) {
	this->auto_addToCanvas = auto_addToCanvas;
	 x=y=0; canvas = 0;
	 selectionBox = 0;
	 if(auto_addToCanvas && ZoomableDrawingArea::cur()) ZoomableDrawingArea::cur()->add(this);
	 visible = true;
}

Component::~Component() {
	if(!canvas) return;
	if(auto_addToCanvas) canvas->remove(this);
}

void Component::repaint() {
	if(canvas) canvas->repaint();
}

void Component::select(bool single) {
	if(bSelected) return;
	if(!selectionBox && canvas->selectionRenderingMode==SELECTION_ShowSingleBoundingBox) selectionBox = new BoundingBox(this);
	if(!bSelected) canvas->add_selection(this);
	ISelectable::select();
}

void Component::unselect() {
	if(!bSelected) return;
	if(bSelected) canvas->remove_selection(this);
	ISelectable::unselect();
}

void Component::set_selectable(bool b) {
	if(b) canvas->add_selectable(this);
	else canvas->remove_selectable(this);
}

void Component::draw(Graphics& g) {
	if(!visible) return;

	g.save();
	g.clear_path();
	transform(g);
	if(canvas && canvas->selectionRenderingMode==SELECTION_Colored && is_selected())
		g.map_color(RGB_BLACK, RGB_RED);
	render(g);
		g.unmap_color(RGB_BLACK);
	g.restore();
	if(canvas && canvas->selectionRenderingMode==SELECTION_ShowComponentsBoundingBoxes && is_selected())
		selectionBox->draw(g);
}

std::ostream& operator<<(std::ostream& os, Component* a) {
	a->dump(os);
	return os;
}

bool Component::has_class(const std::string& c) {
	return css_class.find(std::string(" ") + c + " ") != std::string::npos;
}

void Component::add_class(const std::string& c) {
	if(css_class.empty()) css_class = " " + c + " ";
	else if(!has_class(c)) css_class += c + " ";
	if(style) style->update(css_class);
}

void Component::remove_class(const std::string& c) {
	size_t i = css_class.find(" " + c + " ");
	if(i!=std::string::npos) css_class.erase(i, c.length()+1);
	if(css_class==" ") css_class = "";
	if(style) style->update(css_class);
}

