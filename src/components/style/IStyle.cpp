/*
 * IStyle.cpp
 *
 *  Created on: 31 oct. 2014
 *      Author: jfellus
 */



#include "IStyle.h"

void IStyle::update(const std::string& css_class) {
	default_style();
	CSSDefinitions::apply_styles(this, css_class);
}
