/*
 * Browser.cpp
 *
 *  Created on: 7 oct. 2014
 *      Author: jfellus
 */

#include "Browser.h"

#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <util/utils.h>
#include "../util/http/HTTPServer.h"
#include "ZoomableDrawingArea.h"
#include <semaphore.h>
#include <pthread.h>
#include "../workbench/Workbench.h"


namespace libboiboites {

#undef DEFAULT_PORT
#define DEFAULT_PORT 12215

class BrowserHTTPServer : public HTTPServer {
public:
	std::vector<Browser*> browsers;
	pthread_t thread2 = 0;
	sem_t sem;
	bool bAnswering = false;
	typedef struct { Browser* pending_browser = NULL;	std::string pending_script; } _a;
	std::list<_a> pending;
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
public:
	BrowserHTTPServer() : HTTPServer(DEFAULT_PORT){
		sem_init(&sem, 0, 0);
		pthread_create(&thread2, NULL, _thread2, this);
	}
	virtual ~BrowserHTTPServer() {}
	virtual std::string answer(const std::string& _url, const std::string& data) {
		bAnswering = true;
		std::string url = _url.substr(1);
		size_t i = url.find('/');
		std::string server_id = (i==std::string::npos) ? url : url.substr(0,i);
		std::string req = (i==std::string::npos) ? "" : url.substr(i+1);
		for(uint i=browsers.size()-1; i>=0; i--) {
			if(server_id == browsers[i]->server_id) {
				return browsers[i]->answer(req, data);
			}
		}
		bAnswering = false;
		sem_post(&sem);
		return url;
	}

	void add_browser(Browser* b) {browsers.push_back(b);}

	static void* _thread2(void* arg) {
		BrowserHTTPServer* s = (BrowserHTTPServer*)arg;
		while(true) {
			sem_wait(&s->sem);
			while(!s->pending.empty()) {
				_a p = s->pending.front(); s->pending.pop_front();
				p.pending_browser->script(p.pending_script.c_str());
			}
		}
		return 0;
	}
	void exec_script_soon(Browser* browser, const char* script) {
		_a a; a.pending_browser = browser; a.pending_script = script;
		pending.push_back(a);
		sem_post(&sem);
	}
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
//static void on_call_update_script(gpointer *o, gpointer *p) {
//	((Browser*)p)->do_update();
//}


Browser::Browser(const std::string& server_id) : Widget(gtk_scrolled_window_new(NULL,NULL)), server_id(server_id) {
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widget), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    webkitview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    add(GTK_WIDGET(webkitview));
    gtk_widget_set_size_request(widget, 300, 0);

    g_signal_connect(G_OBJECT(webkitview), "onload-event", G_CALLBACK(libboiboites::on_load), this);
    g_signal_connect(G_OBJECT(webkitview), "document-load-finished", G_CALLBACK(libboiboites::on_load_finished), this);
    g_signal_connect(G_OBJECT(webkitview), "load-finished", G_CALLBACK(libboiboites::on_load), this);
    g_signal_connect(G_OBJECT(webkitview), "notify::load-status", G_CALLBACK(libboiboites::on_load), this);


    if(!server) {
    	try {server = new BrowserHTTPServer();}
    	catch(const char* s) {ERROR("Can't initialize browser server : " << s);}
    }
    if(server) server->add_browser(this);
    nbInstances++;

//    if(!g_signal_lookup("call-update-script", G_TYPE_OBJECT))
//    	g_signal_new("call-update-script", G_TYPE_OBJECT, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);
//    g_signal_connect(G_OBJECT(webkitview), "call-update-script", G_CALLBACK(::on_call_update_script), this);
}

Browser::~Browser() {
	nbInstances--;
	if(nbInstances<0 && server) { delete server; server = 0;}
}


void Browser::open(const std::string& file) {
	webkit_web_view_load_uri(webkitview, TOSTRING("file://" << main_dir() << "/" << file << "#server_port=" << server->port).c_str());
	if(ZoomableDrawingArea::cur()) ZoomableDrawingArea::cur()->grab_focus();
}

void Browser::open_web(const std::string& file) {
	webkit_web_view_load_uri(webkitview,  file.c_str());
	if(ZoomableDrawingArea::cur()) ZoomableDrawingArea::cur()->grab_focus();
}


void Browser::script(const char* script) {
		if(bRuningScript) return;
		else {
			bRuningScript = true;
			webkit_web_view_execute_script(webkitview, script);
			if(ZoomableDrawingArea::cur()) ZoomableDrawingArea::cur()->grab_focus();
			bRuningScript = false;
		}
}

bool Browser::is_loaded() {
	return bLoaded;
}

static int do_update(void* p) {
	Browser* b = (Browser*)p;
	b->do_update();
	b->timeout_update = 0;
	return FALSE;
}

void Browser::update() {
	if(!timeout_update) timeout_update = g_timeout_add(1, libboiboites::do_update, this);
//	g_signal_emit_by_name(G_OBJECT(webkitview), "call-update-script", this);
}

}
