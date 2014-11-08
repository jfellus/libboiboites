/*
 * IStyle.h
 *
 *  Created on: 31 oct. 2014
 *      Author: jfellus
 */

#ifndef ISTYLE_H_
#define ISTYLE_H_

#include "CSSDefinitions.h"




class IStyle {
public:
	IStyle() {update("");}
	virtual ~IStyle() {}

	virtual void update(const std::string& css_class);

	virtual const char* name() {return "*";}

	virtual void default_style() {}
	virtual void apply_style(CSSStyle* s) {}
};

#endif /* ISTYLE_H_ */
