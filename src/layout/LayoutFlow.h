/*
 * LayoutFlow.h
 *
 *  Created on: 29 oct. 2014
 *      Author: jfellus
 */

#ifndef LAYOUTFLOW_H_
#define LAYOUTFLOW_H_

#include "ModulesLayout.h"
#include "../module/Module.h"
#include "../module/Link.h"
#include <queue>
#include <list>

class LayoutFlowModule {
public:
	Module* module;
	std::vector<LayoutFlowModule*> targets;
	bool hasParent = false;
	int depth = -1;
	int rowindex = 0;
	int _rowindex = -1;
	LayoutFlowModule(Module* m) : module(m) {}

	void addTarget(LayoutFlowModule* m) { if(m==this) return; targets.push_back(m); m->hasParent = true;}

};

class LayoutFlow : public ModulesLayout {
public:
	std::vector<LayoutFlowModule*> modules;
	std::vector<int> widths;

public:
	LayoutFlow() {}
	virtual ~LayoutFlow() {}

	virtual void add(Module* m);
	virtual void add(Link* l);

	LayoutFlowModule* get(Module* m) ;

	void compute_depths();

	void compute_row_indices();

	virtual void layout();
};

#endif /* LAYOUTFLOW_H_ */
