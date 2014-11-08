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

	virtual void add(Module* m) {
		modules.push_back(new LayoutFlowModule(m));
	}

	virtual void add(Link* l) {
		get(l->src)->addTarget(get(l->dst));
	}

	LayoutFlowModule* get(Module* m) {
		for(uint i=0; i<modules.size(); i++) {
			if(modules[i]->module==m) return modules[i];
		}
		return NULL;
	}

	void compute_depths() {
		uint nbdone = 0;
		int depth = 0;
		widths.push_back(0);

		for(uint i=0; i<modules.size(); i++) {
			if(!modules[i]->hasParent) {modules[i]->depth=0; nbdone++; widths[0]++;}
		}

		while(nbdone < modules.size()) {
			widths.push_back(0);
			for(uint i=0; i<modules.size(); i++) {
				if(modules[i]->depth == depth) {
					for(uint j=0; j<modules[i]->targets.size(); j++) {
						if(modules[i]->targets[j]->depth==-1) {
							modules[i]->targets[j]->depth = depth+1;
							widths[depth+1]++;
							nbdone++;
						}
					}
				}
			}
			depth++;
		}
	}

	void compute_row_indices() {
		uint nbdone = 0;
		int depth = 0;
		int rowindex = 0;
		for(uint i=0; i<modules.size(); i++) {
			if(modules[i]->depth==0) {
				modules[i]->_rowindex = modules[i]->rowindex = rowindex++;
				nbdone++;
			}
		}
		while(nbdone < modules.size()) {
			rowindex = 0;
			for(uint i=0; i<modules.size(); i++) {
				LayoutFlowModule* m1 = modules[i];
				if(m1->depth==depth) {
					for(uint j=0; j<m1->targets.size(); j++) {
						LayoutFlowModule* m2 = m1->targets[j];
						m2->_rowindex = m1->_rowindex * widths[depth] + (rowindex++);
						nbdone++;
					}
				}
			}

			depth++;

			for(uint i=0; i<modules.size(); i++) {
				if(modules[i]->depth!=depth) continue;
				modules[i]->rowindex = 0;
				for(uint j=0; j<i; j++) {
					if(modules[j]->depth!=depth) continue;
					if(modules[j]->_rowindex > modules[i]->_rowindex) modules[j]->rowindex++;
					else modules[i]->rowindex++;
				}
			}
		}
	}

	virtual void layout() {
		double yspace = 800;
		double xspace = 10000;

		compute_depths();
		compute_row_indices();

		uint nbdone = 0;
		int depth = 0;
		Vector2D p;
		while(nbdone < modules.size()) {
			for(uint i=0; i<modules.size(); i++) {
				if(modules[i]->depth==depth) {
					p.y = (modules[i]->rowindex - widths[depth]/2) * yspace ;
					modules[i]->module->component->set_pos(p);
					nbdone++;
				}
			}
			depth++;
			p.x += xspace;
		}
	}
};

#endif /* LAYOUTFLOW_H_ */
