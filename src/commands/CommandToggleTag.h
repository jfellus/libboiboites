/*
 * CommandToggleTag.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef COMMANDTOGGLETAG_H_
#define COMMANDTOGGLETAG_H_

#include <commands/Command.h>
#include "../module/Document.h"


namespace libboiboites {

class CommandToggleTag : public Command {
public:
	Document* doc;
	std::string cls;
	std::vector<Module*> modules;
	std::vector<Link*> links;
public:
	CommandToggleTag(Document* doc, int i) : doc(doc), modules(doc->selected_modules), links(doc->selected_links) {
		cls = TOSTRING("tag_" << i);
	}
	virtual ~CommandToggleTag() {}

	virtual void execute() {
		for(uint i=0; i<modules.size(); i++) modules[i]->toggle_class(cls);
		for(uint i=0; i<links.size(); i++) links[i]->toggle_class(cls);
		doc->fire_change_event();
	}
	virtual void undo() {
		for(uint i=0; i<modules.size(); i++) modules[i]->toggle_class(cls);
		for(uint i=0; i<links.size(); i++) links[i]->toggle_class(cls);
		doc->fire_change_event();
	}

	virtual std::string get_text() {return TOSTRING("Toggle tag " << cls);}
};


}

#endif /* COMMANDTOGGLETAG_H_ */
