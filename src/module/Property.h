/*
 * Property.h
 *
 *  Created on: 8 oct. 2014
 *      Author: jfellus
 */

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include <util/utils.h>



namespace libboiboites {

class Property {
public:
	std::string format = "";
	bool editable = true;
	bool undefined = true;
	std::string name;
public:
	Property(std::string name) { this->name = name; }
	virtual ~Property() {}

	virtual std::string get_value_as_string() { return undefined ? "???" : "";}
	virtual void set_value_from_string(const std::string& s)  = 0;

	void set_undefined(bool undefined = true) {this->undefined = undefined;}

	operator std::string() {return get_value_as_string();}
	void operator=(const std::string& s) {set_value_from_string(s);}

	virtual Property* copy() = 0;
	virtual void dump(std::ostream& os) {
		os << name << "=" << get_value_as_string();
	}
	friend std::ostream& operator<<(std::ostream& os, Property* a);
};

std::ostream& operator<<(std::ostream& os, Property* a);



template <class T> class TProperty : public Property {
public:
	T val;
public:
	TProperty(std::string name, T val) : Property(name), val(val) { set_undefined(false); }
	virtual std::string get_value_as_string() { return undefined ? "???" : toString(val);}
	virtual void set_value_from_string(const std::string& s) {
		fromString(s, val);  set_undefined(false);
	}

	void set(const T& val) { this->val = val; }

	virtual Property* copy() { return new TProperty<T>(name, val); }
};

template <class T> class TPropertyRef : public Property {
public:
	T* val;
public:
	TPropertyRef(std::string name, T* val) : Property(name), val(val) { set_undefined(false); }
	virtual std::string get_value_as_string() { return undefined ? "???" : toString(*val);}
	virtual void set_value_from_string(const std::string& s) {
		fromString(s, *val);  set_undefined(false);
	}
	void set(T* v) {fromString(toString(*v),*val);}

	virtual Property* copy() { return new TProperty<T>(name, *val); }
};



class Properties  {
public:
	std::vector<Property*> v;
public:
	Properties() {}
	virtual ~Properties() {}

	void clear() { v.clear(); }

	void add(Property* p, const std::string& format = "") {
		p->format = format;
		if(get(p->name)) {DBG("Property key already exists : " << p->name);return;}
		v.push_back(p);
	}

	template <typename T> void add(std::string name, T f, const std::string& format = "") {add(new TProperty<T>(name, f), format);}
	void add(std::string name, const char* s, const std::string& format = "") {add(new TProperty<std::string>(name, std::string(s)), format);}

	template <typename T> void add(std::string name, T* f, const std::string& format = "") {add(new TPropertyRef<T>(name, f), format);}

	void set_from_string(const std::string& name, const std::string& val) {
		Property* p = get(name);
		if(!p) {add(new TProperty<std::string>(name, *(new std::string(val)))); return;}
		p->set_value_from_string(val);
	}

	template <typename T> void set(std::string name, T* f) {
		Property* p = get(name);
		if(!p) {add(new TPropertyRef<T>(name, f)); return;}
		TPropertyRef<T>* pf = dynamic_cast<TPropertyRef<T>*>(p);
		if(!pf) throw "Wrong argument";
		pf->set(f);
	}

	void set(std::string name, const char* s) {
		Property* p = get(name);
		if(!p) {add(new TProperty<std::string>(name, std::string(s))); return;}
		TProperty<std::string>* pf = dynamic_cast<TProperty<std::string>*>(p);
		if(!pf) {
			TPropertyRef<std::string>* pff = dynamic_cast<TPropertyRef<std::string>*>(p);
			if(!pff) throw "Wrong argument";
			std::string ss = std::string(s);
			pff->set(&ss);
		} else pf->set(s);
	}

	template <typename T> void set(std::string name, T f) {
		Property* p = get(name);
		if(!p) {add(new TProperty<T>(name, f)); return;}
		TProperty<T>* pf = dynamic_cast<TProperty<T>*>(p);
		if(!pf) {
			TPropertyRef<T>* pff = dynamic_cast<TPropertyRef<T>*>(p);
			if(!pff) throw "Wrong argument";
			pff->set(&f);
		} else pf->set(f);
	}

	Property* get(const std::string& name) {
		for(uint i=0; i<v.size(); i++) {
			Property* p = v[i];
			if(p->name == name) return p;
		}
		return NULL;
	}

	std::string get_as_string(const std::string& name) {
		Property* p = get(name);
		if(!p) return "";
		else return p->get_value_as_string();
	}

	Property* operator[](const std::string& name) {return get(name);}

	bool remove(const std::string& name) {
		for(uint i=0; i<v.size(); i++) {
			Property* p = v[i];
			if(p->name == name) {vector_remove(v, p);return true;}
		}
		return false;
	}

	uint size() {return v.size();}

	Property* operator[](int i) {return v[i];}

	virtual void dump(std::ostream& os) {
		os << "[";
		for(uint i=0; i<v.size(); i++) {
			if(i!=0) os << ",";
			os << v[i];
			if(!v[i]->format.empty()) os << "(" << v[i]->format << ")";
		}
		os << "]";
	}

	virtual std::string to_JSON() {
		std::ostringstream os;
		os << "{ ";
		for(uint i=0; i<v.size(); i++) {
			if(i!=0) os << ", ";
			os << "\"" << v[i]->name << "\": \"" << JSON_escape(v[i]->get_value_as_string()) << "\"";
			if(!v[i]->format.empty()) os << ", \"__format__" << v[i]->name << "\" : \"" << v[i]->format << "\"";
		}
		os << " }";
		return os.str();
	}
	friend std::ostream& operator<<(std::ostream& os, Properties* a);
};

std::ostream& operator<<(std::ostream& os, Properties* a);




class IPropertiesListener;
class IPropertiesElement {
public:
	Properties properties;
	std::vector<IPropertiesListener*> propertiesListeners;
public:
	IPropertiesElement() {}
	virtual ~IPropertiesElement() {}

	void add_properties_listener(IPropertiesListener* l) { propertiesListeners.push_back(l);}
	void set_property(const std::string& name, const std::string& value);
	std::string get_property(const std::string& name) {return properties.get(name) ? properties.get(name)->get_value_as_string() : "";}
};



/////////////////////////
// PROPERTIES LISTENER //
/////////////////////////

class IPropertiesListener {
public:
	IPropertiesListener() {}
	virtual ~IPropertiesListener() {}
	virtual void on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val) = 0;
};


}

#endif /* PROPERTY_H_ */
