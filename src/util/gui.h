/*
 * gui.h
 *
 *  Created on: 22 oct. 2014
 *      Author: jfellus
 */

#ifndef GUI_H_
#define GUI_H_

#include <gtk/gtk.h>
#include <string>

std::string open_file_dialog(GtkWidget* parent_window);
std::string save_file_dialog(GtkWidget* parent_window);


#endif /* GUI_H_ */
