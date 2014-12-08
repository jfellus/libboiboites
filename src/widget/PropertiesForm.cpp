/*
 * PropertiesForm.cpp
 *
 *  Created on: 9 oct. 2014
 *      Author: jfellus
 */

#include "PropertiesForm.h"
#include "../workbench/Workbench.h"
#include "../commands/CommandPropertySetMultiple.h"

namespace libboiboites {

void PropertiesForm::update(std::vector<Module*>* selected_modules, std::vector<Link*>* selected_links) {
	this->selected_modules = selected_modules;
	this->selected_links = selected_links;
	if(multiproperties) {delete multiproperties; multiproperties = 0;}

	if(selected_modules->empty() && selected_links->empty()) {
		properties = multiproperties = 0;
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
		p->add("__NBSELECTED_MODULES__", nmodules);
		p->add("__NBSELECTED_LINKS__", nlinks);
		multiproperties = p;
	}
}

Properties* PropertiesForm::create_multiproperties(std::vector<Link*>* selection) {
	Properties* p = new Properties();
	int nbSelected = 0;
	p->add("__NBSELECTED_LINKS__", nbSelected);
	for(uint i=0; i<selection->size(); i++) {
		Link* g = dynamic_cast<Link*>(selection->at(i));
		if(!g) continue;
		nbSelected++;
		p->set("__NBSELECTED_LINKS__", nbSelected);
		for(uint i=0; i<g->properties.size(); i++) {
			Property* pp = g->properties[i];
			Property* mypp = p->get(pp->name);
			if(mypp==NULL) p->add(pp->copy());
			else if(mypp->get_value_as_string() != pp->get_value_as_string()) mypp->set_undefined();
		}
	}
	return p;
}

Properties* PropertiesForm::create_multiproperties(std::vector<Module*>* selection) {
	Properties* p = new Properties();
	int nbSelected = 0;
	p->add("__NBSELECTED_MODULES__", nbSelected);
	for(uint i=0; i<selection->size(); i++) {
		Module* g = dynamic_cast<Module*>(selection->at(i));
		if(!g) continue;
		nbSelected++;
		p->set("__NBSELECTED_MODULES__", nbSelected);
		for(uint i=0; i<g->properties.size(); i++) {
			Property* pp = g->properties[i];
			Property* mypp = p->get(pp->name);
			if(mypp==NULL) p->add(pp->copy());
			else if(mypp->get_value_as_string() != pp->get_value_as_string()) mypp->set_undefined();
		}
	}
	return p;
}


std::string PropertiesForm::answer(const std::string& request, const std::string& data) {
	if(request=="unselect_modules") {
		Workbench::cur()->unselect_all_modules();
		return "ok";
	} else if(request=="unselect_links") {
		Workbench::cur()->unselect_all_links();
		return "ok";
	}
	else if(str_starts_with(request, "set/")) {
		std::string field = str_between(request, "set/", "=");
		if(selected_links->empty())
			(new CommandPropertySetMultiple(*selected_modules, field, data))->execute();
		else (new CommandPropertySetMultiple(*selected_links, field, data))->execute();
		return "ok";
	} else {
		if(multiproperties) return multiproperties->to_JSON();
		else if(!properties) return "";
		else return properties->to_JSON();
	}
}

void PropertiesForm::reset() {
	multiproperties = 0;
	properties = 0;
	selected_modules = 0;
	selected_links = 0;
}

}
