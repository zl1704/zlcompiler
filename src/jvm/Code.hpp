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
	int cp;

	Code();

private :
	LocalVar** lvs;


};

/**
 * 跳转链
 */
class Chain{
public:
	int pc;
	Chain *next;
	Chain(int pc,Chain next);
	//合并两个跳转链
	static Chain merge(Chain c1,Chain c2);

};



#endif /* JVM_CODE_HPP_ */
