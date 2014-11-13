/*
 * CommandLinkReconnect.h
 *
 *  Created on: 13 nov. 2014
 *      Author: jfellus
 */

#ifndef COMMANDLINKRECONNECT_H_
#define COMMANDLINKRECONNECT_H_

#include <commands/Command.h>
#include "../module/Document.h"

class CommandLinkReconnect : public Command {
public:
	Link* link;
	Module* src;
	Module* dst;
	Module* old_src = NULL;
	Module* old_dst = NULL;
public:
	CommandLinkReconnect(Link* l, Module* src, Module* dst) : link(l), src(src), dst(dst) {}
	virtual ~CommandLinkReconnect() {}

	virtual void execute() {
		old_src = link->src;
		old_dst = link->dst;
		link->connect(src, dst);
		Document::cur()->update_links_layers();
		Document::cur()->fire_change_event();
	}

	virtual void undo() {
		link->connect(old_src, old_dst);
		Document::cur()->update_links_layers();
		Document::cur()->fire_change_event();
	}

	virtual std::string get_text() {return "Reconnect link";}
};

#endif /* COMMANDLINKRECONNECT_H_ */
