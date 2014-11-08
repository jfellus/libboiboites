/*
 * Graphics.h
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "Color.h"
#include "../util/geom.h"
#include <map>
#include <cairo.h>

class SVG;
class ZoomableDrawingArea;
class ColorMap;

/** NOT INTENDED TO INSTANTIATION : USE SUBCLASSES */
class Graphics {
public:
	std::map<uint,RGB*> colormap;
	cairo_t* cr;
	ZoomableDrawingArea* canvas;
private:
public:
	Graphics(); // Create dummy Graphics for extents measurements
	Graphics(cairo_t* cr, ZoomableDrawingArea* a) {this->cr = cr; canvas = a;}
	virtual ~Graphics();

	operator cairo_t*() const { return cr;}


	// Drawing

	void drawGradient1();
	void drawGradient2();
	void drawGradient3();

	void drawSVG(SVG& svg);

	void dash(int dashtype = 1);

	void drawPoint(const Vector2D& p) {drawPoint(p.x,p.y);}
	void drawPoint(double x, double y);
	void drawLine(const Vector2D& a, const Vector2D& b) {drawLine(a.x, a.y, b.x, b.y);}
	void drawLine(double x1, double y1, double x2, double y2);
	void drawRect(double x, double y, double w, double h);
	void drawRoundedRect(double x, double y, double w, double h);
	void drawRect(const Rectangle& r) {drawRect(r.x, r.y, r.w, r.h);}
	void drawRoundedRect(const Rectangle& r) {drawRoundedRect(r.x, r.y, r.w, r.h);}
	void drawHandle(double x, double y);
	void drawCubicBez(float* p);
	void draw_slashes(int nb_slashes, Vector2D a, const Vector2D& b);


	void mask_circle(const Circle2D& c) { mask_circle(c.center.x, c.center.y, c.radius);}
	void mask_circle(double x, double y, double r);

	void stroke_line_width_independent(double lw);

	void map_color(RGB c_from, RGB  c_to);
	void unmap_color(RGB c_from);
	RGB* get_mapped_color(RGB* c);

	void bezier(const Bezier& b);
	void line(const Line2D& l);
	void rectangle(const Rectangle& r);
	void rectangle(double x, double y, double w, double h) {cairo_rectangle(cr, x,y,w,h);}
	void rounded_rectangle(const Rectangle& r, double radius = -1);
	void rounded_rectangle(double x, double y, double w, double h, double radius = -1);
	void circle(double x, double y, double r);
	void circle(const Circle2D& c) { circle(c.center.x, c.center.y, c.radius); }
	void circle(const Rectangle& inner_rect);

	void fill(RGB rgb);
	void fill_alpha(RGB rgb, double alpha);
	void stroke_alpha(RGB rgb, double thickness, double alpha);
	void fill_all(RGB rgb);

	void set_color(const RGB& rgb);
	void set_line_width(double lw);

	void fill_and_stroke(RGB* fill, RGB* stroke = 0);
	Rectangle fill_and_stroke_extents(bool fill, bool stroke);
	bool is_in_fill_or_stroke(double x, double y, bool fill, bool stroke, double tolerance);
	Rectangle get_stroke_extents();

	void translate(double dx, double dy) {cairo_translate(cr, dx, dy);}
	void scale(double sx, double sy) {cairo_scale(cr, sx, sy);}
	void scale(double s) {cairo_scale(cr, s, s);}

	void clip() {cairo_clip(cr);}
	void reset_clip() {cairo_reset_clip(cr);}
	void stroke() {cairo_stroke(cr);}
	void fill() {cairo_fill(cr);}

	void save() {cairo_save(cr);}
	void restore() {cairo_restore(cr);}

	void set_line_cap(cairo_line_cap_t lc) {cairo_set_line_cap(cr, lc);}
	void move_to(double x, double y) {cairo_move_to(cr, x, y);}
	void line_to(double x, double y) {cairo_line_to(cr, x,y);}
	void curve_to(double x1, double y1, double x2, double y2, double x3, double y3) {cairo_curve_to(cr, x1, y1, x2, y2, x3, y3);}
	void close_path() {cairo_close_path(cr);}
	inline void clear_path() {cairo_new_path(cr);}

	void drawArrowEnd(const Vector2D& p, const Vector2D& towards, double size = 1);
	cairo_path_t* get_piecewise_linear() {return cairo_copy_path_flat(cr);}

	void set_font(uint size = 600, const std::string& font = "Serif", int style = 0);
	void text(const std::string& s, const Rectangle& bounds = Rectangle());
	Rectangle text_extents(const std::string &s);
};

#endif /* GRAPHICS_H_ */
