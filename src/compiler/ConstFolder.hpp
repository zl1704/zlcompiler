/*
 * ConstFolder.hpp
 *
 *  Created on: 2019年3月21日
 *      Author: zz
 */



#ifndef COMPILER_CONSTFOLDER_HPP_
#define COMPILER_CONSTFOLDER_HPP_
/**
 * 常量折叠
 */

class ConstFolder{
public :
	//折叠一元操作
	Type* fold1(int opcode,Type* t);
	Type* fold2(int opcode,Type* left,Type* right);


};


#endif /* COMPILER_CONSTFOLDER_HPP_ */
