/*
 * CommandPaste.h
 *
 *  Created on: 12 nov. 2014
 *      Author: jfellus
 */

#ifndef CommandPaste_H_
#define CommandPaste_H_

#include <commands/Command.h>
#include "../module/Document.h"
#include <ZoomableDrawingArea.h>


class Pastable {
public:
	Pastable() {}
	Pastable(const Pastable& p) {
		create_copy(p.clipboard_modules, p.clipboard_links);
	}

	std::vector<Module*> clipboard_modules;
	std::vector<Link*> clipboard_links;
	std::map<Module*,Module*> copyof;
	Vector2D clipboard_center;
	bool used = false;

	void use() { used = true; }

	Pastable* copy() {
		return new Pastable(*this);
	}

	void create_copy(const std::vector<Module*>& modules, const std::vector<Link*>& links) {
		clipboard_center = Vector2D(0,0);
		for(uint i=0; i<modules.size(); i++) copy_module(modules[i]);
		for(uint i=0; i<links.size(); i++) {
			Link* l = links[i]->copy();

			// SOLUTION 1 : Copy source and destination modules ?
			//			if(copyof.count(l->src)==0) copy_module(l->src);
			//			if(copyof.count(l->dst)==0) copy_module(l->dst);
			//			l->connect(copyof[l->src], copyof[l->dst]);

			// SOLUTION 2 : Don't copy source and destination modules
			l->connect(copyof.count(l->src) ? copyof[l->src] : l->src, copyof.count(l->dst) ? copyof[l->dst] : l->dst);

			clipboard_links.push_back(l);
			l->detach();
		}
		clipboard_center /= modules.size();
	}

	void create_cut(const std::vector<Module*>& modules, const std::vector<Link*>& links) {
		clipboard_center = Vector2D(0,0);
		for(uint i=0; i<modules.size(); i++) {
			Module* m = modules[i];
			clipboard_modules.push_back(m);
			clipboard_center += Vector2D(m->component->x,m->component->y);
		}
		for(uint i=0; i<links.size(); i++) {
			Link* l = links[i];
			clipboard_links.push_back(l);
			clipboard_center += Vector2D(l->src->component->x,l->src->component->y);
			clipboard_center += Vector2D(l->dst->component->x,l->dst->component->y);
		}
		clipboard_center /= links.size()*2 + modules.size();
	}

protected:
	void copy_module(Module* module) {
		Module* m = module->copy();
		copyof[module] = m;
		clipboard_modules.push_back(m);
		clipboard_center += Vector2D(m->component->x,m->component->y);
		m->detach();
	}
};

class CommandPaste : public Command {
public:
	Document* doc;
	double x,y;
	Pastable* pastable;

	static Pastable* cur_pastable;

public:
	CommandPaste(Document* doc, double x, double y) : doc(doc), x(x), y(y) {
		pastable = cur_pastable;
		if(pastable->used) {pastable = cur_pastable = pastable->copy();}
		pastable->use();

		Group* group = doc->get_group_at(x,y);

		for(uint i=0; i<pastable->clipboard_modules.size(); i++) {
			Module* m = pastable->clipboard_modules[i];
			m->component->x = x + (m->component->x - pastable->clipboard_center.x);
			m->component->y = y + (m->component->y - pastable->clipboard_center.y);
			if(m->parent) m->parent->remove(m);
			if(group) group->add(m);
		}

		doc->update_links_layers();
		doc->fire_change_event();
		ZoomableDrawingArea::cur()->update_layers();
	}
	virtual ~CommandPaste() {}

	virtual void execute() {
		pastable->used = true;
		for(uint i=0; i<pastable->clipboard_modules.size(); i++) pastable->clipboard_modules[i]->attach();
		for(uint i=0; i<pastable->clipboard_links.size(); i++) pastable->clipboard_links[i]->attach();

		for(uint i=0; i<pastable->clipboard_modules.size(); i++) {
			pastable->clipboard_modules[i]->on_parent_change(pastable->clipboard_modules[i]->parent);
		}

		doc->update_links_layers();
		doc->fire_change_event();
	}

	virtual void undo() {
		pastable->used = false;
		for(uint i=0; i<pastable->clipboard_modules.size(); i++) pastable->clipboard_modules[i]->detach();
		for(uint i=0; i<pastable->clipboard_links.size(); i++) pastable->clipboard_links[i]->detach();
		doc->update_links_layers();
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return "Paste";
	}
};

#endif /* CommandPaste_H_ */
