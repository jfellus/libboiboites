/*
 * Window.cpp
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#include "Window.h"
#include <semaphore.h>

namespace libboiboites {

typedef struct _callbacks {
	static void _on_realize(GtkWidget *widget, gpointer   user_data) { ((Window*)user_data)->on_realize();}
	static gboolean _on_draw(GtkWidget *widget, cairo_t* cr, gpointer   user_data) { return ((Window*)user_data)->on_draw(cr);}
	static void _on_size_allocate(GtkWidget *widget, GdkRectangle* r, gpointer   user_data) { return ((Window*)user_data)->on_size_allocate(r);}

	static gboolean _on_key(GtkWidget *widget,  GdkEvent  *event, gpointer   user_data) {
		return ((Window*)user_data)->on_key((GdkEventKey*)event);
	}
} _callbacks;


std::string _do_yes_no_box_str;
bool _do_yes_no_box_ret;
sem_t _do_yes_no_box_sem;
static int _do_yes_no_box(void* p) {
	ZoomableDrawingArea::cur()->LOCK();
	Window* w = (Window*)p;
	GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
	GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW(w->widget),
									 flags,
									 GTK_MESSAGE_WARNING,
									 GTK_BUTTONS_YES_NO,
									(const gchar*)(_do_yes_no_box_str.c_str()), 0);
	int r = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	_do_yes_no_box_ret = (r==GTK_RESPONSE_YES);
	sem_post(&_do_yes_no_box_sem);
	ZoomableDrawingArea::cur()->UNLOCK();
	return FALSE;
}
bool Window::yes_no_box(const std::string& s) {
	canvas->LOCK();
	_do_yes_no_box_str = s;
	g_timeout_add(1, _do_yes_no_box, this);
	canvas->UNLOCK();
	sem_wait(&_do_yes_no_box_sem);
	return _do_yes_no_box_ret;
}


Window::Window() : Widget(NULL){
	sem_init(&_do_yes_no_box_sem, 0, 0);

	widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	canvas = new ZoomableDrawingArea(this);

	split = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	GtkWidget* mainbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	GtkWidget* b = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	menubar = gtk_menu_bar_new();
	toolbar = gtk_toolbar_new();
		gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
		gtk_container_set_border_width(GTK_CONTAINER(toolbar), 0);
		gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar),GTK_ICON_SIZE_MENU);
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

int Window::get_menu_pos(const char* menustr) {
	int index = -1;
	GtkWidget* curmenu = menubar;
	GtkWidget* item = NULL;
	char str[512]; strcpy(str, menustr);
	char *curstr = strtok(str, ">");
	for(int i=0; curstr!=0; i++) {
		if(i!=0) {
			curmenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(item));
			if(!curmenu) return -1;
		}

		item = NULL;
		index = 0;
		GList* l = gtk_container_get_children(GTK_CONTAINER(curmenu));
		for(GList * elem = l; elem; elem = elem->next) {
			GtkMenuItem* e = GTK_MENU_ITEM(elem->data);
			if(!strcmp(curstr,gtk_menu_item_get_label(e))) {
				item = GTK_WIDGET(e);
				break;
			}
			else index++;
		}
		if(item==NULL) {return -1;}
		curstr = strtok(NULL, ">");
	}
	if(item==NULL) return -1;
	return index;
}

std::string Window::get_menu(const char* menustr, int offset) {
	GtkWidget* curmenu = menubar;
	GtkWidget* item = NULL;
	char str[512]; strcpy(str, menustr);
	char *curstr = strtok(str, ">");
	for(int i=0; curstr!=0; i++) {
		if(i!=0) {
			curmenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(item));
			if(!curmenu) return "";
		}

		item = NULL;
		GList* l = gtk_container_get_children(GTK_CONTAINER(curmenu));
		for(GList * elem = l; elem; elem = elem->next) {
			GtkMenuItem* e = GTK_MENU_ITEM(elem->data);
			if(item) {
				item = GTK_WIDGET(e);
				offset--;
			}
			else if(!strcmp(curstr,gtk_menu_item_get_label(e))) {
				item = GTK_WIDGET(e);
				curstr = strtok(NULL, ">");
				if(curstr!=0) break;
			}
			if(offset<0) break;
		}
		if(item==NULL) {return "";}
	}
	if(item==NULL) return "";
	return GTK_IS_SEPARATOR_MENU_ITEM(GTK_MENU_ITEM(item))  ? "__" : gtk_menu_item_get_label(GTK_MENU_ITEM(item));
}

void Window::remove_menu(const char* menustr, int offset) {
	GtkWidget* curmenu = menubar;
	GtkWidget* item = NULL;
	char str[512]; strcpy(str, menustr);
	char *curstr = strtok(str, ">");
	for(int i=0; curstr!=0; i++) {
		if(i!=0) {
			curmenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(item));
			if(!curmenu) return;
		}

		item = NULL;
		GList* l = gtk_container_get_children(GTK_CONTAINER(curmenu));
		for(GList * elem = l; elem; elem = elem->next) {
			GtkMenuItem* e = GTK_MENU_ITEM(elem->data);
			if(item) {
				item = GTK_WIDGET(e);
				offset--;
			}
			else if(!strcmp(curstr,gtk_menu_item_get_label(e))) {
				item = GTK_WIDGET(e);
				curstr = strtok(NULL, ">");
				if(curstr!=0) break;
			}
			if(offset<0) break;
		}
		if(item==NULL) {return;}
	}
	if(item==NULL) return;
	gtk_container_remove(GTK_CONTAINER(curmenu), item);
}

void Window::add_toolbar(const std::string& name, const std::string& btn_icon, void (*callback)(GtkToolItem*, void*), void* param) {
	GtkWidget* icon = gtk_image_new_from_file(btn_icon.c_str());
	GtkToolItem* b = gtk_tool_button_new(icon, name.c_str());
	gtk_container_set_border_width(GTK_CONTAINER(b), 0);
	gtk_widget_set_size_request(GTK_WIDGET(b), 20,20);
	gtk_widget_set_tooltip_text(GTK_WIDGET(b), name.c_str());
	g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(callback), param);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), b, -1);
}


void Window::add_toolbar(const std::string& name, const std::string& btn_icon, void (*callback)(GtkToolItem*, void*), void* param, int before) {
	GtkWidget* icon = gtk_image_new_from_file(btn_icon.c_str());
	GtkToolItem* b = gtk_tool_button_new(icon, name.c_str());
	gtk_container_set_border_width(GTK_CONTAINER(b), 0);
	gtk_widget_set_size_request(GTK_WIDGET(b), 20,20);
	gtk_widget_set_tooltip_text(GTK_WIDGET(b), name.c_str());
	g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(callback), param);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), b, before);
}

void Window::add_toolbar(const std::string& name, const std::string& btn_icon, void (*callback)()) {
	GtkWidget* icon = gtk_image_new_from_file(btn_icon.c_str());
	GtkToolItem* b = gtk_tool_button_new(icon, name.c_str());
	gtk_container_set_border_width(GTK_CONTAINER(b), 0);
	gtk_widget_set_size_request(GTK_WIDGET(b), 20,20);
	gtk_widget_set_tooltip_text(GTK_WIDGET(b), name.c_str());
	g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(callback), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), b, -1);
}

void Window::add_toolbar(const std::string& name) {
	GtkToolItem* b = gtk_separator_tool_item_new();
	gtk_container_set_border_width(GTK_CONTAINER(b), 0);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), b, -1);
}

void Window::add_toolbar(const std::string& name, const std::string& btn_icon, void (*callback)(), int before) {
	GtkWidget* icon = gtk_image_new_from_file(btn_icon.c_str());
	GtkToolItem* b = gtk_tool_button_new(icon, name.c_str());
	gtk_container_set_border_width(GTK_CONTAINER(b), 0);
	gtk_widget_set_size_request(GTK_WIDGET(b), 20,20);
	gtk_widget_set_tooltip_text(GTK_WIDGET(b), name.c_str());
	g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(callback), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), b, before);
}

int Window::get_toolbar_pos(const std::string& name) {
	GList* l = gtk_container_get_children(GTK_CONTAINER(toolbar));
	int i=0;
	for(GList * elem = l; elem; elem = elem->next) {
		if(!GTK_IS_TOOL_BUTTON(elem->data)) {i++; continue;}
		if(name==gtk_tool_button_get_label(GTK_TOOL_BUTTON(elem->data))) return i;
		i++;
	}
	return -1;
}

void Window::enable_toolbar(const std::string& name, bool bEnable) {
	GList* l = gtk_container_get_children(GTK_CONTAINER(toolbar));
	for(GList * elem = l; elem; elem = elem->next) {
		if(name==gtk_tool_button_get_label(GTK_TOOL_BUTTON(l->data)))
			{gtk_widget_set_sensitive(GTK_WIDGET(l->data), bEnable); return;}
	}
}



void Window::add_menu(const char* menustr, void (*callback)(), int pos, int accelerator_key) {
	GtkWidget* curmenu = menubar;
	GtkWidget* item = NULL;
	bool bSeparator = false;
	char str[512]; strcpy(str, menustr);
	char *curstr = strtok(str, ">");
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
			if(pos==-1) {	gtk_menu_shell_append(GTK_MENU_SHELL(curmenu), item); }
			else { gtk_menu_shell_insert(GTK_MENU_SHELL(curmenu), item, pos); }
		}
		curstr = strtok(NULL, ">");
	}
	gtk_widget_show_all(curmenu);
	gtk_widget_queue_draw(curmenu);
	gtk_widget_show_all(item);
	if(!bSeparator) g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(callback), NULL);
}

void Window::add_menu(const char* menustr, void (*callback)(GtkMenuItem*, void*), void* param, int pos, int accelerator_key) {
	GtkWidget* curmenu = menubar;
	GtkWidget* item = NULL;
	bool bSeparator = false;
	char str[512]; strcpy(str, menustr);
	char *curstr = strtok(str, ">");
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
			if(pos==-1) {	gtk_menu_shell_append(GTK_MENU_SHELL(curmenu), item); }
			else { gtk_menu_shell_insert(GTK_MENU_SHELL(curmenu), item, pos); }
		}
		curstr = strtok(NULL, ">");
	}
	gtk_widget_show_all(curmenu);
	gtk_widget_queue_draw(curmenu);
	gtk_widget_show_all(item);
	if(!bSeparator) g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(callback), param);
}

void Window::enable_menu(const char* menustr, bool bEnable) {
	int index = -1;
	GtkWidget* curmenu = menubar;
	GtkWidget* item = NULL;
	char str[512]; strcpy(str, menustr);
	char *curstr = strtok(str, ">");
	for(int i=0; curstr!=0; i++) {
		if(i!=0) {
			curmenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(item));
			if(!curmenu) return;
		}

		item = NULL;
		index = 0;
		GList* l = gtk_container_get_children(GTK_CONTAINER(curmenu));
		for(GList * elem = l; elem; elem = elem->next) {
			GtkMenuItem* e = GTK_MENU_ITEM(elem->data);
			if(!strcmp(curstr,gtk_menu_item_get_label(e))) {
				item = GTK_WIDGET(e);
				break;
			}
			else index++;
		}
		if(item==NULL) return;
		curstr = strtok(NULL, ">");
	}
	if(item==NULL) return;
	gtk_widget_set_sensitive(GTK_WIDGET(item), bEnable);
}


void Window::add_tab(Widget* w, const std::string& title) {
	gtk_notebook_append_page(GTK_NOTEBOOK(rightpane), w->widget, gtk_label_new(title.c_str()));
}

void Window::show_tab(int i) {
	gtk_notebook_set_current_page(GTK_NOTEBOOK(rightpane), i);
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

bool Window::step_show_rightpane() {
	int fullw = gtk_widget_get_allocated_width(rightpane);
	int w = gtk_paned_get_position(GTK_PANED(split));
	int maxw = gdk_window_get_width(gtk_widget_get_window(gtk_widget_get_parent(split)));
	float v = ((maxw-fullw) - w)*0.2; if(v>-1) v = -1;
	gtk_paned_set_position(GTK_PANED(split), (int)(w+v));
	return (w - (maxw-fullw)) > 10;
}

bool Window::step_hide_rightpane() {
	int w = gtk_paned_get_position(GTK_PANED(split));
	int maxw = gdk_window_get_width(gtk_widget_get_window(gtk_widget_get_parent(split)));
	float v = (maxw - w)*0.2; if(v<1) v = 1;
	gtk_paned_set_position(GTK_PANED(split), (int)(w+v));
	return maxw - w > 10;
}

int _th_show_rightpane_id = 0;
int _th_hide_rightpane_id = 0;
static int _th_show_rightpane(void* data) {
	Window* w = (Window*)data;
	if(!w->step_show_rightpane() || _th_show_rightpane_id == 0) return FALSE;
	return TRUE;
}

static int _th_hide_rightpane(void* data) {
	Window* w = (Window*)data;
	if(!w->step_hide_rightpane() || _th_hide_rightpane_id == 0) return FALSE;
	return TRUE;
}


void Window::show_rightpane() {
	if(_th_show_rightpane_id) return;
	if(_th_hide_rightpane_id) _th_hide_rightpane_id = 0;
	_th_show_rightpane_id = 1;
	_th_show_rightpane_id = g_timeout_add(50, _th_show_rightpane, this);
}

void Window::hide_rightpane() {
	if(_th_hide_rightpane_id) return;
	if(_th_show_rightpane_id) _th_show_rightpane_id = 0;
	_th_hide_rightpane_id = 1;
	_th_hide_rightpane_id = g_timeout_add(50, _th_hide_rightpane, this);
}


std::string _do_message_box_str;
static int _do_message_box(void* p) {
	ZoomableDrawingArea::cur()->LOCK();
	Window* w = (Window*)p;
	GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
	GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW(w->widget),
									 flags,
									 GTK_MESSAGE_ERROR,
									 GTK_BUTTONS_OK,
									(const gchar*)(_do_message_box_str.c_str()), 0);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	ZoomableDrawingArea::cur()->UNLOCK();
	return FALSE;
}

void Window::message_box(const std::string& s) {
	canvas->LOCK();
	_do_message_box_str = s;
	g_timeout_add(1, _do_message_box, this);
	canvas->UNLOCK();
}


std::string _do_set_status_text;
static int _do_set_status(void* p) {
	Window* w = (Window*)p;
	w->canvas->LOCK();
	gtk_statusbar_remove_all(GTK_STATUSBAR(w->status), 0);
	gtk_statusbar_push(GTK_STATUSBAR(w->status), 0, _do_set_status_text.c_str());
	w->canvas->UNLOCK();
	return FALSE;
}
void Window::set_status(const std::string& text) {
	canvas->LOCK();
	g_timeout_add(1, _do_set_status, this);
	canvas->UNLOCK();
}


}
