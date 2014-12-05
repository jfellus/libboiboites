/*
 * CommandCopy.h
 *
 *  Created on: 12 nov. 2014
 *      Author: jfellus
 */

#ifndef CommandCopy_H_
#define CommandCopy_H_

#include <commands/Command.h>
#include "../module/Document.h"
#include "CommandPaste.h"


class CommandCopy : public Command {
public:

	Document* doc;
	std::vector<Module*> modules;
	std::vector<Link*> links;

	Pastable* pastable = NULL;

public:
	CommandCopy(Document* doc) : doc(doc), modules(doc->selected_modules), links(doc->selected_links) {
		pastable = new Pastable();
		pastable->create_copy(modules,links);
		CommandPaste::cur_pastable = pastable;
	}

	virtual ~CommandCopy() {
		while(!pastable->clipboard_links.empty()) { Link* l = pastable->clipboard_links[0]; vector_remove(pastable->clipboard_links, l); delete l;}
		while(!pastable->clipboard_modules.empty()) { Module* m = pastable->clipboard_modules[0]; vector_remove(pastable->clipboard_modules, m); delete m;}
	}

	virtual void execute() {
		doc->fire_change_event();
	}

	virtual void undo() {
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return "Copy";
	}


};

#endif /* CommandCopy_H_ */
