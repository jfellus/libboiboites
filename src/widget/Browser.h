/*
 * Browser.h
 *
 *  Created on: 7 oct. 2014
 *      Author: jfellus
 */

#ifndef BROWSER_H_
#define BROWSER_H_

#include "Widget.h"
#include <webkit/webkit.h>
#include "../util/utils.h"


class Browser : public Widget {
public:
	WebKitWebView* webkitview = 0;
	std::string server_id;
	bool bLoaded = false;
public:
	Browser(const std::string& server_id);
	virtual ~Browser();

	void open(const std::string& file);
	void script(const char* script);

	virtual std::string answer(const std::string& request, const std::string& data) {return "ok"; };

	bool is_loaded();
	virtual void on_load() {}
};

#endif /* BROWSER_H_ */
