/*
 * Popup.h
 *
 *  Created on: 21 déc. 2014
 *      Author: jfellus
 */

#ifndef POPUP_H_
#define POPUP_H_

#include <widget/Widget.h>
#include <util/utils.h>

namespace libboiboites {

class Popup : public Widget {
public:
	GtkWidget* content = NULL;
public:
	Popup(const std::string& text);
	virtual ~Popup();

};

} /* namespace libboiboites */

#endif /* POPUP_H_ */
