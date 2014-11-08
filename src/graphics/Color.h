/*
 * Color.h
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#ifndef COLOR_H_
#define COLOR_H_

#include "../util/utils.h"

#define RGB_BLACK RGB(0,0,0)
#define RGB_WHITE RGB(1,1,1)
#define RGB_RED RGB(1,0,0)
#define RGB_GREEN RGB(0,1,0)
#define RGB_BLUE RGB(0,0,1)
#define RGB_YELLOW RGB(1,0,0)
#define RGB_INVALID RGB(-1,-1,-1)

class RGB {
public:
	unsigned char r,g,b;
public:
	RGB() {this->r = this->g = this->b = 0;}
	RGB(double r, double g, double b) { this->r = (unsigned char)(r*255); this->g = (unsigned char)(g*255); this->b = (unsigned char)(b*255); }
	RGB(uint c) {
		this->r = (unsigned char)(c & 255);
		this->g = (unsigned char)((c >> 8) & 255);
		this->b = (unsigned char)((c >> 16) & 255);
	}

	uint as_packed_RGB() {
		return (((uint)r) << 16) | (((uint)g) << 8) | (((uint)b));
	}

	inline RGB& operator=(const RGB& r) {this->r=r.r; g=r.g; b=r.b; return *this;}
	inline RGB& operator=(uint c) {
		this->r = (unsigned char)(c & 255);
		this->g = (unsigned char)((c >> 8) & 255);
		this->b = (unsigned char)((c >> 16) & 255);
		return *this;
	}

	inline RGB& operator=(const std::string& s) {
		if(s=="white") return (*this) = RGB_WHITE;
		if(s=="blue") return (*this) = RGB_BLUE;
		if(s=="red") return (*this) = RGB_RED;
		if(s=="black") return (*this) = RGB_BLACK;
		if(s=="green") return (*this) = RGB_GREEN;
		if(s[0]=='#') {
			std::istringstream iss(s); iss.get();
			uint i;
			iss >> std::hex >> i;
			(*this) = i; unsigned char tmp = r; r = g; g = b; b = tmp;
			return *this;
		}
		std::istringstream iss(s);
		float _r,_g,_b; char c;
		iss >> _r; iss >> c;
		iss >> _g; iss >> c;
		iss >> _b;
		r = (unsigned char)(_r*255);
		g = (unsigned char)(_g*255);
		b = (unsigned char)(_b*255);
		return *this;
	}


	inline bool operator==(const RGB& c) const {return c.r==r && c.g==g && c.b==b;}
	inline bool operator!=(const RGB& c) const {return !(c == *this);}
};

inline std::ostream& operator<<(std::ostream& os, const RGB& a) {
	return os << "RGB(" << a.r << "," << a.g << "," << a.b << ")";
}







#endif /* COLOR_H_ */
