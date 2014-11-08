/*
 * Window.cpp
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#include "Window.h"


typedef struct _callbacks {
	static void _on_realize(GtkWidget *widget, gpointer   user_data) { ((Window*)user_data)->on_realize();}
	static gboolean _on_draw(GtkWidget *widget, cairo_t* cr, gpointer   user_data) { return ((Window*)user_data)->on_draw(cr);}
	static void _on_size_allocate(GtkWidget *widget, GdkRectangle* r, gpointer   user_data) { return ((Window*)user_data)->on_size_allocate(r);}

	static gboolean _on_key(GtkWidget *widget,  GdkEvent  *event, gpointer   user_data) {
		return ((Window*)user_data)->on_key((GdkEventKey*)event);
	}
} _callbacks;


Window::Window() : Widget(NULL){
	builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, "gui.xml", NULL);
	widget = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
	gtk_builder_connect_signals (builder, NULL);

	status = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));

	maximize();
	gtk_window_set_default_size(GTK_WINDOW(widget), 800,600);
	g_signal_connect(widget, "destroy", G_CALLBACK(gtk_main_quit), 0);

	canvas = new ZoomableDrawingArea(this);
	gtk_box_pack_end(GTK_BOX(gtk_builder_get_object(builder, "canvas")), canvas->widget, TRUE, TRUE, 0);

	add_tab(properties = new PropertiesForm(), "Properties");

//	infoform = new InfoForm();
//	gtk_notebook_append_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "rightpane")), infoform->widget, gtk_label_new("Infos"));

	gtk_window_set_focus(GTK_WINDOW(widget), canvas->widget);

	g_signal_connect(widget, "realize", G_CALLBACK(_callbacks::_on_realize), this);
//	g_signal_connect(widget, "draw", G_CALLBACK(_callbacks::_on_draw), this);
	g_signal_connect(widget, "size-allocate", G_CALLBACK(_callbacks::_on_size_allocate), this);
	g_signal_connect(widget, "key-press-event", G_CALLBACK(_callbacks::_on_key), this);

}

Window::~Window() {
	g_object_unref (G_OBJECT (builder));
}


void Window::add_tab(Widget* w, const std::string& title) {
	gtk_notebook_append_page(GTK_NOTEBOOK(gtk_builder_get_object(builder, "rightpane")), w->widget, gtk_label_new(title.c_str()));
}

void Window::on_size_allocate(GdkRectangle* alloc) {
}

void Window::close() {gtk_widget_destroy(GTK_WIDGET(widget)); }

gboolean Window::on_key(GdkEventKey* e) {
//	if(e->keyval==GDK_KEY_Left || e->keyval==GDK_KEY_Right || e->keyval==GDK_KEY_Up || e->keyval==GDK_KEY_Down) {
//		gtk_window_get_focus(GTK_WINDOW(widget))
//		return TRUE;
//	}
	return FALSE;
}



void Window::message_box(const std::string& s) {
	GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
	GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW(widget),
									 flags,
									 GTK_MESSAGE_ERROR,
									 GTK_BUTTONS_OK,
									(const gchar*)(s.c_str()), 0);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

bool Window::yes_no_box(const std::string& s) {
	GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
	GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW(widget),
									 flags,
									 GTK_MESSAGE_WARNING,
									 GTK_BUTTONS_YES_NO,
									(const gchar*)(s.c_str()), 0);
	int r = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	return r==GTK_RESPONSE_YES;
}

void Window::set_status(const std::string& text) {
	gtk_statusbar_remove_all(GTK_STATUSBAR(status), 0);
	gtk_statusbar_push(GTK_STATUSBAR(status), 0, text.c_str());
}

