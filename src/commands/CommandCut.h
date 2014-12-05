/*
 * CommandCut.h
 *
 *  Created on: 12 nov. 2014
 *      Author: jfellus
 */

#ifndef CommandCut_H_
#define CommandCut_H_

#include <commands/Command.h>
#include "../module/Document.h"
#include "CommandPaste.h"

class CommandCut : public Command {
public:
	Document* doc;
	std::vector<Module*> modules;
	std::vector<Link*> links;

	Pastable* pastable = NULL;

public:
	CommandCut(Document* doc) : doc(doc), modules(doc->selected_modules), links(doc->selected_links) {
		pastable = new Pastable();
		pastable->create_cut(modules, links);
		CommandPaste::cur_pastable = pastable;
	}

	virtual ~CommandCut() {
		while(!pastable->clipboard_links.empty()) { Link* l = pastable->clipboard_links[0]; vector_remove(pastable->clipboard_links, l); delete l;}
		while(!pastable->clipboard_modules.empty()) { Module* m = pastable->clipboard_modules[0]; vector_remove(pastable->clipboard_modules, m); delete m;}
	}

	virtual void execute() {
		for(uint i=0; i<modules.size(); i++) modules[i]->detach();
		for(uint i=0; i<links.size(); i++) links[i]->detach();
		doc->fire_change_event();
	}

	virtual void undo() {
		for(uint i=0; i<modules.size(); i++) modules[i]->attach();
		for(uint i=0; i<links.size(); i++) links[i]->attach();
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return "Cut";
	}
};

#endif /* CommandCut_H_ */
