/*
 * DynEngine.cpp
 *
 *  Created on: 29 oct. 2014
 *      Author: jfellus
 */

#include "DynEngine.h"

void* DynEngine::_start(void* p) { ((DynEngine*)p)->run(); return 0;}

void DynEngine::start() {
	pthread_create(&thread, NULL, _start, this);
}
