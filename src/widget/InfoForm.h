/*
 * InfoForm.h
 *
 *  Created on: 21 oct. 2014
 *      Author: jfellus
 */

#ifndef INFOFORM_H_
#define INFOFORM_H_

#include "Widget.h"
#include "../util/utils.h"


class InfoForm : public Widget {
public:
	GtkWidget* txt = 0;
	GtkWidget* dbg = 0;
	GtkTextBuffer *buffer, *buffer_dbg;
public:
	InfoForm();
	virtual ~InfoForm() {

	}

	void set_font(const char* f);

	void add_text(const std::string& s) {
		GtkTextIter i; gtk_text_buffer_get_end_iter(buffer, &i);
		gtk_text_buffer_insert(buffer, &i, s.c_str(), -1);
	}

	void add_text_dbg(const std::string& s) {
		GtkTextIter i; gtk_text_buffer_get_end_iter(buffer_dbg, &i);
		gtk_text_buffer_insert(buffer_dbg, &i, s.c_str(), -1);
		gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(dbg), &i, 0,0,0,0);
		update();
	}

	void set_text(const std::string& s) {	gtk_text_buffer_set_text (buffer, s.c_str(), -1); }

	void update();
};



#endif /* INFOFORM_H_ */
