/*
 * Context.hpp
 *
 *  Created on: 2017年12月31日
 *      Author: ZL
 */

#ifndef UTIL_CONTEXT_HPP_
#define UTIL_CONTEXT_HPP_
#include "global.hpp"
class Context {
public:
	static map<void *, void*>* ht;
	Context();
    static void* get(void* key) {
        map<void*, void*>::iterator it = ht->find(key);
        if (it == ht->end())
            return NULL;
        return it->second;
    }
    template<class T>
    static void put(void* key, T value) {
        ht->insert(pair<void*, T>(key, value));
    }
	virtual ~Context();
};

#endif /* UTIL_CONTEXT_HPP_ */
