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


// Utils

#define STATUS(x) workbench_set_status(TOSTRING(x))
void workbench_set_status(const std::string& text);



class Workbench : public Document::IPropertiesListener, Document::IDocumentChangeListener,
							ZoomableDrawingArea::ISelectionListener, ZoomableDrawingArea::IChangeListener {

public:
	Document* document = 0;

	Window* win = 0;
		ZoomableDrawingArea* canvas;
		PropertiesForm* properties = 0;
		InfoForm* infoform = 0;

	bool bPreventUpdating = false;

public:
	static Workbench* cur();
	Workbench();
	virtual ~Workbench();

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

	void allow_update() {bPreventUpdating = false;}
	void prevent_update() {bPreventUpdating = true;}

	void toggle_display_all_modules_details();

	bool question(const std::string& msg);


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


	// Commands

	virtual void new_document() = 0;
	virtual void close() = 0;
	virtual void save() { save_as(); }
	virtual void open();
	virtual void save_as();

	virtual void open(const std::string& filename) = 0;
	virtual void save(const std::string& filename) = 0;


	virtual void create_module() = 0;
	virtual void create_link() = 0;
	virtual void reconnect_link();

	virtual void copy();
	virtual void paste();
	virtual void cut();


	// Runtime

	virtual void update(bool force = false);
	void run();


	// Events handling

	virtual void on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val);
	virtual void on_selection_change();
	virtual void on_document_change();
	virtual void on_canvas_change();

	// Debug

	virtual void dump(std::ostream& os) {os << "Workbench";}
	friend std::ostream& operator<<(std::ostream& os, Workbench* a);
};

inline std::ostream& operator<<(std::ostream& os, Workbench* a) {a->dump(os);return os;}

#endif /* COEOS_WORKBENCH_H_ */
