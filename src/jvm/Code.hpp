/*
 * Code.hpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */

#ifndef JVM_CODE_HPP_
#define JVM_CODE_HPP_
#include "../util/global.hpp"
#include "../util/includeGlobal.hpp"
#include "Pool.hpp"
class Code;

/**
 * 局部变量
 */
class LocalVar{
	LocalVar(VarSymbol v);
	LocalVar dup();

private :
	VarSymbol sym;
	char reg;
	Code* code;

};


/**
 * 方法代码
 */
class Code{
public:
	jbyte* code;
	int size ;
	int cp;
	LocalVar** lvs;
	int lvsize;
	MethodSymbol* msym;
	Symtab* syms;
	Pool* pool;
	Code(MethodSymbol* sym,Pool* pool){
		syms = sym;
		this->pool = pool;
		code = new jbyte[64];
		size = 64;
		lvs = new LocalVar[20];
		lvsize = 20;
	}
	void checkCode();
	//byte short  char int ---> int
	static int truncate(int tc);
	//对象长度
	int witdh(int tc);



};

/**
 * 跳转链
 */
class Chain{
public:
	int pc;
	Chain *next;
	Chain(int pc,Chain* next);
	//合并两个跳转链
	static Chain* merge(Chain* c1,Chain* c2);

};



#endif /* JVM_CODE_HPP_ */
