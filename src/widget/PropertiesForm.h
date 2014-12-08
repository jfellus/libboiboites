/*
 * PropertiesForm.h
 *
 *  Created on: 9 oct. 2014
 *      Author: jfellus
 */

#ifndef PROPERTIESFORM_H_
#define PROPERTIESFORM_H_

#include "Browser.h"
#include "../module/Property.h"
#include "../module/Module.h"
#include "../module/Link.h"
#include <util/utils.h>


namespace libboiboites {

class PropertiesForm : public Browser {
public:
	Properties* properties = 0;
	Properties* multiproperties = 0;
	std::vector<Module*>* selected_modules = 0;
	std::vector<Link*>* selected_links = 0;

public:
	PropertiesForm(Properties* properties = NULL) : Browser("properties") {
		this->properties = properties;
		open("src/js/properties_form.html");
	}
	virtual ~PropertiesForm() {	}

	void update(std::vector<Module*>* selected_modules, std::vector<Link*>* selected_links);
	virtual void update() { Browser::update(); }

	virtual std::string answer(const std::string& request, const std::string& data);

	void reset();

protected:
	virtual Properties* create_multiproperties(std::vector<Link*>* selection);
	virtual Properties* create_multiproperties(std::vector<Module*>* selection);

};

}

#endif /* PROPERTIESFORM_H_ */

