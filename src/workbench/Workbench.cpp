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
#include <commands/Command.h>
#include "../commands/CommandCut.h"
#include "../commands/CommandCopy.h"
#include "../commands/CommandPaste.h"
#include "../commands/CommandSpaceSelection.h"
#include "../commands/CommandAlignSelection.h"
#include "Job.h"
#include <X11/Xlib.h>

namespace libboiboites {

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

static void on_new() 	{	Workbench::cur()->new_document();								}
static void on_open() 	{	Workbench::cur()->open();										}
static void on_reopen() {	Workbench::cur()->reopen();										}
static void on_close() 	{	Workbench::cur()->close();										}
static void on_saveas()	{	Workbench::cur()->save_as();									}
static void on_save()	{	Workbench::cur()->save();										}

static void on_undo() {
	JOB_SUBMIT("Undo", {CommandStack::undo(); Workbench::cur()->win->enable_menu("_Edit>_Undo",CommandStack::can_undo());});
}
static void on_redo() {
	JOB_SUBMIT("Redo", {CommandStack::redo(); Workbench::cur()->win->enable_menu("_Edit>_Redo",CommandStack::can_redo());});
}

static void on_group() {	JOB_SUBMIT("Group",  Workbench::cur()->group_selection()); }
static void on_ungroup() {	JOB_SUBMIT("Ungroup", Workbench::cur()->ungroup_selected()); }
static void on_change_group() { JOB_SUBMIT("Change Group", Workbench::cur()->change_group_selected()); }
static void on_delete() {	JOB_SUBMIT("Delete", Workbench::cur()->delete_selection()); }
static void on_create_module() {Workbench::cur()->create_module();}
static void on_create_link()  {Workbench::cur()->create_link();}
static void on_reconnect_link()  {Workbench::cur()->reconnect_link();}

static void on_display_all_modules_details()  {Workbench::cur()->toggle_display_all_modules_details();}

static void on_space_selection(double x, double y, double dx, double dy) {Workbench::cur()->space_selection(-dy);}
static void on_align_selection() {Workbench::cur()->align_selection();}


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

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_t locker = 0;
static int nlocks = 0;
void Workbench::LOCK() {
	if(locker==pthread_self()) {nlocks++;return;}
	pthread_mutex_lock(&mut);
	locker = pthread_self();
//	f_write_line("workbench_locker.txt", GET_STACK_TRACE());
	nlocks++;
}
void Workbench::UNLOCK() {
	nlocks--;
	if(nlocks<=0) {
		locker = 0;
		pthread_mutex_unlock(&mut);
	}
}
bool Workbench::IS_LOCKED() {
	return locker==pthread_self();
}



Workbench::Workbench() {
	_cur = this;
	Job::init();

	win = new Window();
	ERROR_STREAM = new MessageErrorStream();

	win->add_tab(properties = new PropertiesForm(), "Properties");
//	win->add_tab(infoform = new InfoForm(), "Infos");


	// Key shortcuts

	canvas = win->canvas;
	canvas->add_key_listener(new IKeyListener(GDK_KEY_g, GDK_CONTROL_MASK, on_group));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_G, GDK_CONTROL_MASK | GDK_SHIFT_MASK, on_ungroup));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_g, 0, on_change_group));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_Delete, 0, on_delete));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_m, 0, on_create_module));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_l, 0, on_create_link));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_r, 0, on_reconnect_link));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_a, 0, on_align_selection));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_s, GDK_CONTROL_MASK, on_save));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_s, GDK_CONTROL_MASK | GDK_SHIFT_MASK, on_saveas));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_o, GDK_CONTROL_MASK, on_open));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_r, GDK_CONTROL_MASK, on_reopen));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_w, GDK_CONTROL_MASK, on_close));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_n, GDK_CONTROL_MASK, on_new));

	canvas->add_scroll_listener(new IScrollListener(GDK_CONTROL_MASK|GDK_SHIFT_MASK, libboiboites::on_space_selection));

	// Menus

	win->add_menu("_File>_New", on_new);
	win->add_menu("_File>_Open", on_open);
	win->add_menu("_File>_Close", on_close);
	win->add_menu("_File>_Save", on_save);
	win->add_menu("_File>_Save as", on_saveas);
	win->add_menu("_File>__", NULL);
	win->add_menu("_File>_Quit", gtk_main_quit);

	win->add_menu("_Edit>_Undo",on_undo);
		win->enable_menu("_Edit>_Undo",CommandStack::can_undo());
	win->add_menu("_Edit>_Redo",on_redo);
		win->enable_menu("_Edit>_Redo",CommandStack::can_redo());
		class CSL : public CommandStack::Listener {
		public:
			Window* w; CSL(Window* w) : w(w){} virtual ~CSL() {}
			virtual void on_command_stack_change() {
				w->enable_menu("_Edit>_Undo",CommandStack::can_undo());
				w->enable_menu("_Edit>_Redo",CommandStack::can_redo());
			}
		};
		CommandStack::add_listener(new CSL(win));
	win->add_menu("_Edit>__",NULL);
	win->add_menu("_Edit>_Delete selection",on_delete);

	win->add_menu("_Create>_Module", on_create_module);
	win->add_menu("_Create>_Link", on_create_link);
	win->add_menu("_Create>__", NULL);
	win->add_menu("_Create>_Group from selection", on_group);

	win->add_menu("_Group>_Group selection", on_group);
	win->add_menu("_Group>_Ungroup selection", on_ungroup);
	win->add_menu("_Group>__", NULL);
	win->add_menu("_Group>_Move selection to group", on_change_group);

	win->add_menu("_View>Zoom All" , on_zoom_all);
	win->add_menu("_View>__", NULL);
	win->add_menu("_View>Display all modules details", on_display_all_modules_details);


	document = new Document();
	document->add_change_listener(this);
	document->add_properties_listener(this);

	canvas->add_selection_listener(this);
	canvas->add_change_listener(this);
	canvas->add_dbl_click_listener(this);


	g_signal_new("update_workbench", G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0);
	g_signal_connect(G_OBJECT(win->widget), "update_workbench", G_CALLBACK(_on_update), this);
}

Workbench::~Workbench() {	delete win; }


/////////////
// METHODS //
/////////////

void Workbench::run() {	win->show_all();
	gtk_main();
}


void Workbench::update(bool force) {
	if(bPreventUpdating && !force) return;
	STATUS(get_selected_modules()->size() << " modules selected\t\t" << get_selected_links()->size() << " links selected");
	if(canvas) {		canvas->grab_focus();		canvas->repaint(); }
	if(properties) properties->update(get_selected_modules(), get_selected_links());

	if(infoform) infoform->update();
	if(properties) properties->update();
}

void Workbench::set_title(const std::string& title) {
	win->set_title(title);
}


void Workbench::toggle_display_all_modules_details() {
	ModuleComponent::toggle_display_mode();
}

bool Workbench::question(const std::string& msg) {
	return question_dialog(win->widget, msg);
}


////////
// IO //
////////

void Workbench::new_document() {
	JOB_SUBMIT("New document", Workbench::cur()->do_new_document());
}

void Workbench::close() {
	JOB_SUBMIT("Close", Workbench::cur()->do_close());
}


void Workbench::open() {
	std::string filename = open_file_dialog(win->widget);
	if(!filename.empty()) open(filename);
}

void Workbench::reopen() {
	if(!cur_filename.empty()) open(cur_filename);
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

void Workbench::open(const std::string& filename) {
	cur_filename = filename;
	JOB_SUBMIT(TOSTRING("Open " << filename),Workbench::cur()->do_open(Workbench::cur()->cur_filename));
}

void Workbench::save(const std::string& filename) {
	cur_filename = filename;
	JOB_SUBMIT(TOSTRING("Save " << filename),Workbench::cur()->do_save(Workbench::cur()->cur_filename));
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

void Workbench::copy() {
	(new CommandCopy(document))->execute();
}

void Workbench::paste() {
	if(!CommandPaste::cur_pastable) return;
	(new CommandPaste(document, canvas->mousePosDoc.x, canvas->mousePosDoc.y))->execute();
}

void Workbench::cut() {
	(new CommandCut(document))->execute();
}

void Workbench::space_selection(double amount) {
	if(get_selected_modules_count() < 2) return;
	(new CommandSpaceSelection(document, amount*0.1))->execute();
}

void Workbench::align_selection() {
	if(get_selected_modules_count() < 2) return;
	(new CommandAlignSelection(document))->execute();
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

void Workbench::on_dbl_click(Component* c) {
	if(c) {
		win->show_rightpane();
		win->show_tab(0);
	}
	else win->hide_rightpane();
}


}
