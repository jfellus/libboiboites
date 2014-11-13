/*
 * Document.h
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <util/utils.h>
#include <util/selection/ISelectionListener.h>
#include "Module.h"
#include "Link.h"
#include "Group.h"
#include "../components/GroupComponent.h"




class Document : public IPropertiesListener, ISelectionListener {
public:

	///////////////
	// LISTENERS //
	///////////////

	class IDocumentChangeListener {
	public:
		IDocumentChangeListener(){}
		virtual ~IDocumentChangeListener(){}
		virtual void on_document_change() = 0;
	};


public:

	////////////
	// FIELDS //
	////////////

	std::vector<Module*> modules;
	std::vector<Link*> links;

	std::vector<Module*> selected_modules;
	std::vector<Link*> selected_links;

	std::vector<IPropertiesListener*> propertiesListeners;
	std::vector<IDocumentChangeListener*> change_listeners;



public:
	static Document* cur();
	Document();
	virtual ~Document() {}


	// Accessors

	Module* get_module(int i) {return modules[i];}
	Link* get_link(int i) {return links[i];}
	int get_modules_count() {return modules.size();}
	int get_links_count() {return links.size();}
	Group* get_group_at(double x, double y);

	Module* add_module(Module* m);
	void remove_module(Module* m, bool bDeleteConnectedLinks = true);
	Link* add_link(Link* l);
	void remove_link(Link* l);

	void add_properties_listener(IPropertiesListener* l) 	{ propertiesListeners.push_back(l);}
	void add_change_listener(IDocumentChangeListener* l) 	{ change_listeners.push_back(l); }


	// Selection

	void unselect_all();
	void unselect_all_modules();
	void unselect_all_links();
	void delete_selection();


	// Grouping

	void group_selection();
	void ungroup_selected();
	void open_group(Group* g);
	void close_group(Group* g);
	void change_selection_group(Group* g);


	// Tags

	void toggle_selection_tag(int i);
	void add_selection_tag(int i);
	void remove_selection_tag(int i);


	// Events

	virtual void on_dbl_click(ISelectable* s, GdkEventButton* e);
	virtual void on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val);
	virtual void on_selection_event(ISelectable* s);


	// ! Internals !
	void update_links_layers();
	void fire_change_event();

protected:

	virtual void dump(std::ostream& os) {	os << "Document (" << modules.size() << " modules & " << links.size() << " links)";	}
	friend std::ostream& operator<<(std::ostream& os, Document* a);
};

inline std::ostream& operator<<(std::ostream& os, Document* a) {	a->dump(os);return os;}


#endif /* DOCUMENT_H_ */
