/*
 * Workbench.cpp
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#include <gtk/gtk.h>
#include "Workbench.h"
#include "../components/LinkCreator.h"
#include "../components/ModuleCreator.h"
#include "../components/ChangeGroupCreator.h"


static Workbench* _cur = 0;
Workbench* Workbench::cur() { return _cur;}


///////////
// DEBUG //
///////////

class MessageErrorStream : public DebugStream {
public:
	std::ostringstream s;
	MessageErrorStream() : DebugStream(s) {}
	virtual ~MessageErrorStream() {}
	virtual void end() {
		Workbench::cur()->win->message_box(s.str());
		s.str(std::string());
	}
};

void workbench_set_status(const std::string& text) {
	if(Workbench::cur()) Workbench::cur()->win->set_status(text);
}



///////////////
// CALLBACKS //
///////////////

GLADE_CALLBACK {
	void on_create_group(GtkToolButton* w, gpointer u) {		Workbench::cur()->create_module();	}
	void on_create_link(GtkToolButton* w, gpointer u) {		Workbench::cur()->create_link();		}
	void on_new(GtkMenuItem* m, gpointer u) {				Workbench::cur()->new_document();		}
	void on_open(GtkMenuItem* m, gpointer u) {				Workbench::cur()->open_dialog();		}
	void on_close(GtkMenuItem* m, gpointer u) {				Workbench::cur()->close();				}
}

static void on_key_saveas()  {Workbench::cur()->save_dialog();}

static void on_key_group() {Workbench::cur()->group_selection();}
static void on_key_ungroup() {Workbench::cur()->ungroup_selected();}
static void on_key_change_group() {Workbench::cur()->change_group_selected();}
static void on_key_delete() {Workbench::cur()->delete_selection();}
static void on_key_create_module() {Workbench::cur()->create_module();}
static void on_key_create_link()  {Workbench::cur()->create_link();}

static void on_key_tag1_on()  {Workbench::cur()->add_selection_tag(1);}
static void on_key_tag2_on()  {Workbench::cur()->add_selection_tag(2);}
static void on_key_tag3_on()  {Workbench::cur()->add_selection_tag(3);}
static void on_key_tag1_off()  {Workbench::cur()->remove_selection_tag(1);}
static void on_key_tag2_off()  {Workbench::cur()->remove_selection_tag(2);}
static void on_key_tag3_off()  {Workbench::cur()->remove_selection_tag(3);}




/////////////////////
// WORKBENCH CLASS //
/////////////////////


Workbench::Workbench() {
	_cur = this;

	win = new Window();
	ERROR_STREAM = new MessageErrorStream();

	properties = win->properties;
	canvas = win->canvas;
	infoform = win->infoform;
	document = new Document();

	document->add_module_selection_listener(this);
	document->add_link_selection_listener(this);
	document->add_properties_listener(this);

	canvas->add_key_listener(new IKeyListener(GDK_KEY_g, GDK_CONTROL_MASK, on_key_group));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_G, GDK_CONTROL_MASK | GDK_SHIFT_MASK, on_key_ungroup));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_g, 0, on_key_change_group));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_Delete, 0, on_key_delete));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_m, 0, on_key_create_module));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_l, 0, on_key_create_link));

	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_1, GDK_CONTROL_MASK, ::on_key_tag1_on));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_2, GDK_CONTROL_MASK, ::on_key_tag2_on));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_3, GDK_CONTROL_MASK, ::on_key_tag3_on));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_End,  GDK_SHIFT_MASK, ::on_key_tag1_off));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_Down,  GDK_SHIFT_MASK, ::on_key_tag2_off));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_Page_Down,  GDK_SHIFT_MASK, ::on_key_tag3_off));

	canvas->add_key_listener(new IKeyListener(GDK_KEY_s, GDK_CONTROL_MASK, on_key_saveas));
}

Workbench::~Workbench() {
	delete win;
}

void Workbench::run() {
	win->show_all();
	gtk_main();
}


void Workbench::change_group_selected() {
	if(!selected_modules.empty()) canvas->start_creator(new ChangeGroupCreator());
}

void Workbench::group_selection() {
	if(selected_modules.size()==1 && dynamic_cast<Group*>(selected_modules[0])!=NULL) {
		ungroup_selected();
	} else {
		document->group_selection();
		canvas->update_layers();
	}
}

void Workbench::ungroup_selected() {
	document->ungroup_selected();
	canvas->update_layers();
}

void Workbench::open_dialog() {
	std::string filename = open_file_dialog(win->widget);
	if(!filename.empty()) open(filename);
}

void Workbench::save_dialog() {
	std::string filename = save_file_dialog(win->widget);
	if(!filename.empty()) {
		while(file_exists(filename)) {
			if(win->yes_no_box(TOSTRING("File " << filename << " exists. Overwrite ?"))) break;
			filename = save_file_dialog(win->widget);
			if(filename.empty()) break;
		}
		if(!filename.empty()) save(filename);
	}
}



void Workbench::unselect_all() {
	canvas->unselect_all();
	while(selected_modules.size()>0) vector_remove(selected_modules, selected_modules[0]);
	while(selected_links.size()>0) vector_remove(selected_links, selected_links[0]);
}
void Workbench::select(Module* m) {m->select();}


void Workbench::start_change_group() {
	canvas->start_creator(new ChangeGroupCreator());
}

void Workbench::on_module_selected(Module* m, bool bSelected) {
	if(!canvas->isSelecting) properties->update(&selected_modules, &selected_links);

	if(bSelected) selected_modules.push_back(m);
	else vector_remove(selected_modules, m);

	if(!canvas->isSelecting) update();
}

void Workbench::delete_selection() {
	canvas->isSelecting = true;
	document->delete_selection();
	canvas->isSelecting = false;
	update();
}

void Workbench::update() {
	STATUS(selected_modules.size() << " modules selected\t\t" << selected_links.size() << " links selected");
	properties->update(&selected_modules, &selected_links);
	if(infoform) infoform->update();
	canvas->grab_focus();
	canvas->repaint();
}

void Workbench::on_link_selected(Link* m, bool bSelected) {
	if(!canvas->isSelecting) properties->update(&selected_modules, &selected_links);

	if(bSelected) selected_links.push_back(m);
	else vector_remove(selected_links, m);
	if(!canvas->isSelecting) update();
}
