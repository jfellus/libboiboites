/*
 * string.cpp
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include "utils.h"
#include <algorithm>

std::string str_replace(std::string subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

bool str_ends_with(const char* s, const char* suffix) {
	return g_str_has_suffix(s, suffix);
}

bool str_starts_with(const char* s, const char* prefix) {
	return g_str_has_prefix(s, prefix);
}

bool str_starts_with(const std::string& s, const std::string& prefix) {
	return g_str_has_prefix(s.c_str(), prefix.c_str());
}

std::string str_after(const std::string& s, const std::string& prefix) {
	return s.substr(s.find(prefix)+prefix.length());
}

std::string str_before(const std::string& s, const std::string& suffix) {
	return s.substr(0,s.find(suffix));
}

std::string str_between(const std::string& s, const std::string& prefix, const std::string& suffix) {
	return str_before(str_after(s,prefix), suffix);
}


char* str_to_lower(const char* s) {
	return g_ascii_strdown (s, -1);
}


void parse_keycode_str(const char* keycode_str, uint* key, uint* modifiers) {
	*key = keycode_str[0];
	*modifiers = 0;

	if(!keycode_str[1]) return;
	if(str_starts_with(keycode_str, "CTRL + ")) {
		keycode_str += strlen("CTRL + ");
		parse_keycode_str(keycode_str, key, modifiers);
		*modifiers |= GDK_CONTROL_MASK;
	}
	else if(str_starts_with(keycode_str, "SHIFT + ")) {
		keycode_str += strlen("SHIFT + ");
		parse_keycode_str(keycode_str, key, modifiers);
		*modifiers |= GDK_SHIFT_MASK;
	}
	else if(str_starts_with(keycode_str, "ALT + ")) {
		keycode_str += strlen("ALT + ");
		parse_keycode_str(keycode_str, key, modifiers);
		*modifiers |= GDK_MOD1_MASK;
	}

	else if(!strcmp(keycode_str, "UP")) *key = GDK_KEY_Up;
	else if(!strcmp(keycode_str, "DOWN")) *key = GDK_KEY_Down;
	else if(!strcmp(keycode_str, "LEFT")) *key = GDK_KEY_Left;
	else if(!strcmp(keycode_str, "RIGHT")) *key = GDK_KEY_Right;
	else if(!strcmp(keycode_str, "PAGEUP")) *key = GDK_KEY_Page_Up;
	else if(!strcmp(keycode_str, "PAGEDOWN")) *key = GDK_KEY_Page_Down;
	else if(!strcmp(keycode_str, "HOME")) *key = GDK_KEY_Home;
	else if(!strcmp(keycode_str, "END")) *key = GDK_KEY_End;
	else if(!strcmp(keycode_str, "F1")) *key = GDK_KEY_F1;
	else if(!strcmp(keycode_str, "F2")) *key = GDK_KEY_F2;
	else if(!strcmp(keycode_str, "F3")) *key = GDK_KEY_F3;
	else if(!strcmp(keycode_str, "F4")) *key = GDK_KEY_F4;
	else if(!strcmp(keycode_str, "F5")) *key = GDK_KEY_F5;
	else if(!strcmp(keycode_str, "F6")) *key = GDK_KEY_F6;
	else if(!strcmp(keycode_str, "F7")) *key = GDK_KEY_F7;
	else if(!strcmp(keycode_str, "F8")) *key = GDK_KEY_F8;
	else if(!strcmp(keycode_str, "F9")) *key = GDK_KEY_F9;
	else if(!strcmp(keycode_str, "F10")) *key = GDK_KEY_F10;
	else if(!strcmp(keycode_str, "F11")) *key = GDK_KEY_F11;
	else if(!strcmp(keycode_str, "F12")) *key = GDK_KEY_F12;
	else if(!strcmp(keycode_str, "ESCAPE")) *key = GDK_KEY_Escape;
	else if(!strcmp(keycode_str, "DELETE")) *key = GDK_KEY_Delete;
	else if(!strcmp(keycode_str, "BACKSPACE")) *key = GDK_KEY_BackSpace;
	else if(!strcmp(keycode_str, "MENU")) *key = GDK_KEY_Menu;
	else if(!strcmp(keycode_str, "ENTER")) *key = GDK_KEY_Return;
}

std::string cwd() { static char s[MAX_PATH]; getcwd(s,MAX_PATH); return std::string(s);}


std::string str_to_lower(const std::string& s) {
	std::string a = s;
	std::transform(a.begin(), a.end(), a.begin(), ::tolower);
	return a;
}


std::string url_decode(const std::string& s) {
	return str_replace(str_replace(s, "%0A", "\n"), "%20", " ");
}

std::string JSON_escape(const std::string& s) {
	return str_replace(str_replace(s, "\"", "\\\""), "\n", "\\n");
}

std::string str_trim(const std::string& s) {
	int i=0;
	while(isspace(s[i])) i++;
	int j=s.length()-1;
	while(isspace(s[j])) j--;
	return s.substr(i,j-i+1);
}


void str_remove(std::string& s, const std::string& what) {
	size_t i = s.find(what);
	if(i!=std::string::npos) s.erase(s.find(what), what.length());
}

