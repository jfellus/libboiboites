/*
 * CommandCloseGroup.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef CommandCloseGroup_H_
#define CommandCloseGroup_H_

#include <commands/Command.h>
#include "../module/Document.h"

namespace libboiboites {

class CommandCloseGroup : public Command {
public:
	Document* doc;
	Group* group;
public:
	CommandCloseGroup(Document* doc, Group* g) : doc(doc), group(g) {}
	virtual ~CommandCloseGroup() {}

	virtual void execute() {
		group->close();
		Document::cur()->unselect_all();
		group->component->select(true);
	}

	virtual void undo() {
		group->open();
		Document::cur()->unselect_all();
	}

	virtual std::string get_text() {
		return "Close group";
	}
};

}

#endif /* CommandCloseGroup_H_ */
