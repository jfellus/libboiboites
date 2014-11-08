/*
 * DynEngine.h
 *
 *  Created on: 29 oct. 2014
 *      Author: jfellus
 */

#ifndef DYNENGINE_H_
#define DYNENGINE_H_

#include "../util/utils.h"
#include "Body.h"

class DynEngine {
public:
	std::vector<Body*> bodies;
	int iterations = 0;
	double ips = 600;
	double shakeQty = 0;
	bool bPause = false;

public:
	DynEngine() {}
	virtual ~DynEngine() {}

	void add(Body* b) {bodies.push_back(b);b->engine = this;}

	void start();
	void stop() {bStop = true;}
	void pause() {bPause = true;}



	void shake(double qty) {shakeQty = qty;}


protected:
	static void* _start(void* p);
	pthread_t thread = 0;
	bool bStop = false;

	void run() {
			while(!bStop) {	if(!bPause) iteration(); usleep((int)(1000000.0/ips)+1);}
	}

	void iteration() {
		for(uint i=0; i<bodies.size(); i++) {
			bodies[i]->iteration();
		}
	}
};

#endif /* DYNENGINE_H_ */
