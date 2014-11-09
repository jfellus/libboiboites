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
	widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	canvas = new ZoomableDrawingArea(this);

	GtkWidget* split = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	GtkWidget* mainbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	GtkWidget* b = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	menubar = gtk_menu_bar_new();
	toolbar = gtk_toolbar_new();
	status = gtk_statusbar_new();
	rightpane = gtk_notebook_new();

	gtk_box_pack_start(GTK_BOX(mainbox), menubar, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(mainbox), split, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(mainbox), status, FALSE, TRUE, 0);
		gtk_paned_pack1(GTK_PANED(split), b, TRUE, TRUE);
		gtk_paned_pack2(GTK_PANED(split), rightpane, FALSE, TRUE);
		gtk_box_pack_start(GTK_BOX(b), toolbar, FALSE, TRUE, 0);
		gtk_box_pack_start(GTK_BOX(b), canvas->widget, TRUE, TRUE, 0);


	gtk_container_add (GTK_CONTAINER (widget), mainbox);
	gtk_widget_show_all(widget);

	maximize();
	gtk_window_set_default_size(GTK_WINDOW(widget), 800,600);
	g_signal_connect(widget, "destroy", G_CALLBACK(gtk_main_quit), 0);


	gtk_window_set_focus(GTK_WINDOW(widget), canvas->widget);

	g_signal_connect(widget, "realize", G_CALLBACK(_callbacks::_on_realize), this);
//	g_signal_connect(widget, "draw", G_CALLBACK(_callbacks::_on_draw), this);
	g_signal_connect(widget, "size-allocate", G_CALLBACK(_callbacks::_on_size_allocate), this);
	g_signal_connect(widget, "key-press-event", G_CALLBACK(_callbacks::_on_key), this);

}

Window::~Window() {
	g_object_unref (G_OBJECT (builder));
}

void Window::add_menu(const char* menustr, void (*callback)(), int accelerator_key) {
	GtkWidget* curmenu = menubar;
	GtkWidget* item = NULL;
	bool bSeparator = false;
	char str[512]; strcpy(str, menustr);
	char *curstr = strtok(str, "/>");
	for(int i=0; curstr!=0; i++) {
		if(i!=0) {
			curmenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(item));
			if(!curmenu) {curmenu = gtk_menu_new(); gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), curmenu);}
		}

		item = NULL;
		GList* l = gtk_container_get_children(GTK_CONTAINER(curmenu));
		for(GList * elem = l; elem; elem = elem->next) {
		  GtkMenuItem* e = GTK_MENU_ITEM(elem->data);
		  if(!strcmp(curstr,gtk_menu_item_get_label(e))) item = GTK_WIDGET(e);
		}
		if(item==NULL) {
			if(!strcmp(curstr, "__")) {
				bSeparator = true;
				item = gtk_separator_menu_item_new();
			} else {
				item = gtk_menu_item_new_with_mnemonic(curstr);
			}
			gtk_menu_shell_append(GTK_MENU_SHELL(curmenu), item);
		}
		curstr = strtok(NULL, "/>");
	}

	if(!bSeparator) g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(callback), NULL);
}


void Window::add_tab(Widget* w, const std::string& title) {
	gtk_notebook_append_page(GTK_NOTEBOOK(rightpane), w->widget, gtk_label_new(title.c_str()));
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

