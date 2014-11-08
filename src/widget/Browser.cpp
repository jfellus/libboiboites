/*
 * Browser.cpp
 *
 *  Created on: 7 oct. 2014
 *      Author: jfellus
 */

#include "Browser.h"

#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include "../util/utils.h"
#include "../util/http/HTTPServer.h"
#include "ZoomableDrawingArea.h"


#undef DEFAULT_PORT
#define DEFAULT_PORT 12215

class BrowserHTTPServer : public HTTPServer {
public:
	std::vector<Browser*> browsers;
public:
	BrowserHTTPServer() : HTTPServer(DEFAULT_PORT){}
	virtual ~BrowserHTTPServer() {}
	virtual std::string answer(const std::string& _url, const std::string& data) {
		std::string url = _url.substr(1);
		std::string server_id = url.substr(0,url.find("/"));
		std::string req = url.substr(url.find("/")+1);
		for(uint i=browsers.size()-1; i>=0; i--) {
			if(server_id == browsers[i]->server_id) return browsers[i]->answer(req, data);
		}
		return url;
	}

	void add_browser(Browser* b) {browsers.push_back(b);}
};
static BrowserHTTPServer* server = 0;
static int nbInstances = 0;

static void on_load (WebKitWebView  *web_view,  WebKitWebFrame *frame,  gpointer        user_data) {
	((Browser*)user_data)->on_load();
	if(ZoomableDrawingArea::cur()) ZoomableDrawingArea::cur()->grab_focus();
}
static void on_load_finished (WebKitWebView  *web_view,  WebKitWebFrame *frame,  gpointer        user_data) {
	((Browser*)user_data)->bLoaded = true;
	((Browser*)user_data)->on_load();
	if(ZoomableDrawingArea::cur()) ZoomableDrawingArea::cur()->grab_focus();
}


Browser::Browser(const std::string& server_id) : Widget(gtk_scrolled_window_new(NULL,NULL)), server_id(server_id) {
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widget), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    webkitview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    add(GTK_WIDGET(webkitview));
    gtk_widget_set_size_request(widget, 300, 0);

    g_signal_connect(G_OBJECT(webkitview), "onload-event", G_CALLBACK(::on_load), this);
    g_signal_connect(G_OBJECT(webkitview), "document-load-finished", G_CALLBACK(::on_load_finished), this);
    g_signal_connect(G_OBJECT(webkitview), "load-finished", G_CALLBACK(::on_load), this);
    g_signal_connect(G_OBJECT(webkitview), "notify::load-status", G_CALLBACK(::on_load), this);


    if(!server) {
    	try {server = new BrowserHTTPServer();}
    	catch(const char* s) {ERROR("Can't initialize browser server : " << s);}
    }
    if(server) server->add_browser(this);
    nbInstances++;
}

Browser::~Browser() {
	nbInstances--;
	if(nbInstances<0 && server) { delete server; server = 0;}
}


void Browser::open(const std::string& file) {
	webkit_web_view_load_uri(webkitview, (std::string("file://") + cwd() + "/" + file + "#server_port=" + toString(DEFAULT_PORT)).c_str());
	if(ZoomableDrawingArea::cur()) ZoomableDrawingArea::cur()->grab_focus();
}

void Browser::script(const char* script) {
	webkit_web_view_execute_script(webkitview, script);
	if(ZoomableDrawingArea::cur()) ZoomableDrawingArea::cur()->grab_focus();
}

bool Browser::is_loaded() {
	return bLoaded;
}
