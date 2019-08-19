/*
 * Type.hpp
 *
 *  Created on: 2017年12月30日
 *      Author: ZL
 */

#ifndef CODE_TYPE_HPP_
#define CODE_TYPE_HPP_
#include "../util/includeGlobal.hpp"
#include "Symbol.hpp"
#include "../code/Constant.hpp"
using namespace util;
class Type;
class TypeSymbol;
class ClassSymbol;
class NoType;
class Type {
public:
	static  NoType* noType ;
	static string typeName(Type* t);
	static string typeName(int tag);
	static Type* elemtype(Type* t);
	static Type* elemtypeOrType(Type* t);
	int tag;
	TypeSymbol* tsym;
	Type();
	Type(int tag, TypeSymbol* tsym) :
			tag(tag), tsym(tsym) {
	}
	;
	virtual void complete() {
	}
	;
	virtual bool isConst(){return false;};
	virtual ~Type();
};

/**
 * 常量类
 * byte short int long
 * float double
 * char
 * boolean
 *
 */
typedef unsigned char   jboolean;
typedef signed char 	jbyte;
typedef short           jshort;
typedef int 			jint;
typedef long long 		jlong;
typedef float           jfloat;
typedef double          jdouble;
typedef unsigned short  jchar;
typedef union {
	jboolean _b;
	jchar c; //java char
	jbyte b;
	jshort s;
	jint i;
	jlong l;
	jfloat f;
	jdouble d;

} Object;
class ConstType:public Type{
public:
//	Constant constVal;
	//值
	Object val;
	string str;
	static ConstType* create(int tag, string value);
	virtual bool isConst(){return true;};


};

class BottomType: public Type {
public:
	BottomType() :
			Type(TypeTags::BOT, NULL) {
	}
	virtual bool isConst(){return false;};
};
class NoType: public Type {
public:
	NoType(int tag) :
			Type(tag, NULL) {
	}
	;
};
/**
 * 数组类型
 */
class ArrayType :public Type{
public:
	 Type* elemtype;
	 ArrayType(Type* elemtype,TypeSymbol* arrayclass):
		 Type(TypeTags::ARRAY,arrayclass),elemtype(elemtype){}
};


class ClassType: public Type {
public:
	ClassType(TypeSymbol* tsym) :
			Type(TypeTags::CLASS, tsym) {

	}
	virtual void complete();
};

class MethodType: public Type {
public:
	vector<Type*> argtypes;
	Type* restype;

	MethodType(vector<Type*> argtypes, Type* restype, TypeSymbol* methodClass) :
			Type(TypeTags::METHOD, methodClass), argtypes(argtypes), restype(
					restype) {
	}
	virtual void complete();

};
class ErrorType:public ClassType{
public :
	ErrorType(Type* originalType, TypeSymbol* tsym);
	ErrorType(ClassSymbol* c, Type* originalType);
	ErrorType(string name, TypeSymbol* container, Type* originalType);
private :
	Type* originType;
};




#endif /* CODE_TYPE_HPP_ */
