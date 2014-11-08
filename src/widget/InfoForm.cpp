/*
 * InfoForm.cpp
 *
 *  Created on: 21 oct. 2014
 *      Author: jfellus
 */

#include "InfoForm.h"
#include <pango-1.0/pango/pango-font.h>
#include <gtk/gtk.h>
#include "../workbench/Workbench.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

static InfoForm* _cur = 0;

class DebugStreamInfoForm : public DebugStream {
public:
	std::ostringstream s;
	DebugStreamInfoForm() : DebugStream(s) {}
	virtual ~DebugStreamInfoForm() {}
	virtual void end() {
		_cur->add_text_dbg(s.str());
		s.str(std::string());
	}
};

InfoForm::InfoForm() : Widget(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)) {
	GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
	GtkWidget* scroll2 = gtk_scrolled_window_new(NULL, NULL);
		txt = gtk_text_view_new ();
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (txt));
	dbg = gtk_text_view_new ();
		buffer_dbg = gtk_text_view_get_buffer (GTK_TEXT_VIEW (dbg));

	set_text("No info");
	gtk_container_add(GTK_CONTAINER(scroll), txt);
	gtk_container_add(GTK_CONTAINER(scroll2), dbg);
	gtk_box_pack_start(GTK_BOX(widget), scroll, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(widget), gtk_label_new("Debug"), FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(widget), scroll2, TRUE, TRUE, 0);
	set_font("Serif 15");

	_cur = this;
	DEBUG_STREAM = new DebugStreamInfoForm();
	update();
}

void InfoForm::set_font(const char* f) {
	PangoFontDescription *font_desc = pango_font_description_from_string (f);
	gtk_widget_modify_font (txt, font_desc);
}


void InfoForm::update() {
	std::ostringstream s;
	s << "\n\n----\n";
	s << "Infos : \n";
	s << "Selected modules : " << Workbench::cur()->selected_modules.size() << "\n";
	s << Workbench::cur()->selected_modules << "\n";
	s << "Selected links : " << Workbench::cur()->selected_links.size() << "\n";
	if(Document::cur()) {
		s << "Document modules : " << Document::cur()->modules.size() << "\n";
		s << "Document links : " << Document::cur()->links.size() << "\n";
	}
	set_text(s.str());

	GtkTextIter i; gtk_text_buffer_get_end_iter(buffer, &i);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(txt), &i, 0, 0, 0, 0);
	repaint();
}
