/*
 * SVGDefinitions.h
 *
 *  Created on: 6 nov. 2014
 *      Author: jfellus
 */

#ifndef SVGDEFINITIONS_H_
#define SVGDEFINITIONS_H_

#include "../../util/utils.h"
#include <dirent.h>

class SVGDefinitions {
public:
	static std::vector<std::string> files;
public:
	static void add(const std::string& path);
	static std::string get(const std::string& path);
};

#endif /* SVGDEFINITIONS_H_ */
