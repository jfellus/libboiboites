/*
 * CommandOpenGroup.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef COMMANDOPENGROUP_H_
#define COMMANDOPENGROUP_H_

#include <commands/Command.h>
#include "../module/Document.h"

class CommandOpenGroup : public Command {
public:
	Document* doc;
	Group* group;
public:
	CommandOpenGroup(Document* doc, Group* g) : doc(doc), group(g) {}
	virtual ~CommandOpenGroup() {}

	virtual void execute() {
		group->open();
	}

	virtual void undo() {
		group->close();
	}

	virtual std::string get_text() {
		return "Open group";
	}
};

#endif /* COMMANDOPENGROUP_H_ */
