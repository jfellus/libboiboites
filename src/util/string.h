/*
 * string.h
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#ifndef COEOS_STRING_H_
#define COEOS_STRING_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sstream>


#define MAX_PATH 1024

bool str_ends_with(const char* s, const char* suffix);
bool str_starts_with(const char* s, const char* prefix);
bool str_starts_with(const std::string& s, const std::string& prefix);
std::string str_after(const std::string& s, const std::string& prefix);
std::string str_before(const std::string& s, const std::string& suffix);
std::string str_between(const std::string& s, const std::string& prefix, const std::string& suffix);
char* str_to_lower(const char* s);
std::string str_replace(std::string subject, const std::string& search, const std::string& replace);

std::string cwd();

void parse_keycode_str(const char* keycode_str, uint* key, uint* modifers);

#define TOSTRING(x) ((std::ostringstream&)(std::ostringstream() << x)).str()

template<typename T> std::string toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<typename T> void fromString(const std::string& s, T& v) {
    std::istringstream iss(s);
    iss >> v;
}

inline void fromString(const std::string& s, std::string& v) {v = s;}

std::string str_to_lower(const std::string& s);


#define JSON_P(prop, value) "\"" << prop << "\": \"" << toString(value) << "\""


std::string url_decode(const std::string& s);
std::string JSON_escape(const std::string& s);

std::string str_trim(const std::string& s);


/**
 * input : std::string
 * separator : char
 * s : variable name (each token will be available with this variable as a std::string)
 */
#define FOR_EACH_TOKEN(input, separator, s) 				\
		size_t ____i__=0,____j__=-1;									\
		std::string s;										\
		for(__for_each_token_init(____i__,____j__,s,input, separator);			\
				__for_each_token_cond(____i__);		\
				__for_each_token_update(____i__,____j__,s,input, separator))

inline int __for_each_token_init(size_t& i, size_t& j, std::string& s, const std::string& arg, char tok) {
	i=0;
	j=arg.find(tok, i);
	s = j==std::string::npos ? arg.substr(i,-1) : arg.substr(i,j-i);
	return 0;
}

inline int __for_each_token_cond(size_t& i) {
	return i!=std::string::npos;
}

inline int __for_each_token_update(size_t& i, size_t& j, std::string& s, const std::string& arg, char tok) {
	i = j==std::string::npos ? j : j+1;
	if(j!=std::string::npos) {
		j=arg.find(tok, i);
		s = j==std::string::npos ? arg.substr(i,-1) : arg.substr(i,j-i);
	}
	return 0;
}


void str_remove(std::string& s, const std::string& what);

#endif /* COEOS_STRING_H_ */
