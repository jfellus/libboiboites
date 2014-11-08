/*
 * CSSDefinitions.h
 *
 *  Created on: 31 oct. 2014
 *      Author: jfellus
 */

#ifndef CSSDEFINITIONS_H_
#define CSSDEFINITIONS_H_

#include "../../util/utils.h"
#include <map>
#include <list>


class IStyle;

class CSSStyle {
public:
	class Item {
	public:
		std::string property;
		std::string value;
		Item(const std::string& property, const std::string& value) : property(property), value(value) {}
		void dump(std::ostream& os) {
			os << property << " : " << value << ";\n";
		}
	};
	std::vector<Item*> items;
	std::string css_class;
	bool bRecursive = false;
	CSSStyle* parent = NULL;
	size_t line = 0;

	CSSStyle(const std::string css_class) : css_class(css_class) {}
	virtual ~CSSStyle() {
		while(size()>0) {
			delete items[0]; items.erase(items.begin());
		}
	}

	void add(Item* i) {
		items.push_back(i);
		if(i->property=="recursive") fromString(i->value, bRecursive);
	}

	void add(CSSStyle* s) {
		for(uint i=0; i<s->size(); s++) {
			add(s->get(i));
		}
	}

	Item* get(int i) {return items[i];}

	uint size() {return items.size();}

	CSSStyle* get_parent() {return parent;}

	void dump(std::ostream& os) {
		for(uint i=0; i<size(); i++) {
			get(i)->dump(os);
		}
	}
};

std::ostream& operator<<(std::ostream& os, CSSStyle* a);



class CSSDefinitions {
public:
	class Type {
	public:
		std::string name;
		Type* parent;
		Type(const std::string& name, Type* parent) : name(name), parent(parent) {}
	};

public:
	static std::map<std::string, CSSStyle*> defs;
	static std::vector<Type*> element_types;

public:
	CSSDefinitions() {}
	virtual ~CSSDefinitions() {}

	static bool add(const std::string& filename) {
		std::ifstream f(filename);
		if(file_has_ext(filename, ".css") && !read_css(f)) {ERROR("Can't read css file " << filename); return false;}
		if(file_has_ext(filename, ".defs") && !read_defs(f)) {ERROR("Can't read defs file " << filename); return false;}
		return true;
	}

	static bool read_defs(std::ifstream& f);
	static bool read_css(std::ifstream& f);

	static void apply_styles(IStyle* s, const std::string& classes);
	static void retrieve_styles(IStyle* s, std::vector<CSSStyle*>& styles_out);
	static void retrieve_styles(IStyle* s, const std::string& cls, std::vector<CSSStyle*>& styles_out);

	static bool is_recursive(const std::string& css_class) {
		CSSStyle* s = get(css_class);
		if(!s) return false;
		return s->bRecursive;
	}

	static void add_element_type(const std::string& type, const std::string& parent) {
		Type* p = get_type(parent);
		element_types.push_back(new Type(type, p));
	}

	static Type* get_type(const std::string& type) {
		for(uint i=0; i<element_types.size(); i++) {
			if(element_types[i]->name == type) return element_types[i];
		}
		return NULL;
	}

	static CSSStyle* get(const std::string& css_class) {
		if(defs.count(css_class)>0) return defs[css_class];
		return NULL;
	}

	static void add(const std::string& css_class, CSSStyle* s) {
		CSSStyle* ss = get(css_class);
		if(ss) ss->add(s);
		else {
			defs[css_class] = s;
			s->line = curline++;
		}
	}

	static void dump(std::ostream& os) {
		for(std::map<std::string, CSSStyle*>::iterator i = defs.begin(); i!=defs.end(); i++) {
			os << (*i).first << "{\n" << (*i).second << "}\n\n";
		}
	}

private:
	static size_t curline;
};

#endif /* CSSDEFINITIONS_H_ */
