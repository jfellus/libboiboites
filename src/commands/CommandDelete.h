/*
 * CommandDelete.h
 *
 *  Created on: 12 nov. 2014
 *      Author: jfellus
 */

#ifndef COMMANDDELETE_H_
#define COMMANDDELETE_H_

#include <commands/Command.h>
#include "../module/Document.h"


class CommandDelete : public Command {
public:
	Document* doc;
	std::vector<Module*> modules;
	std::vector<Link*> links;
	std::vector<Group*> parents;
public:
	CommandDelete(Document* doc) : doc(doc), modules(doc->selected_modules), links(doc->selected_links) {}
	virtual ~CommandDelete() {
		for(uint i=0; i<modules.size(); i++) {
			if(!modules[i]->bAttached) delete modules[i];
		}
		for(uint i=0; i<links.size(); i++) {
			if(!links[i]->bAttached) delete links[i];
		}
	}

	virtual void execute() {
		for(uint i=0; i<modules.size(); i++) {
			Module* m = modules[i];
			m->detach();
		}
		for(uint i=0; i<links.size(); i++) {
			Link* l = links[i];
			l->detach();
		}
		doc->fire_change_event();
	}

	virtual void undo() {
		for(uint i=0; i<modules.size(); i++) {
			Module* m = modules[i];
			m->attach();
		}
		for(uint i=0; i<links.size(); i++) {
			Link* l = links[i];
			l->attach();
		}
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return "Delete";
	}
};

#endif /* COMMANDDELETE_H_ */
