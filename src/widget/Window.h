/*
 * Window.h
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <gtk/gtk.h>
#include <glib.h>
#include "ZoomableDrawingArea.h"
#include "PropertiesForm.h"
#include "InfoForm.h"
#include "../util/utils.h"

#define GLADE_CALLBACK extern "C" G_MODULE_EXPORT



class Window : public Widget {
public:

	ZoomableDrawingArea* canvas = 0;
	PropertiesForm* properties = 0;
	InfoForm* infoform = 0;

	GtkWidget* status = 0;

private:
	bool bFirstDraw = true;
	float panPosition = 0.8;

public:
	Window();
	virtual ~Window();

	void maximize() {	gtk_window_maximize(GTK_WINDOW(widget)); }
	void set_title(const char* s) { gtk_window_set_title(GTK_WINDOW(widget), s);}
	void add(GtkWidget* w) { gtk_container_add(GTK_CONTAINER(widget), w); }
	void close();
	void show_all() {gtk_widget_show_all(widget);}

	void add_tab(Widget* w, const std::string& title);

	void set_status(const std::string& text);

	void message_box(const std::string& s);
	bool yes_no_box(const std::string& message);

protected:
	friend struct _callbacks;
	GtkBuilder *builder;

	virtual void on_realize() {}
	virtual gboolean on_draw(cairo_t* cr) {return FALSE;}
	virtual void on_size_allocate(GdkRectangle* alloc) ;
	virtual gboolean on_key(GdkEventKey* e) ;
};



#endif /* WINDOW_H_ */
