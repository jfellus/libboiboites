/*
 * Browser.h
 *
 *  Created on: 7 oct. 2014
 *      Author: jfellus
 */

#ifndef BROWSERS_H_
#define BROWSERS_H_

#include <widget/Widget.h>
#include <webkit/webkit.h>
#include <util/utils.h>


class Browser : public Widget {
public:
	WebKitWebView* webkitview = 0;
	std::string server_id;
	bool bLoaded = false;
	bool bAnswering = false;
	bool bRuningScript = false;

public:
	Browser(const std::string& server_id);
	virtual ~Browser();

	void open(const std::string& file);
	void script(const char* script);

	virtual std::string answer(const std::string& request, const std::string& data) {return "ok"; };

	bool is_loaded();
	virtual void on_load() {}

	virtual void update();
	virtual void do_update() {}
};

#endif /* BROWSERS_H_ */
