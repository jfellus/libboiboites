/*
 * Module.h
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#ifndef MODULE_H_
#define MODULE_H_


#include "../components/Component.h"
#include <vector>
#include "Property.h"
#include "../util/selection/ISelectable.h"
#include "../util/selection/ISelectionListener.h"
#include "../components/SVGComponent.h"

class Group;
class IPropertiesListener;

class Module : public ISelectable, public ISelectionListener, public IPropertiesElement {
public:
	Component* component;
	Group* parent = NULL;

	std::string text;


	bool visible = false;
public:
	Module();
	virtual ~Module();

	bool is_descendant(Module* m);
	bool is_ancestor(Module* m);

	virtual Rectangle get_bounds() { return component->get_bounds(); }


	virtual void toggle_class(const std::string& cls) {	component->toggle_class(cls);	}
	virtual void add_class(const std::string& cls) {	component->add_class(cls);	}
	virtual void remove_class(const std::string& cls) {	component->remove_class(cls);	}


	virtual void show() {
		component->show();
		visible = true;
	}

	virtual void hide() {
		unselect();
		component->hide();
		visible = false;
	}


	virtual void translate(double dx, double dy) {
		if(component) component->set_pos(component->x + dx, component->y + dy);
	}

	virtual void select();


	virtual void unselect() {
		if(component) component->unselect();
		if(bSelected) {
			ISelectable::unselect();
		}
		bSelected = false;
	}

	virtual void on_selection_event(ISelectable* s);

	virtual void on_dbl_click(ISelectable* s, GdkEventButton* e) {
		if(s!=component) return;
		ISelectable::on_dbl_click(e);
	}

	bool has_selected_ancestor();

	virtual void set_layer(int l) { if(component) component->layer = l; }
	float get_layer() {return component->layer;}

public:
	virtual void create_component(const char* component_spec);
	friend std::ostream& operator<<(std::ostream& os, Module* a);
	virtual void dump(std::ostream& os) { os << "Module " << (void*)this; }
};

std::ostream& operator<<(std::ostream& os, Module* a);





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



#endif /* MODULE_H_ */
