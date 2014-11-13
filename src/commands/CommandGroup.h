/*
 * CommandGroup.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef COMMANDGROUP_H_
#define COMMANDGROUP_H_

#include <commands/Command.h>
#include "../module/Document.h"

class CommandGroup : public Command {
public:
	Group* g = NULL;
	Document* doc;
	Group* parent;
	std::list<Module*> modules_to_add;
public:
	CommandGroup(Document* doc, Group* parent, const std::list<Module*>& modules_to_add) :
		doc(doc), parent(parent), modules_to_add(modules_to_add) {}
	virtual ~CommandGroup() {}

	virtual void execute() {
		g = new Group();
		g->realize();
		for(auto i=modules_to_add.begin(); i!=modules_to_add.end(); i++) g->add(*i);
		if(parent) parent->add(g);
		g->close();

		doc->update_links_layers();
		doc->fire_change_event();
	}

	virtual void undo() {
		g->open();
		g->ungroup();
		doc->update_links_layers();
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return "Group";
	}
};

#endif /* COMMANDGROUP_H_ */
