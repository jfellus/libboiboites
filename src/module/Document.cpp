/*
 * Document.cpp
 *
 *  Created on: 12 oct. 2014
 *      Author: jfellus
 */

#include "Document.h"
#include "../workbench/Workbench.h"


static Document* _cur = 0;

Document* Document::cur() {
	return _cur;
}


Document::Document() {
	_cur = this;
}

std::ostream& operator<<(std::ostream& os, Document* a) {
	a->dump(os);
	return os;
}

void Document::unselect_all() {
	if(Workbench::cur()) Workbench::cur()->unselect_all();
}
