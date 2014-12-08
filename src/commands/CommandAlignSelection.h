/*
 * CommandAlignSelection.h
 *
 *  Created on: 11 nov. 2014
 *      Author: jfellus
 */

#ifndef CommandAlignSelection_H_
#define CommandAlignSelection_H_

#include <commands/Command.h>
#include "../module/Document.h"

namespace libboiboites {


class CommandAlignSelection : public Command {
public:
	Document* doc;
	std::vector<Module*> modules;
	std::vector<Vector2D> old_pos;
	Vector2D center;
	bool evenly_is_x;
	double minx, maxx, miny, maxy;
public:
	static bool _sort_x(Module* c1, Module* c2) {	return c1->component->center().x < c2->component->center().x;}
	static bool _sort_y(Module* c1, Module* c2) {	return c1->component->center().y < c2->component->center().y;}

	CommandAlignSelection(Document* doc) : doc(doc), modules(doc->selected_modules) {
		maxx = modules[0]->component->center().x;
		minx = modules[0]->component->center().x;
		maxy = modules[0]->component->center().y;
		miny = modules[0]->component->center().y;

		for(uint i=0; i<modules.size(); i++) {
			old_pos.push_back(modules[i]->component->center());
			center += modules[i]->component->center();
			modules[i]->component->center();
			if(modules[i]->component->center().x > maxx) maxx = modules[i]->component->center().x;
			if(modules[i]->component->center().x < minx) minx = modules[i]->component->center().x;
			if(modules[i]->component->center().y > maxy) maxy = modules[i]->component->center().y;
			if(modules[i]->component->center().y < miny) miny = modules[i]->component->center().y;
		}
		center /= modules.size();
		evenly_is_x = (maxx-minx > maxy-miny);

		std::sort(modules.begin(), modules.end(), evenly_is_x ? _sort_x : _sort_y);
	}

	virtual ~CommandAlignSelection() {}

	virtual void execute() {
		for(uint i=0; i<modules.size(); i++) {
			Vector2D p = modules[i]->component->center();
			if(evenly_is_x) {
				p.y = center.y;
				p.x = minx + ((maxx-minx)*i)/(modules.size()-1);
			} else {
				p.x = center.x;
				p.y = miny + ((maxy-miny)*i)/(modules.size()-1);
			}
			modules[i]->component->center(p, true);
		}
		doc->fire_change_event();
	}

	virtual void undo() {
		for(uint i=0; i<modules.size(); i++) {
			modules[i]->component->center(old_pos[i], true);
		}
		doc->fire_change_event();
	}

	virtual std::string get_text() {
		return TOSTRING("Align");
	}
};

}

#endif /* CommandAlignSelection_H_ */
