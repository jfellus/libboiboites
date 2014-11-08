/*
 * svg.cpp
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */


#include "../../lib/nanosvg.h"

#include "svg.h"
#include "../util/utils.h"

static std::map<std::string, SVG*> resources;
SVG* SVG::get_resource(const char* filename) {
	std::string fullpath = file_absolute_path(filename);
	if(resources.count(fullpath)) {
		return resources[fullpath];
	} else {
		SVG* svg = new SVG;
		svg->load(fullpath.c_str());
		resources[fullpath] = svg;
		return svg;
	}
}

void SVG::load(const char* filename) {
	this->filename = filename;
	image = nsvgParseFromFile(filename, "px", 96);
	if(!image) throw "";
}

void SVG::init() {
	image = 0;
}

SVG::~SVG() {
	nsvgDelete(image);
}

double SVG::get_width() {
	return image->width;
}

double SVG::get_height() {
	return image->height;
}

void SVG::render(Graphics& g) {
	g.save();
	NSVGshape* shape;
	NSVGpath* path;
	int i;
	g.set_color(RGB_BLACK);
	for (shape = image->shapes; shape != NULL; shape = shape->next) {
		for (path = shape->paths; path != NULL; path = path->next) {
	        for (i = 0; i < path->npts-1; i += 3) {
	            float* p = &path->pts[i*2];
	        	if(!i) g.move_to(p[0],p[1]);
	        	g.curve_to(p[2],p[3],p[4],p[5],p[6],p[7]);
	        }
	        if(path->closed) g.close_path();
	    }
		RGB fill(shape->fill.color);
		RGB stroke(shape->stroke.color);
		g.set_line_width(shape->strokeWidth);
		g.fill_and_stroke(
				shape->fill.type == NSVG_PAINT_COLOR ? &fill : 0,
	    		shape->stroke.type == NSVG_PAINT_COLOR ? &stroke : 0
	    );
	}
	g.restore();
}

void SVG::save(const char* filename) {
	if(file_has_ext(filename, ".png")) {
		GraphicsImage cr(get_width(),get_height());
	    render(cr);
	    cr.save(filename);
	}
}

Rectangle SVG::get_bounds() {
	Rectangle r;
	Graphics g;
	NSVGshape* shape;
	NSVGpath* path;
	int i;
	g.set_color(RGB_BLACK);
	for (shape = image->shapes; shape != NULL; shape = shape->next) {
		for (path = shape->paths; path != NULL; path = path->next) {
	        for (i = 0; i < path->npts-1; i += 3) {
	            float* p = &path->pts[i*2];
	        	if(!i) g.move_to(p[0],p[1]);
	        	g.curve_to(p[2],p[3],p[4],p[5],p[6],p[7]);
	        }
	        if(path->closed) g.close_path();
	    }
		RGB fill(shape->fill.color);
		RGB stroke(shape->stroke.color);
		g.set_line_width(shape->strokeWidth);
		Rectangle rr = g.fill_and_stroke_extents(shape->fill.type == NSVG_PAINT_COLOR ,shape->stroke.type == NSVG_PAINT_COLOR);
		r.add(rr);
	}
	return r;
}
