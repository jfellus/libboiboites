/*
 * geom.cpp
 *
 *  Created on: 2 oct. 2014
 *      Author: jfellus
 */


#include "geom.h"
#include "../graphics/Graphics.h"

int sgn(double s) {
	return s > 0 ? 1 : ((s<0) ? -1 : 0);
}

Vector2D Rectangle::center() {return Vector2D(x+w/2,y+h/2);}

Rectangle Bezier::get_precise_bounds() {
	Graphics g;
	Rectangle r;
	g.set_line_width(3);
	g.bezier(*this);
	return g.get_stroke_extents();
}

Rectangle Bezier::get_bounds() {
	Rectangle r(x1,y1,x4-x1,y4-y1);
	r.add(x2,y2);
	r.add(x3,y3);
	return r;
}


std::ostream& operator<<(std::ostream& os, const Rectangle& r)	{
	os << "(" << r.x << " " << r.y << " " << r.w << " " << r.h << ")";
	return os;
}


bool Bezier::has_point(double x, double y, double tolerance) {
	Graphics g;
	g.bezier(*this);
	return g.is_in_fill_or_stroke(x,y,false, true, tolerance);
}


bool Bezier::intersect(const Circle2D& c, Vector2D& intersection) {
	Graphics g;
	g.bezier(*this);
	cairo_path_t* path = g.get_piecewise_linear();
	Vector2D a(0,0),b(0,0);
	for(int i=0; i<path->num_data; i+= path->data[i].header.length) {
		cairo_path_data_t* p = &path->data[i];
		b.x = p->point.x; b.y = p->point.y;
		if(i!=0) {
			Line2D ab = Line2D(a,b);
			if(ab.intersect(c, intersection)) {cairo_path_destroy(path); return true;}
		}
		a = b;
	}
	cairo_path_destroy(path);
	return false;
}

bool Bezier::intersect(const Rectangle& r, Vector2D& intersection) {
	Graphics g;
	g.bezier(*this);
	cairo_path_t* path = g.get_piecewise_linear();
	Vector2D a(0,0),b(0,0);
	for(int i=path->num_data-2; i>=0; i-= path->data[i].header.length) {
		cairo_path_data_t* p = &path->data[i];
		a.x = p[1].point.x; a.y = p[1].point.y;
		if(i!=path->num_data-2) {
			Line2D ab = Line2D(a,b);
			if(ab.intersect(r, intersection)) {cairo_path_destroy(path); return true;}
		}
		b=a;
	}
	cairo_path_destroy(path);
	return false;
}

std::ostream& operator<<(std::ostream& os, const Vector2D& r) {
	os << r.x << "," << r.y;
	return os;
}

std::ostream& operator<<(std::ostream& os, const Line2D& r) {
	os << r.a << " - " << r.b;
	return os;
}

bool Line2D::intersect(const Rectangle& r, Vector2D& intersection) {
    double t0 = 0.0;    double t1 = 1.0;
    double xdelta = b.x-a.x;
    double ydelta = b.y-a.y;
    double p,q,R;

    for(int edge=0; edge<4; edge++) {   // Traverse through left, right, bottom, top edges.
        if (edge==0) {  p = -xdelta;    q = a.x-r.x;  }
        if (edge==1) {  p = xdelta;     q = ((r.x+r.w)-a.x); }
        if (edge==2) {  p = -ydelta;    q = a.y-r.y;}
        if (edge==3) {  p = ydelta;     q = (r.y+r.h)-a.y;   }
        R = q/p;
        if(p==0 && q<0) return false;   //  (parallel line outside)

        if(p<0) {
            if(R>t1) return false;         // Don't draw line at all.
            else if(R>t0) t0=R;            // Line is clipped!
        } else if(p>0) {
            if(R<t0) return false;      // Don't draw line at all.
            else if(R<t1) t1=R;         // Line is clipped!
        }
    }

    if(t0 <1 && t0 > 0) {
        intersection.x = a.x + t0*xdelta;
        intersection.y = a.y + t0*ydelta;
    	return true;
    }

    if(t1 <1 && t1 > 0) {
        intersection.x = a.x + t1*xdelta;
        intersection.y = a.y + t1*ydelta;
      	return true;
    }
    return false;
}


double Bezier::length() {
	Vector2D a; double l = 0;
	for(double t = 0; t <1 ; t+=0.01) {
		Vector2D b = get(t);
		if(t!=0) {
			l += (b-a).norm();
		}
		a = b;
	}
	return l;
}

Vector2D Bezier::pointOnPath(double d) {
	double l = length(); d*=l;
	double ll = 0;
	Vector2D a;
	for(double t = 0; t<1; t+=0.01) {
		Vector2D b = get(t);
		if(t!=0) {
			double lll = (b-a).norm();
			if(ll + lll > d) {
				return a + (b-a).normalize()*(d-ll);
			}
			ll += lll;
		}
		a = b;
	}
	return a;
}

double Bezier::location(const Vector2D& p) {
	double mind = 0;
	double loc = -1;
	for(double t = 0; t<1; t+=0.01) {
		double d = p.dist_sq(get(t));
		if(loc==-1 || d < mind) {loc = t; mind = d;}
	}
	return loc;
}

double Bezier::location(double d) {
	double l = length(); d*=l;
	double ll = 0;
	Vector2D a;
	for(double t = 0; t<1; t+=0.01) {
		Vector2D b = get(t);
		if(t!=0) {
			double n = (b-a).norm();
			if(ll + n > d) {
				return t + (d-(ll+n))/n*0.01;
			}
			ll += n;
		}
		a = b;
	}
	return 1;
}
