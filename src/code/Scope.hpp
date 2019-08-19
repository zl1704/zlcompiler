/*
 * Scope.hpp
 *
 *  Created on: 2017年12月30日
 *      Author: ZL
 */

#ifndef CODE_SCOPE_HPP_
#define CODE_SCOPE_HPP_
#include "../util/global.hpp"
#include "../util/includeGlobal.hpp"
#include "Symbol.hpp"
#include <cmath>
class Symbol;
class Scope;
class Entry{
public :
	Symbol* sym;
	//next entry
	Entry* next;
	int hash;
	Scope* scope;
	Entry(){}
	Entry(int hash,Symbol* key, Scope* value ,Entry* next):hash(hash),sym(key),scope(value),next(next){

	}



	Scope* getOrigin(){return scope;}



};
class Scope {
public:
	//Next enclosing scope,  parent scope
	Scope* next ;
	//不用
//	Entry* elems;
	//scope的拥有者
	Symbol* owner ;

	static const Scope* emptyScope;

	Scope(Symbol* owner);
	Scope(Scope* next,Symbol* owner,Entry** table,int size);
	Scope(Scope* next,Symbol* owner,Entry** table,int nelems,int size);
	//复制
	Scope* dup(bool shared =false);
	Scope* dup(Symbol* newOwner,bool shared);
	//此方法修改为dup,深层复制。 如果考虑内存空间，使用此接口
	Scope* dupShared();
	Scope* leave();

	//填充符号
	void enter(Symbol* sym);
	void enter(Symbol* sym,Scope* scope);
	void enter(Symbol* sym, Scope* s, Scope * origin);
	void makeEntry(int hash,Symbol* sym, Scope* scope,int index);
	void remove(Symbol* sym);
	void enterIfAbsent(Symbol* sym);
	bool includes(Symbol* c);
	Entry* findLocal(string name);
	Entry* find(string name);
	Entry* getEntry(string name);

	void print();

	virtual ~Scope();

private :
	int shared;
	int size;
	Entry** table;
	int nelems;
	static  Entry* sentinel;
	static const int INITIAL_SIZE = 0x10;
	int hash(string s);
	void dble();
	void transfer(Entry** oldtable,int size);
	int index(int h);
};




#endif /* CODE_SCOPE_HPP_ */
