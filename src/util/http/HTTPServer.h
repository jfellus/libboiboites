/*
 * HTTPServer.h
 *
 *  Created on: 13 oct. 2014
 *      Author: jfellus
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include <string>

struct MHD_Daemon;

#define DEFAULT_PORT 21235

class HTTPServer {
public:
	  struct MHD_Daemon *daemon;
	  int port;
public:
	HTTPServer(int port = DEFAULT_PORT);
	virtual ~HTTPServer();

	virtual std::string answer(const std::string& url, const std::string& data) = 0;
};




#endif /* HTTPSERVER_H_ */
