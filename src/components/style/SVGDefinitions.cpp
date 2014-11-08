/*
 * SVGDefinitions.cpp
 *
 *  Created on: 6 nov. 2014
 *      Author: jfellus
 */

#include "SVGDefinitions.h"

std::vector<std::string> SVGDefinitions::files;


void SVGDefinitions::add(const std::string& path) {
	if(file_is_directory(path)) {
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(path.c_str())) == NULL) return;
		while ((dirp = readdir(dp)) != NULL) {
			if(file_has_ext(dirp->d_name, ".svg")) add(path + "/" + dirp->d_name);
		}
		closedir(dp);
		return;
	} else {
		if(file_exists(path)) files.push_back(file_absolute_path(path));
	}
}

std::string SVGDefinitions::get(const std::string& path) {
	if(files.empty()) return "";
	std::string p = path + ".svg";
	for(int i=files.size()-1; i>=0; i--) {
		if(file_basename(files[i])==p) return files[i];
	}
	return "";
}
