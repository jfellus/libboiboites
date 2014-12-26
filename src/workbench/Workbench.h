/*
 * Workbench.h
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#ifndef COEOS_WORKBENCH_H_
#define COEOS_WORKBENCH_H_

#include "../widget/Window.h"
#include "../module/Document.h"
#include <util/gui.h>
#include "../widget/PropertiesForm.h"
#include "../widget/InfoForm.h"

namespace libboiboites {

// Utils

#define STATUS(x) workbench_set_status(TOSTRING(x))
void workbench_set_status(const std::string& text);

#define POPUP(x) _POPUP(TOSTRING(x))
void _POPUP(const std::string& msg);

class Workbench : public Document::IPropertiesListener, Document::IDocumentChangeListener, Window::IQuitListener,
							ZoomableDrawingArea::ISelectionListener, ZoomableDrawingArea::IChangeListener, ZoomableDrawingArea::IDblClickListener {

public:
	Document* document = 0;

	Window* win = 0;
		ZoomableDrawingArea* canvas;
		PropertiesForm* properties = 0;
		InfoForm* infoform = 0;

	bool bPreventUpdating = false;

	std::string application_name = "";

public:
	static Workbench* cur();
	Workbench();
	virtual ~Workbench();

	void LOCK();
	void UNLOCK();
	bool IS_LOCKED();

	// Accessors

	std::vector<Module*>* get_selected_modules() {return &Document::cur()->selected_modules;}
	std::vector<Link*>* get_selected_links() {return &Document::cur()->selected_links;}
	inline uint get_selected_links_count() {return get_selected_links()->size();}
	inline uint get_selected_modules_count() {return get_selected_modules()->size();}
	Module* get_single_selected_module() {
		if(get_selected_links_count()!=0) return NULL;
		if(get_selected_modules_count()!=1) return NULL;
		return (*get_selected_modules())[0];
	}
	Link* get_single_selected_links() {
			if(get_selected_modules_count()!=0) return NULL;
			if(get_selected_links_count()!=1) return NULL;
			return (*get_selected_links())[0];
	}

	void set_title(const std::string& title);
	void update_title();
	virtual void do_update_title();

	void allow_update() {bPreventUpdating = false;}
	void prevent_update() {bPreventUpdating = true;}

	void toggle_display_all_modules_details();

	bool question(const std::string& msg);
	bool ask_save_modifications();
	void popup(const std::string& msg);

	// Selection

	void unselect_all();
	void unselect_all_modules();
	void unselect_all_links();


	// Transformations

	void delete_selection();

	inline void toggle_selection_tag(int i) {	Document::cur()->toggle_selection_tag(i);	canvas->repaint();}
	inline void add_selection_tag(int i) 	{	Document::cur()->add_selection_tag(i);	canvas->repaint(); }
	inline void remove_selection_tag(int i) {	Document::cur()->remove_selection_tag(i); canvas->repaint();}

	void change_group_selected();
	void group_selection();
	void ungroup_selected();

	void space_selection(double amount);
	void align_selection();

	// Commands

	virtual void new_document();
	virtual void close();
	virtual void save() { save_as(); }
	virtual void open();
	virtual void reopen();
	virtual void save_as();
	void open(const std::string& filename);
	void save(const std::string& filename);

	virtual void do_new_document() = 0;
	virtual void do_close() = 0;
	virtual void do_open(const std::string& filename) = 0;
	virtual void do_save(const std::string& filename) = 0;


	virtual void create_module() = 0;
	virtual void create_link() = 0;
	virtual void reconnect_link();

	virtual void copy();
	virtual void paste();
	virtual void cut();

	virtual void edit(bool bEdit = true);


	// Runtime

	void update();
	virtual void do_update();
	void run();


	// Events handling

	virtual void on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val);
	virtual void on_selection_change();
	virtual void on_document_change();
	virtual void on_canvas_change();
	virtual void on_dbl_click(Component* c);
	virtual bool on_quit();

	// Debug

	virtual void dump(std::ostream& os) {os << "Workbench";}
	friend std::ostream& operator<<(std::ostream& os, Workbench* a);
};

inline std::ostream& operator<<(std::ostream& os, Workbench* a) {a->dump(os);return os;}

}

#endif /* COEOS_WORKBENCH_H_ */


