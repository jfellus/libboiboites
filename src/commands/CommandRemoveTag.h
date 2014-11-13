/*
 * CommandRemoveTag.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef CommandRemoveTag_H_
#define CommandRemoveTag_H_

#include <commands/Command.h>
#include "../module/Document.h"


class CommandRemoveTag : public Command {
public:
	Document* doc;
	std::vector<Module*> modules;
	std::vector<Link*> links;
	std::string cls;
public:
	CommandRemoveTag(Document* doc, int i) : doc(doc), modules(doc->selected_modules), links(doc->selected_links) {
		cls = TOSTRING("tag_" << i);
		for(uint i=0; i<modules.size(); i++) {
			if(!modules[i]->component || !modules[i]->component->has_class(cls)) {
				vector_remove(modules, modules[i]);
				i--;
			}
		}
		for(uint i=0; i<links.size(); i++) {
			if(!links[i]->component || !links[i]->component->has_class(cls)) {
				vector_remove(links, links[i]);
				i--;
			}
		}
	}

	virtual ~CommandRemoveTag() {}

	virtual void execute() {
		for(uint i=0; i<modules.size(); i++) modules[i]->remove_class(cls);
		for(uint i=0; i<links.size(); i++) links[i]->remove_class(cls);
		doc->fire_change_event();
	}

	virtual void undo() {
		for(uint i=0; i<modules.size(); i++) modules[i]->add_class(cls);
		for(uint i=0; i<links.size(); i++) links[i]->add_class(cls);
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return TOSTRING("Remove tag " << cls);
	}
};

#endif /* CommandRemoveTag_H_ */
