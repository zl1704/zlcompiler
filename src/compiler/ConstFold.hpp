/*
 * ConstFolder.hpp
 *
 *  Created on: 2019年3月21日
 *      Author: zz
 */



#ifndef COMPILER_CONSTFOLD_HPP_
#define COMPILER_CONSTFOLD_HPP_
#include "../util/global.hpp"
#include "../util/includeGlobal.hpp"
#include "../code/Type.hpp"
using namespace util;
/**
 * 常量折叠
 */

class ConstFold{
public :
	static ConstFold* instance();
	ConstFold();
	//折叠一元操作
	Type* fold1(int opcode,ConstType* t);
	//折叠二元操作
	Type* fold2(int opcode,ConstType* left,ConstType* right);
	//Type转换
	Type* coerce(Type* et,Type* tt);
private :
	static int* ctkey;

};


#endif /* COMPILER_CONSTFOLD_HPP_ */
