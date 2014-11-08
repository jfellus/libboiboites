/*
 * HTTPServer.cpp
 *
 *  Created on: 13 oct. 2014
 *      Author: jfellus
 */

#include <iostream>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include "HTTPServer.h"
#include <string>
#include "../utils.h"


static int answer_to_connection (void *cls, struct MHD_Connection *connection,
		const char *url, const char *method,
		const char *version, const char *upload_data,
		size_t *upload_data_size, void **con_cls)
{
	std::string s;

	if(!strcmp(method,"POST")) {
		if (*con_cls == NULL) {
			*con_cls = new std::string();
			return MHD_YES;
		}
		if (*upload_data_size) {
			std::string ss(upload_data, *upload_data_size);
			(*((std::string*)*con_cls)) += ss;
			*upload_data_size = 0;
			return MHD_YES;
		}
		else {
			s = ((HTTPServer*)cls)->answer(url, (*((std::string*)*con_cls)));
			delete (std::string*)*con_cls;
			*con_cls = 0;
		}
	}
	else s = ((HTTPServer*)cls)->answer(url, "");

	static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&mut);
	struct MHD_Response *response;
	int ret;
	static char* ss = 0;
	if(ss) delete ss;
	ss = new char[s.length()+1];
	strcpy(ss, s.c_str()); ss[s.length()] = 0;
	response =  MHD_create_response_from_buffer (strlen(ss), ss, MHD_RESPMEM_PERSISTENT);
	MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	MHD_add_response_header(response, "Access-Control-Allow-Methods", "POST, GET, OPTIONS");
	MHD_add_response_header(response, "Access-Control-Allow-Headers", "X-Requested-With");
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	MHD_destroy_response (response);
	pthread_mutex_unlock(&mut);

	return ret;
}


HTTPServer::HTTPServer(int port) {
	this->port = port;
	daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, port, NULL, NULL,
			&answer_to_connection, this, MHD_OPTION_END);
	if (NULL == daemon) throw "Can't init HTTP Server";
}

HTTPServer::~HTTPServer() {
	MHD_stop_daemon (daemon);
}





