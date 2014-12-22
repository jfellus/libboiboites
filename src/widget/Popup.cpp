/*
 * Popup.cpp
 *
 *  Created on: 21 déc. 2014
 *      Author: jfellus
 */

#include "Popup.h"
#include <gtk/gtk.h>
#include <ZoomableDrawingArea.h>
#include <gdk/gdkscreen.h>

namespace libboiboites {

static gboolean supports_alpha = FALSE;
static void screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
    /* To check if the display supports alpha channels, get the visual */
    GdkScreen *screen = gtk_widget_get_screen(widget);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);

    if (!visual)
    {
        visual = gdk_screen_get_system_visual(screen);
        supports_alpha = FALSE;
    }
    else
    {
        supports_alpha = TRUE;
    }

    gtk_widget_set_visual(widget, visual);
}

static gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer userdata)
{
   cairo_t *new_cr = gdk_cairo_create(gtk_widget_get_window(widget));

    if (supports_alpha)
    {
        cairo_set_source_rgba (new_cr, 0.5,0.5,0.5,1); /* transparent */
    }
    else
    {
        cairo_set_source_rgb (new_cr, 1.0, 1.0, 1.0); /* opaque white */
    }

    /* draw the background */
    cairo_set_operator (new_cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint (new_cr);

    cairo_destroy(new_cr);

    return FALSE;
}

Popup::Popup(const std::string& text) : Widget(gtk_window_new(GTK_WINDOW_POPUP)) {
	content = gtk_label_new(TOSTRING("<b>" << text << "</b>").c_str());
	GdkRGBA color;
	gdk_rgba_parse(&color, "white");
	gtk_widget_override_color(GTK_WIDGET(content), GTK_STATE_FLAG_NORMAL, &color);
	gtk_label_set_use_markup(GTK_LABEL(content), TRUE);
	gtk_window_set_decorated(GTK_WINDOW(widget), FALSE);
	GtkWidget* fixed_container = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(widget), fixed_container);
	gtk_container_add(GTK_CONTAINER(fixed_container), content);
	gtk_widget_set_margin_left(GTK_WIDGET(content), 20);
	gtk_widget_set_margin_top(GTK_WIDGET(content), 10);
	gtk_widget_set_margin_bottom(GTK_WIDGET(content), 10);
	gtk_widget_set_margin_right(GTK_WIDGET(content), 20);
	gtk_widget_set_app_paintable(widget, TRUE);

	g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw), NULL);
	g_signal_connect(G_OBJECT(widget), "screen-changed", G_CALLBACK(screen_changed), NULL);
	screen_changed(widget, NULL, NULL);



	if(ZoomableDrawingArea::cur()) {
		Vector2D p = ZoomableDrawingArea::cur()->get_absolute_position();
		p.y += 20;
		p.x += 10;
		gtk_window_move(GTK_WINDOW(widget), (gint)p.x, (gint)p.y);
	}

	gtk_widget_show_all(widget);
	gtk_widget_set_opacity (GTK_WIDGET (widget), 0.5);
}

Popup::~Popup() {
	gtk_window_close(GTK_WINDOW(widget));
}

} /* namespace libboiboites */
