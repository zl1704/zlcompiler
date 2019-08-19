/*
 * Constant.hpp
 *
 *  Created on: 2019年3月20日
 *      Author: zz
 */

#ifndef CODE_CONSTANT_HPP_
#define CODE_CONSTANT_HPP_
/**
 * 常量类
 * byte short int long
 * float double
 * char
 * boolean
 *
 */
#include "../util/global.hpp"

class Constant{
public:
	//值
	union{
			unsigned short c; //java char
			short s;
			int i;
			long long l;
			float f;
			double d;
//			char* str;

	} val;
	string str;
	//类型  用TypeTags中的tag
	int tag;




};

class Byte:Constant{


};
class Short:Constant{


};

class Integer:Constant{


};

class Long:Constant{


};
class Float:Constant{


};

class Double:Constant{


};

class Char:Constant{


};

class ConstString:Constant{


};

#endif /* CODE_CONSTANT_HPP_ */
