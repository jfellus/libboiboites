/*
 * CSSDefinitions.cpp
 *
 *  Created on: 31 oct. 2014
 *      Author: jfellus
 */

#include "CSSDefinitions.h"
#include "IStyle.h"
#include <list>

std::map<std::string, CSSStyle*> CSSDefinitions::defs;
std::vector<CSSDefinitions::Type*> CSSDefinitions::element_types;
size_t CSSDefinitions::curline = 0;

std::ostream& operator<<(std::ostream& os, CSSStyle* a) {
	if(!a) os << "null";
	else a->dump(os);
	return os;
}

static bool _style_order_comp(CSSStyle* c1, CSSStyle* c2) {
	return c1->line < c2->line;
}

void CSSDefinitions::apply_styles(IStyle* s, const std::string& classes) {
	std::vector<CSSStyle*> csss;
	retrieve_styles(s, csss);

	// Class styling
	FOR_EACH_TOKEN(classes, ' ', cls) {
		if(cls.empty()) continue;
		retrieve_styles(s, cls, csss);
	}

	std::sort (csss.begin(), csss.end(), _style_order_comp);
	for(uint i=0; i<csss.size(); i++) s->apply_style(csss[i]);
}

void CSSDefinitions::retrieve_styles(IStyle* s, std::vector<CSSStyle*>& styles_out) {
	if(!s) return;
	std::list<Type*> inherits;
	Type* type = get_type(s->name());
	if(!type) ERROR("Unknown type " << s->name());
	while(type!=NULL) {
		inherits.push_back(type);
		type = type->parent;
	}
	while(!inherits.empty()) {
		CSSStyle* css = get(inherits.front()->name); inherits.pop_front();
		if(css) styles_out.push_back(css);
	}
}

void CSSDefinitions::retrieve_styles(IStyle* s, const std::string& cls, std::vector<CSSStyle*>& styles_out) {
	if(!s) return;
	std::list<Type*> inherits;
	Type* type = get_type(s->name());
	if(!type) ERROR("Unknown type " << s->name());
	while(type!=NULL) {
		inherits.push_back(type);
		type = type->parent;
	}
	while(!inherits.empty()) {
		Type* type = inherits.front(); inherits.pop_front();
		std::string ss = type->name[0]=='*' ? TOSTRING("." << cls) : TOSTRING(type->name << "." << cls);
		CSSStyle* css = get(ss);
		if(css) styles_out.push_back(css);
	}
}

bool CSSDefinitions::read_defs(std::ifstream& f) {
	std::string name;
	std::string parent;
	while(f.good()) {
		f >> name >> parent;
		add_element_type(name, parent);
	}
	return true;
}

bool CSSDefinitions::read_css(std::ifstream& f) {
	if(!f.good()) return false;
	char css_class[256];
	char prop[512];
	char val[1024];
	int c;
	int i=0;
	while(f.good()) {
		while(((c=f.get())!=-1) && (c=='\n' || c=='\t' || c==' ')) {}
		if(!f.good()) break;
		if(c=='/') {f_skipline(f); continue;}
		css_class[i=0] = c;	while(((c=f.get())!=-1) && (isalnum(c) || c=='_' || c=='.' || c==':')) css_class[++i] = c;
		css_class[++i] = 0;
		CSSStyle* s = new CSSStyle(css_class);
		if(c!='{') while(((c=f.get())!=-1) && c!='{') {}
		while(((c=f.get())!=-1) && (c=='\n' || c=='\t' || c==' ')) {}
		if(c!='}') {
			for(;;) {
				prop[i=0] = c;	while(((c=f.get())!=-1) && (c!='\n' && c!='\t' && c!=' ' && c!=':')) prop[++i] = c;
				while(((c=f.get())!=-1) && (c=='\n' || c=='\t' || c==' ' || c==':')) {}
				prop[++i] = 0;
				val[i=0] = c; while(((c=f.get())!=-1) && (c!=';')) val[++i] = c;
				val[++i] = 0;
				s->add(new CSSStyle::Item(str_trim(prop), str_trim(val)));
				while(((c=f.get())!=-1) && (c=='\n' || c=='\t' || c==' ')) {}
				if(c=='}' || !f.good()) break;
			}
		}
		add(css_class, s);
	}
	return true;
}
