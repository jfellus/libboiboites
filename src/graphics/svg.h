/*
 * svg.h
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#ifndef SVG_H_
#define SVG_H_

#include "GraphicsImage.h"

struct NSVGimage;

class SVG {
public:
	std::string filename;
public:
	static SVG* get_resource(const char* filename);
public:
	struct NSVGimage* image;

	SVG() {init();}
	SVG(const char* filename) {init(); load(filename);}
	virtual ~SVG();

	void init();
	void load(const char* filename);

	Rectangle get_bounds();
	double get_width();
	double get_height();

	void render(Graphics& g);

	void save(const char* filename);
};



#endif /* SVG_H_ */
