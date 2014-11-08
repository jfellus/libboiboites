/*
 * Module.cpp
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#include "Module.h"
#include "../components/SVGComponent.h"
#include "Document.h"
#include "../widget/ZoomableDrawingArea.h"




ModuleComponent::ModuleComponent(Module* module, const char* component_spec, std::string& text)
: SVGComponent(component_spec), text(text), module(module) {
	style = new ModuleComponentStyle();
	style->update(css_class);
}


///////////////////////////
// MODULE IMPLEMENTATION //
///////////////////////////

Module::Module() {
	component = NULL;
	if(Document::cur()) Document::cur()->add_module(this);
}

Module::~Module() {
	unselect();
	if(parent!=NULL) {
		Group* g = parent;
		g->remove(this);
		if(g->children.size()==0 && !g->bDeleted) delete g;
	}
	Document::cur()->remove_module(this);
	delete component;
}

void Module::create_component(const char* component_spec) {
	component = new ModuleComponent(this, component_spec, text);
	component->set_selectable();
	component->set_user_data("Module", this);
	component->add_selection_listener(this);
	visible = true;
}

bool Module::is_descendant(Module* m) {
	Group* g = dynamic_cast<Group*>(m);
	if(!g) return false;
	if(parent==NULL) return false;
	else if(parent==g) return true;
	return parent->is_descendant(g);
}

bool Module::is_ancestor(Module* m) {
	return m->is_descendant(this);
}



std::ostream& operator<<(std::ostream& os, Module* a) {
	if(!a) os << "(null)";
	else a->dump(os);
	return os;
}

void Module::select() {
	if(has_selected_ancestor()) {unselect(); return;}
	if(!bSelected) {
		component->select(false);
		ISelectable::select();
	}
	bSelected = true;
}

void Module::on_selection_event(ISelectable* s) {
	if(s!=component) return;
	if(s->bSelected && has_selected_ancestor()) {unselect(); return;}
	if(s->bSelected) ISelectable::select(); else ISelectable::unselect();
}

bool Module::has_selected_ancestor() {
	if(!parent) return false;
	if(parent->bSelected) return true;
	return parent->has_selected_ancestor();
}



//////////////////////
// MODULE COMPONENT //
//////////////////////

void ModuleComponent::render(Graphics& g) {
	ModuleComponentStyle* style = (ModuleComponentStyle*)this->style;

	double cursize = 0;
	for(uint i=0; i<style->glows.size(); i++) {
		cursize += style->glows[i].size;
		g.circle(get_bounds().at_origin().augment(cursize/canvas->get_zoom()));
		if(i==0) {
			g.fill_alpha(style->glows[i].color, style->glows[i].alpha);
		} else {
			g.stroke_alpha(style->glows[i].color, style->glows[i].size/canvas->get_zoom(), style->glows[i].alpha);
		}

		// Blur last
		if(i==style->glows.size()-1 && style->bPretty) {
			cursize ++;
			float alpha = style->glows[i].alpha/2;
			while(alpha > 0.01) {
				alpha *= 0.8;
				g.circle(get_bounds().at_origin().augment(cursize/canvas->get_zoom()));
				g.stroke_alpha(style->glows[i].color, 4/canvas->get_zoom(), alpha);
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
