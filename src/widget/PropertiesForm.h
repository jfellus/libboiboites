/*
 * PropertiesForm.h
 *
 *  Created on: 9 oct. 2014
 *      Author: jfellus
 */

#ifndef PROPERTIESFORM_H_
#define PROPERTIESFORM_H_

#include "Browser.h"
#include "../module/Property.h"
#include "../module/Module.h"
#include "../module/Link.h"
#include <util/utils.h>

class PropertiesForm : public Browser {
public:
	Properties* properties = 0;
	Properties* multiproperties = 0;
	std::vector<Module*>* selected_modules = 0;
	std::vector<Link*>* selected_links = 0;
public:
	PropertiesForm(Properties* properties = NULL) : Browser("properties") {
		this->properties = properties;
		open("src/js/properties_form.html");
	}
	virtual ~PropertiesForm() {

	}

	void update(std::vector<Module*>* selected_modules, std::vector<Link*>* selected_links) {
		this->selected_modules = selected_modules;
		this->selected_links = selected_links;
		if(multiproperties) {delete multiproperties; multiproperties = 0;}

		if(selected_modules->empty() && selected_links->empty()) {
			properties = multiproperties = 0;
			update();
		} else if(selected_modules->size()==1 && selected_links->empty()) {
			multiproperties = 0;
			properties = &(selected_modules->at(0))->properties;
		} else if(selected_modules->empty() && selected_links->size()==1) {
			multiproperties = 0;
			properties = &(selected_links->at(0))->properties;
		} else if(selected_modules->empty()){
			properties = 0;
			multiproperties = create_multiproperties(selected_links);
		} else if(selected_links->empty()){
			properties = 0;
			multiproperties = create_multiproperties(selected_modules);
		} else {
			properties = 0;
			Properties* p = new Properties();
			int nmodules = selected_modules->size();
			int nlinks =  selected_links->size();
			p->add("NBSELECTED_MODULES", nmodules);
			p->add("NBSELECTED_LINKS", nlinks);
			multiproperties = p;
		}
		update();
	}

	virtual Properties* create_multiproperties(std::vector<Link*>* selection) {
			Properties* p = new Properties();
			int nbSelected = 0;
			p->add("NBSELECTED_LINKS", nbSelected);
			for(uint i=0; i<selection->size(); i++) {
				Link* g = dynamic_cast<Link*>(selection->at(i));
				if(!g) continue;
				nbSelected++;
				p->set("NBSELECTED_LINKS", nbSelected);
				for(uint i=0; i<g->properties.size(); i++) {
					Property* pp = g->properties[i];
					Property* mypp = p->get(pp->name);
					if(mypp==NULL) p->add(pp->copy());
					else if(mypp->get_value_as_string() != pp->get_value_as_string()) mypp->set_undefined();
				}
			}
			return p;
		}

	virtual Properties* create_multiproperties(std::vector<Module*>* selection) {
		Properties* p = new Properties();
		int nbSelected = 0;
		p->add("NBSELECTED_MODULES", nbSelected);
		for(uint i=0; i<selection->size(); i++) {
			Module* g = dynamic_cast<Module*>(selection->at(i));
			if(!g) continue;
			nbSelected++;
			p->set("NBSELECTED_MODULES", nbSelected);
			for(uint i=0; i<g->properties.size(); i++) {
				Property* pp = g->properties[i];
				Property* mypp = p->get(pp->name);
				if(mypp==NULL) p->add(pp->copy());
				else if(mypp->get_value_as_string() != pp->get_value_as_string()) mypp->set_undefined();
			}
		}
		return p;
	}


	virtual std::string answer(const std::string& request, const std::string& data) {
		if(str_starts_with(request, "set/")) {
			std::string field = str_between(request, "set/", "=");
			if(selected_links->empty()) {
				for(uint i=0; i<selected_modules->size(); i++) {
					Module* m = selected_modules->at(i);
					m->set_property(field, data);
				}
			} else {
				for(uint i=0; i<selected_links->size(); i++) {
					Link* m = selected_links->at(i);
					m->set_property(field, data);
				}
			}
			return "ok";
		} else {
			if(multiproperties) return multiproperties->to_JSON();
			else if(!properties) return "";
			else return properties->to_JSON();
		}
	}


	void update() {
		if(is_loaded())	{
			script("update();");
		}
	}
};

#endif /* PROPERTIESFORM_H_ */
