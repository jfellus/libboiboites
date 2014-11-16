/*
 * CommandPropertySetMultiple.h
 *
 *  Created on: 13 nov. 2014
 *      Author: jfellus
 */

#ifndef COMMANDPROPERTYSETMULTIPLE_H_
#define COMMANDPROPERTYSETMULTIPLE_H_

#include <commands/Command.h>
#include "../module/Document.h"

class CommandPropertySetMultiple : public Command {
public:
	std::vector<Module*> modules;
	std::vector<Link*> links;
	std::string name;
	std::string value;
	std::vector<std::string> old_module_values;
	std::vector<std::string> old_link_values;

	bool bFirstTime = true;

public:
	CommandPropertySetMultiple(const std::vector<Module*>& modules, const std::string& name, const std::string& value) :
		modules(modules), name(name), value(value) {}
	CommandPropertySetMultiple(const std::vector<Link*>& links, const std::string& name, const std::string& value) :
		links(links), name(name), value(value) {}
	CommandPropertySetMultiple(const std::vector<Module*>& modules, const std::vector<Link*>& links, const std::string& name, const std::string& value) :
		links(links), name(name), value(value) {}

	virtual ~CommandPropertySetMultiple() {}

	virtual void execute() {
		for(uint i=0; i<modules.size(); i++) {
			old_module_values.push_back(modules[i]->get_property(name));
			modules[i]->set_property(name, value);
		}
		for(uint i=0; i<links.size(); i++) {
			old_link_values.push_back(links[i]->get_property(name));
			links[i]->set_property(name, value);
		}
		Document::cur()->fire_change_event();
		bFirstTime = false;
	}

	virtual void undo() {
		for(uint i=0; i<modules.size(); i++) {
			modules[i]->set_property(name, old_module_values[i]);
		}
		for(uint i=0; i<links.size(); i++) {
			links[i]->set_property(name, old_link_values[i]);
		}
		Document::cur()->fire_change_event();
	}

	virtual std::string get_text() {
		return TOSTRING("Set " << name);
	}
};

#endif /* COMMANDPROPERTYSETMULTIPLE_H_ */
