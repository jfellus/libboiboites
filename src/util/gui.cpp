/*
 * gui.cpp
 *
 *  Created on: 22 oct. 2014
 *      Author: jfellus
 */


#include "gui.h"


std::string open_file_dialog(GtkWidget* parent_window) {
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	std::string filename;
	dialog = gtk_file_chooser_dialog_new ("Open File",
										  GTK_WINDOW(parent_window),
										  action,
										  "_Cancel",
										  GTK_RESPONSE_CANCEL,
										  "_Open",
										  GTK_RESPONSE_ACCEPT,
										  NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	  {
		char *_filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		_filename = gtk_file_chooser_get_filename (chooser);
		filename = _filename;
		g_free (_filename);
	  }

	gtk_widget_destroy (dialog);
	return filename;
}

std::string save_file_dialog(GtkWidget* parent_window) {
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;

	std::string filename;
	dialog = gtk_file_chooser_dialog_new ("Save as",
										  GTK_WINDOW(parent_window),
										  action,
										  "_Cancel",
										  GTK_RESPONSE_CANCEL,
										  "_Open",
										  GTK_RESPONSE_ACCEPT,
										  NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	  {
		char *_filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		_filename = gtk_file_chooser_get_filename (chooser);
		filename = _filename;
		g_free (_filename);
	  }

	gtk_widget_destroy (dialog);
	return filename;
}
