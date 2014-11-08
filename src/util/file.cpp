/*
 * file.cpp
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#include "file.h"
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ios>
#include "utils.h"

bool file_has_ext(const char* filename, const char* ext) {
	return str_ends_with(str_to_lower(filename),ext);
}

std::string file_basename(const std::string& file) {
	size_t i = file.rfind('/');
	if(i==std::string::npos) return file;
	return file.substr(i+1);
}

std::string file_dirname(const std::string& file) {
	size_t i = file.rfind('/');
	if(i==std::string::npos) return ".";
	else return file.substr(0,i);
}



void fcomeBackToLine(std::istream& f, int linesBefore) {
	while(linesBefore<0) {
		f.seekg(-2, std::ios_base::cur);
		if(!f.good()) {f.clear(); return;}
		int c = f.get();
		if(c=='\n') linesBefore++;
	}
}

bool file_exists(const std::string& filename) {
	  struct stat   buffer;
	  return (stat (filename.c_str(), &buffer) == 0);
}

bool file_is_directory(const std::string& filename) {
	struct stat   buffer;
	if(stat (filename.c_str(), &buffer) != 0) return false;
    return (S_ISDIR (buffer.st_mode));
}

std::string file_absolute_path(const std::string& path) {
	char* s = realpath(path.c_str(), NULL);
	if(!s) return "";
	std::string ss = s;
	free(s);
	return ss;
}

std::string fgetlines(std::istream& f, int nblines) {
	std::string s;
	std::string buf;
	while(nblines) {
		getline(f, buf);
		if(!f.good()) return s;
		s += buf;
		s += '\n';
		nblines--;
	}
	return s;
}

std::string f_read_comments(std::istream& f) {
	  std::string comment = "",com;
	  char c;
	  while (f >> c && (c=='%' || c=='\n')) {
	    if (c == '%') {
	      std::getline(f, com);
	      comment += com;
	      comment += "\n";
	    }
	  }
	  f.seekg(-1, std::ios_base::cur);
	  return comment;
}

void f_write_comments(std::ostream& f, const std::string& comments) {
	if(comments.empty()) return;
	f << "%";
	std::string s = str_replace(comments, "\n", "\n%");
	while(s[s.size()-1]=='%') s = s.substr(0, s.size()-1);
	f << s;
	if(s[s.size()-1]!='\n') f << '\n';
}

void f_skipline(std::ifstream& f) {
	char c;
	while((c=f.get())!=-1 && c!='\n') {}
}

