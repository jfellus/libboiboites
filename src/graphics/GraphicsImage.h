/*
 * GraphicsImage.h
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#ifndef GRAPHICSIMAGE_H_
#define GRAPHICSIMAGE_H_

#include "Graphics.h"


class GraphicsImage : public Graphics {
protected:
	cairo_surface_t *surface;
public:
	GraphicsImage(int w, int h);
	virtual ~GraphicsImage();

	void save(const char* filename);
};

#endif /* GRAPHICSIMAGE_H_ */
