/*
 * Property.cpp
 *
 *  Created on: 23 oct. 2014
 *      Author: jfellus
 */




#include "Property.h"



std::ostream& operator<<(std::ostream& os, Properties* a) {
	if(!a) {return os << "";}
	a->dump(os);
	return os;
}


std::ostream& operator<<(std::ostream& os, Property* a) {
	if(!a) {return os << "(null)";}
	a->dump(os);
	return os;
}


void IPropertiesElement::set_property(const std::string& name, const std::string& value) {
	properties.set_from_string(name, value);
	for(uint i=0; i<propertiesListeners.size(); i++) {
		propertiesListeners[i]->on_property_change(this, name, value);
	}
}
