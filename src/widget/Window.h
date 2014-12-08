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
#include <ZoomableDrawingArea.h>
#include <util/utils.h>

#define GLADE_CALLBACK extern "C" G_MODULE_EXPORT

namespace libboiboites {

class Window : public Widget {
public:

	ZoomableDrawingArea* canvas = 0;
	GtkWidget* status = 0;
	GtkWidget* rightpane = 0;
	GtkWidget* menubar = 0;
	GtkWidget* toolbar = 0;
	GtkWidget* split = 0;

private:
	bool bFirstDraw = true;
	float panPosition = 0.8;

public:
	Window();
	virtual ~Window();

	void maximize() {	gtk_window_maximize(GTK_WINDOW(widget)); }
	void set_title(const std::string& s) { gtk_window_set_title(GTK_WINDOW(widget), s.c_str());}
	void add(GtkWidget* w) { gtk_container_add(GTK_CONTAINER(widget), w); }
	void close();
	void show_all() {gtk_widget_show_all(widget);}

	void add_menu(const char* menustr, void (*callback)(), int pos = -1, int accelerator_key = 0);
	void add_menu(const char* menustr, void (*callback)(GtkMenuItem*,void*), void* param, int pos = -1, int accelerator_key = 0);

	std::string get_menu(const char* menustr, int offset);
	void remove_menu(const char* menustr, int offset);
	int get_menu_pos(const char* menustr);
	void enable_menu(const char* menustr, bool bEnable = true);

	void add_tab(Widget* w, const std::string& title);
	void show_tab(int i);

	void set_status(const std::string& text);

	void message_box(const std::string& s);
	bool yes_no_box(const std::string& message);

	void show_rightpane();
	void hide_rightpane();
	bool step_show_rightpane();
	bool step_hide_rightpane();

protected:
	friend struct _callbacks;
	GtkBuilder *builder;

	virtual void on_realize() {}
	virtual gboolean on_draw(cairo_t* cr) {return FALSE;}
	virtual void on_size_allocate(GdkRectangle* alloc) ;
	virtual gboolean on_key(GdkEventKey* e) ;
};

}


#endif /* WINDOW_H_ */
