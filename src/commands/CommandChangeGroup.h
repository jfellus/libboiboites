/*
 * CommandChangeGroup.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef COMMANDCHANGEGROUP_H_
#define COMMANDCHANGEGROUP_H_

#include <commands/Command.h>
#include "../module/Document.h"


class CommandChangeGroup : public Command {
public:
	Document* doc;
	std::vector<Module*> modules;
	std::vector<Group*> old_parent;
	Group* group;
public:
	CommandChangeGroup(Document* doc, Group* g) : doc(doc), modules(doc->selected_modules), group(g) {}
	virtual ~CommandChangeGroup() {}

	virtual void execute() {
		for(uint i=0; i<modules.size(); i++) {
			old_parent[i] = modules[i]->parent;
			if(modules[i]->parent) {
				modules[i]->parent->remove(modules[i]);
			}
			if(group) group->add(modules[i]);
		}
		doc->update_links_layers();
		doc->fire_change_event();
	}

	virtual void undo() {
		for(uint i=0; i<modules.size(); i++) {
			if(modules[i]->parent) {
				modules[i]->parent->remove(modules[i]);
			}
			if(old_parent[i]) old_parent[i]->add(modules[i]);
		}

		doc->update_links_layers();
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return "Change group";
	}
};

#endif /* COMMANDCHANGEGROUP_H_ */
