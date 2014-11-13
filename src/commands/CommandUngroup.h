/*
 * CommandUngroup.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef COMMANDUNGROUP_H_
#define COMMANDUNGROUP_H_

#include <commands/Command.h>
#include "../module/Document.h"
#include <map>


class CommandUngroup : public Command {
public:
	Document* doc;
	std::vector<Module*> modules;
	std::vector<Group*> old_groups;
	std::vector<Group*> old_parents;
	std::vector<std::vector<Module*>> old_children;
	std::vector<bool> old_status;

public:
	CommandUngroup(Document* doc) : doc(doc), modules(doc->selected_modules) {}

	virtual ~CommandUngroup() {

	}

	virtual void execute() {
		for(uint i=0; i<modules.size(); i++) {
			Group* g = dynamic_cast<Group*>(modules[i]);
			if(!g) continue;
			old_groups.push_back(g);
			old_parents.push_back(g->parent);
			old_children.push_back(g->children);
			old_status.push_back(g->is_opened());
			g->ungroup();
		}

		doc->update_links_layers();
		doc->fire_change_event();
	}

	virtual void undo() {
		for(uint i = 0; i<old_groups.size(); i++) {
			doc->add_module(old_groups[i]);
			if(old_parents[i]) old_parents[i]->add(old_groups[i]);
			for(uint j=0; j<old_children[i].size(); j++) {
				old_groups[i]->add(old_children[i][j]);
			}
			old_groups[i]->close();
			if(old_status[i]) old_groups[i]->open();
		}

		doc->update_links_layers();
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return "Ungroup";
	}
};

#endif /* COMMANDUNGROUP_H_ */
