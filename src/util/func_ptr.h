/*
 * func_ptr.h
 *
 *  Created on: 1 oct. 2014
 *      Author: jfellus
 */

#ifndef FUNC_PTR_H_
#define FUNC_PTR_H_

/** Functor for 0-arguments member functions */
template <class T>
class callback_f {
	T* obj;
	void (T::*func)();
public:
	void operator()() const { (obj->*func)();}
	callback_f(T& obj_, void (T::*func_)()): obj(&obj_), func(func_) {}
};

/** Creates a callback function from an object member method */
template <class T> callback_f<T> CALLBACK(T& t, void (T::*func)()) {
	return callback_f<T>(t, func);
}


#endif /* FUNC_PTR_H_ */
