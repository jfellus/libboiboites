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
#include "../util/gui.h"


// Utils

#define STATUS(x) workbench_set_status(TOSTRING(x))
void workbench_set_status(const std::string& text);



class Workbench : public IModuleSelectionListener, ILinkSelectionListener, IPropertiesListener {
public:
	Document* document = 0;

	Window* win = 0;
		ZoomableDrawingArea* canvas;
		PropertiesForm* properties = 0;
		InfoForm* infoform = 0;

	std::vector<Module*> selected_modules;
	std::vector<Link*> selected_links;

	static Workbench* cur();
public:
	Workbench();
	virtual ~Workbench();


	// Selection

	void select(Module* m);
	void unselect_all();

	void delete_selection();

	inline void toggle_selection_tag(int i) {	Document::cur()->toggle_selection_tag(i);	canvas->repaint();}
	inline void add_selection_tag(int i) 	{	Document::cur()->add_selection_tag(i);	canvas->repaint(); }
	inline void remove_selection_tag(int i) {	Document::cur()->remove_selection_tag(i); canvas->repaint();}

	void change_group_selected();
	void start_change_group();
	void group_selection();
	void ungroup_selected();


	// Commands

	virtual void new_document() = 0;
	void open_dialog();
	virtual void open(const std::string& filename) = 0;
	void save_dialog();
	virtual void save(const std::string& filename) = 0;
	virtual void close() = 0;

	virtual void create_module() = 0;
	virtual void create_link() = 0;


	// Runtime

	virtual void update();
	void run();


	// Events handling

	virtual void on_module_selected(Module* m, bool bSelected);
	virtual void on_link_selected(Link* m, bool bSelected);
	virtual void on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val) {	canvas->repaint();	}


	// Debug

	virtual void dump(std::ostream& os) {os << "Workbench";}
	friend std::ostream& operator<<(std::ostream& os, Workbench* a);
};

inline std::ostream& operator<<(std::ostream& os, Workbench* a) {a->dump(os);return os;}

#endif /* COEOS_WORKBENCH_H_ */
