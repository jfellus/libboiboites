/*
 * CommandSpaceSelection.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef CommandSpaceSelection_H_
#define CommandSpaceSelection_H_

#include <commands/Command.h>
#include "../module/Document.h"



namespace libboiboites {

class CommandSpaceSelection : public Command {
public:
	Document* doc;
	std::vector<Module*> modules;
	Vector2D center;
	double amount;
public:

	CommandSpaceSelection(Document* doc, double amount) : doc(doc), modules(doc->selected_modules), amount(amount) {
		for(uint i=0; i<modules.size(); i++) center += modules[i]->component->center();
		center /= modules.size();
	}

	virtual ~CommandSpaceSelection() {}

	virtual void execute() {
		for(uint i=0; i<modules.size(); i++) {
			Vector2D dir = (modules[i]->component->center() - center) * amount;
			modules[i]->translate(dir.x, dir.y,true);
		}
		doc->fire_change_event();
	}

	virtual void undo() {
		for(uint i=0; i<modules.size(); i++) {
			Vector2D dir = (1-1/(1+amount))*(center - modules[i]->component->center());
			modules[i]->translate(dir.x, dir.y,true);
		}
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return TOSTRING("Space selection");
	}
};



}

#endif /* CommandSpaceSelection_H_ */
