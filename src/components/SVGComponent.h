/*
 * SVGComponent.h
 *
 *  Created on: 2 oct. 2014
 *      Author: jfellus
 */

#ifndef SVGCOMPONENT_H_
#define SVGCOMPONENT_H_

#include "Component.h"
#include "../graphics/svg.h"
#include "style/SVGDefinitions.h"


class SVGComponent : public Component {
	SVG* svg = NULL;
	Rectangle bounds;
public:
	SVGComponent(const char* filename) {
		if(file_has_ext(filename, ".svg")) load(filename);
		else {
			std::string f = SVGDefinitions::get(filename);
			if(f.empty()) {throw "";}
			else load(f.c_str());
		}
	}
	virtual ~SVGComponent() {}


	void load(const char* filename) {try {svg = SVG::get_resource(filename);} catch(...) { ERROR("Couldn't load SVG file : " << filename); throw ""; }}


	virtual Rectangle get_bounds() {
		if(!bounds) compute_bounds();
		return bounds + Vector2D(x,y);
	}

	virtual void render(Graphics& g) {
		g.drawSVG(*svg);
	}

	virtual void select(bool single) {
		Component::select(single);
	}

protected:
	void compute_bounds() {
		bounds = svg->get_bounds();
	}

	virtual void dump(std::ostream& os) { os << "SVGComponent(" << svg->filename << ")";}
};


#endif /* SVGCOMPONENT_H_ */
