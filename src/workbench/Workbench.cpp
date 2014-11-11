/*
 * Workbench.cpp
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#include <gtk/gtk.h>
#include "Workbench.h"
#include "../creators/LinkCreator.h"
#include "../creators/ModuleCreator.h"
#include "../creators/ChangeGroupCreator.h"
#include "../creators/LinkReconnectCreator.h"
#include "../widget/PropertiesForm.h"
#include "../widget/InfoForm.h"
#include <semaphore.h>


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

static void on_new() 	{	Workbench::cur()->new_document();	}
static void on_open() 	{	Workbench::cur()->open();			}
static void on_close() 	{	Workbench::cur()->close();			}
static void on_saveas()	{	Workbench::cur()->save_as();	}

static void on_group() {Workbench::cur()->group_selection();}
static void on_ungroup() {Workbench::cur()->ungroup_selected();}
static void on_change_group() {Workbench::cur()->change_group_selected();}
static void on_delete() {Workbench::cur()->delete_selection();}
static void on_create_module() {Workbench::cur()->create_module();}
static void on_create_link()  {Workbench::cur()->create_link();}
static void on_reconnect_link()  {Workbench::cur()->reconnect_link();}


static void on_key_tag1_on()  {Workbench::cur()->add_selection_tag(1);}
static void on_key_tag2_on()  {Workbench::cur()->add_selection_tag(2);}
static void on_key_tag3_on()  {Workbench::cur()->add_selection_tag(3);}
static void on_key_tag1_off()  {Workbench::cur()->remove_selection_tag(1);}
static void on_key_tag2_off()  {Workbench::cur()->remove_selection_tag(2);}
static void on_key_tag3_off()  {Workbench::cur()->remove_selection_tag(3);}

static void on_zoom_all()  {Workbench::cur()->canvas->zoom_all();}

static bool _bUpdating = false;
static void _on_update() {
	_bUpdating = true;
	Workbench::cur()->update(true);
	_bUpdating = false;
}



/////////////////////
// WORKBENCH CLASS //
/////////////////////


Workbench::Workbench() {
	_cur = this;

	win = new Window();
	ERROR_STREAM = new MessageErrorStream();

	win->add_tab(properties = new PropertiesForm(), "Properties");
//	win->add_tab(infoform = new InfoForm(), "Infos");

	canvas = win->canvas;
	canvas->add_key_listener(new IKeyListener(GDK_KEY_g, GDK_CONTROL_MASK, on_group));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_G, GDK_CONTROL_MASK | GDK_SHIFT_MASK, on_ungroup));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_g, 0, on_change_group));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_Delete, 0, on_delete));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_m, 0, on_create_module));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_l, 0, on_create_link));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_r, 0, on_reconnect_link));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_1, GDK_CONTROL_MASK, ::on_key_tag1_on));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_2, GDK_CONTROL_MASK, ::on_key_tag2_on));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_3, GDK_CONTROL_MASK, ::on_key_tag3_on));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_End,  GDK_SHIFT_MASK, ::on_key_tag1_off));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_Down,  GDK_SHIFT_MASK, ::on_key_tag2_off));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_KP_Page_Down,  GDK_SHIFT_MASK, ::on_key_tag3_off));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_s, GDK_CONTROL_MASK, on_saveas));

	win->add_menu("_File>_New", on_new);
	win->add_menu("_File>_Open", on_open);
	win->add_menu("_File>_Close", on_close);
	win->add_menu("_File>_Save", on_saveas);
	win->add_menu("_File>_Save as", on_saveas);
	win->add_menu("_File>__", NULL);
	win->add_menu("_File>_Quit", gtk_main_quit);
	win->add_menu("_Create>_Module", on_create_module);
	win->add_menu("_Create>_Link", on_create_link);
	win->add_menu("_Create>__", NULL);
	win->add_menu("_Create>_Group from selection", on_group);
	win->add_menu("_Edit>_Delete selection",on_delete);
	win->add_menu("_View>Zoom All" , on_zoom_all);
	win->add_menu("_View>__", NULL);
	win->add_menu("_View>Add tag 1 to selection" , on_key_tag1_on);
	win->add_menu("_View>Add tag 2 to selection" , on_key_tag2_on);
	win->add_menu("_View>Add tag 3 to selection" , on_key_tag2_on);
	win->add_menu("_View>Remove tag 1 to selection" , on_key_tag1_off);
	win->add_menu("_View>Remove tag 2 to selection" , on_key_tag2_off);
	win->add_menu("_View>Remove tag 3 to selection" , on_key_tag3_off);
	win->add_menu("_Group>_Group selection", on_group);
	win->add_menu("_Group>_Ungroup selection", on_ungroup);
	win->add_menu("_Group>__", NULL);
	win->add_menu("_Group>_Move selection to group", on_change_group);

	document = new Document();
	document->add_change_listener(this);
	document->add_properties_listener(this);
	canvas->add_selection_listener(this);
	canvas->add_change_listener(this);

	g_signal_new("update_workbench", G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0);
	g_signal_connect(G_OBJECT(win->widget), "update_workbench", G_CALLBACK(_on_update), this);
}

Workbench::~Workbench() {	delete win; }


/////////////
// METHODS //
/////////////

void Workbench::run() {	win->show_all();	gtk_main();}


void Workbench::update(bool force) {
	STATUS(get_selected_modules()->size() << " modules selected\t\t" << get_selected_links()->size() << " links selected");
	if(canvas) {		canvas->grab_focus();		canvas->repaint(); }
	if(properties) properties->update(get_selected_modules(), get_selected_links());

	if(bPreventUpdating && !force) return;
	if(infoform) infoform->update();
	if(properties) properties->update();
}


////////
// IO //
////////


void Workbench::open() {
	std::string filename = open_file_dialog(win->widget);
	if(!filename.empty()) open(filename);
}

void Workbench::save_as() {
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



///////////////
// SELECTION //
///////////////

void Workbench::unselect_all() {
	document->unselect_all();
	canvas->unselect_all();
	update();
}

void Workbench::unselect_all_modules() {
	document->unselect_all_modules();
	update();
}

void Workbench::unselect_all_links() {
	document->unselect_all_links();
	update();
}



//////////////
// COMMANDS //
//////////////

void Workbench::change_group_selected() {
	if(!get_selected_modules()->empty()) canvas->start_creator(new ChangeGroupCreator());
}

void Workbench::group_selection() {
	if(get_selected_modules()->size()==1 && dynamic_cast<Group*>((*get_selected_modules())[0])!=NULL) {
		ungroup_selected();
	} else {
		document->group_selection();
		canvas->update_layers();
	}
	update();
}

void Workbench::ungroup_selected() {
	document->ungroup_selected();
	canvas->update_layers();
	update();
}

void Workbench::delete_selection() {
	canvas->isSelecting = true;
	document->delete_selection();
	canvas->isSelecting = false;
	update();
}

void Workbench::reconnect_link() {
	if(get_selected_modules()->empty() && get_selected_links()->size()==1) {
		canvas->start_creator(new LinkReconnectCreator((*get_selected_links())[0]));
	}
}


////////////
// EVENTS //
////////////


void Workbench::on_selection_change() {
	update();
}

void Workbench::on_document_change() {
	update();
}

void Workbench::on_canvas_change() {
	update();
}

void Workbench::on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val) {
	canvas->repaint();
}


