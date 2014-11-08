/*
 * GraphicsImage.cpp
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#include "GraphicsImage.h"
#include <cairo/cairo.h>
#include "../util/utils.h"

GraphicsImage::GraphicsImage(int w, int h) {
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
    cr = cairo_create (surface);
}

GraphicsImage::~GraphicsImage() {
    cairo_surface_destroy (surface);
    cairo_destroy(cr);
}

void GraphicsImage::save(const char* filename) {
	if(file_has_ext(filename, ".png"))
		cairo_surface_write_to_png (surface, filename);
	else DBG("Unknown format : " << filename);
}
