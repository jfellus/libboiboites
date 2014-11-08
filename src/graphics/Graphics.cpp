/*
 * Graphics.cpp
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */



#include "Graphics.h"
#include <cairo/cairo.h>
#include "svg.h"
#include "../widget/ZoomableDrawingArea.h"

static cairo_t* _dummy_context = 0;
static cairo_surface_t* surf = 0;
static cairo_t* create_dummy_cairo_context() {
	if(!surf) {
		surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 0, 0);
	}
	if(_dummy_context) cairo_destroy(_dummy_context);
	_dummy_context = cairo_create(surf);
	return _dummy_context;
}

Graphics::Graphics() {
	cr = create_dummy_cairo_context();
	save();
	canvas = 0;
}

Graphics::~Graphics() {
}


void Graphics::drawSVG(SVG &svg) {
	svg.render(*this);
}

void Graphics::dash(int dashtype) {
	static const double dashed1[] = {4.0};
	static const double dashed2[] = {8.0,2.0};
	cairo_set_dash(cr, dashtype==1 ? dashed1 : dashed2, dashtype, 0);
}

void Graphics::drawPoint(double x, double y) {
	save();
	set_line_cap(CAIRO_LINE_CAP_ROUND);
	move_to (x, y);
	line_to (x, y);
	stroke_line_width_independent(10);
	restore();
}

void Graphics::drawLine(double x1, double y1, double x2, double y2) {
	move_to (x1, y1);
	line_to (x2, y2);
	close_path ();
	stroke_line_width_independent(1);
}

void Graphics::fill_all(RGB rgb) {
	set_color(rgb);
	cairo_paint(cr);
}

void Graphics::fill(RGB rgb) {
	set_color(rgb);
	cairo_fill(cr);
}

void Graphics::fill_alpha(RGB rgb, double alpha) {
	cairo_set_source_rgba(cr, rgb.r/255.0, rgb.g/255.0, rgb.b/255.0, alpha);
	cairo_fill(cr);
}

void Graphics::mask_circle(double x, double y, double r) {
	cairo_arc_negative(cr, x,y, r,2*M_PI, 0);
	clip();
}

void Graphics::draw_slashes(int nb_slashes, Vector2D a, const Vector2D& b) {
	Vector2D v = (b-a).normalize()*60;
	Vector2D v2 = (v-v.normal());
	for(int i=0; i<nb_slashes; i++) {
		drawLine(a-v2, a+v2);
		a += v;
	}
}

void Graphics::stroke_alpha(RGB rgb, double thickness, double alpha) {
	cairo_set_line_width(cr, thickness);
	cairo_set_source_rgba(cr, rgb.r/255.0, rgb.g/255.0, rgb.b/255.0, alpha);
	cairo_stroke(cr);
}

void Graphics::set_color(const RGB& rgb) {
	cairo_set_source_rgb(cr, rgb.r/255.0, rgb.g/255.0, rgb.b/255.0);
}

void Graphics::set_line_width(double lw) {
	cairo_set_line_width(cr, lw);
}

void Graphics::stroke_line_width_independent(double lw) {
	save();
	set_line_width(lw);
	scale(1.0/canvas->get_zoom(),1.0/canvas->get_zoom());
	stroke();
	restore();
}

void Graphics::drawRect(double x, double y, double w, double h) {
	save();
	set_line_width(1);
	rectangle(x,y,w,h);
	scale(1.0/canvas->get_zoom(),1.0/canvas->get_zoom());
	stroke();
	restore();
}

void Graphics::drawRoundedRect(double x, double y, double w, double h) {
	save();
	set_line_width(1);
	rounded_rectangle(x,y,w,h);
	scale(1.0/canvas->get_zoom(),1.0/canvas->get_zoom());
	stroke();
	restore();
}

void Graphics::drawHandle(double x, double y) {
	save();
	translate(x,y);
	scale(1.0/canvas->get_zoom(),1.0/canvas->get_zoom());
	rectangle(-3.5,-3.5,7,7);
	fill();
	restore();
}

void Graphics::map_color(RGB c_from, RGB c_to) {
	RGB* c2 = new RGB(c_to);
	colormap[c_from.as_packed_RGB()] = c2;
}

void Graphics::unmap_color(RGB c_from) {
	colormap.erase(c_from.as_packed_RGB());
}

RGB* Graphics::get_mapped_color(RGB* c) {
	if(!c) return NULL;
	if(colormap.count(c->as_packed_RGB())) return colormap[c->as_packed_RGB()];
	return c;
}

void Graphics::fill_and_stroke(RGB* fill, RGB* stroke) {
	if(colormap.size()) { fill = get_mapped_color(fill); stroke = get_mapped_color(stroke); }
	if(fill && stroke)  {
		set_color(*fill);
		cairo_fill_preserve(cr);
		set_color(*stroke);
		cairo_stroke(cr);
	}
	else if(!fill && stroke)  { set_color(*stroke); cairo_stroke(cr);}
	else if(fill && !stroke)  { set_color(*fill); cairo_fill(cr);}
}

Rectangle Graphics::fill_and_stroke_extents(bool fill, bool stroke) {
	Rectangle r;
	if(stroke)  cairo_stroke_extents(cr, &r.x, &r.y, &r.w, &r.h);
	else if(fill) cairo_fill_extents(cr, &r.x, &r.y, &r.w, &r.h);
	else return r;
	r.w -= r.x; r.h -= r.y;
	r.empty = false;
	return r;
}

Rectangle Graphics::get_stroke_extents() {
	Rectangle r;
	cairo_stroke_extents(cr, &r.x, &r.y, &r.w, &r.h);
	r.w -= r.x; r.h -= r.y;
	return r;
}

bool Graphics::is_in_fill_or_stroke(double x, double y, bool fill, bool stroke, double tolerance) {
	set_line_width(tolerance == 0 ? 1 : tolerance);
	bool b = false;
	if(stroke && fill) b = cairo_in_fill(cr, x,y) || cairo_in_stroke(cr, x,y);
	if(stroke)  b = cairo_in_stroke(cr, x,y);
	else if(fill) b = cairo_in_fill(cr, x,y);
	return b;
}


void Graphics::bezier(const Bezier& b) {
	move_to(b.x1, b.y1);
	curve_to(b.x2, b.y2, b.x3, b.y3, b.x4, b.y4);
}

void Graphics::line(const Line2D& l) {
	move_to(l.a.x, l.a.y);
	line_to(l.b.x, l.b.y);
}

void Graphics::rectangle(const Rectangle& r) {
	rectangle(r.x, r.y, r.w, r.h);
}

void Graphics::rounded_rectangle(const Rectangle& r, double radius) {
	rounded_rectangle(r.x, r.y, r.w, r.h, radius);
}

void Graphics::rounded_rectangle(double x, double y, double w, double h, double _radius) {
	double aspect  = 1.0,  corner_radius = _radius<0 ? h / 40.0 : _radius;
	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cairo_new_sub_path (cr);
	cairo_arc (cr, x + w - radius, y + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc (cr, x + w - radius, y + h - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc (cr, x + radius, y + h - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc (cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path (cr);
}

void Graphics::circle(double x, double y, double r) {
	cairo_arc(cr, x,y, r,0, 2*M_PI);
}

void Graphics::circle(const Rectangle& inner_rect) {
	circle(inner_rect.x+inner_rect.w/2,inner_rect.y+inner_rect.h/2,MAX(inner_rect.w,inner_rect.h)/2);
}


void Graphics::drawArrowEnd(const Vector2D& p, const Vector2D& towards, double size) {
	Vector2D dir = towards; dir.normalize();
	Vector2D orth = dir.normal();
	dir*= MIN(80*size, (20*size / canvas->get_zoom()));
	move_to(p.x, p.y);
	double ssize = MIN(40*size, 10*size / canvas->get_zoom());
	line_to(p.x + dir.x + orth.x * ssize, p.y + dir.y + orth.y*ssize);
	line_to(p.x + dir.x - orth.x * ssize, p.y + dir.y - orth.y*ssize);
	line_to(p.x, p.y);
	cairo_fill(cr);
}

void Graphics::set_font(uint size, const std::string& font, int style) {
	cairo_select_font_face(cr, font.c_str(),
			style & 0x001 ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
			style & 0x010 ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, size);
}

void Graphics::text(const std::string& s, const Rectangle& bounds) {
	if(bounds) {
		cairo_text_extents_t extents;
		cairo_text_extents(cr, s.c_str(), &extents);
		cairo_move_to(cr, bounds.x + bounds.w/2 - extents.width/2, bounds.y + bounds.h/2 - extents.height/2 - extents.y_bearing);
	}
	cairo_show_text(cr, s.c_str());
}

Rectangle Graphics::text_extents(const std::string &s) {
	cairo_text_extents_t extents;
	cairo_text_extents(cr, s.c_str(), &extents);
	return Rectangle(0,0,extents.width, extents.height);
}

