/*
 * Symtab.hpp
 *
 *  Created on: 2017年12月31日
 *      Author: ZL
 */

#ifndef CODE_SYMTAB_HPP_
#define CODE_SYMTAB_HPP_
#include "../util/includeGlobal.hpp"
#include "../util/global.hpp"
#include "Type.hpp"
#include "Symbol.hpp"
#include "../jvm/ByteCodes.hpp"
class Type;
class NoType;
class PackageSymbol;
class TypeSymbol;
class ClassSymbol;
class MethodSymbol;
class OperatorSymbol;
class VarSymbol;
/**
 * 预定义一些符号存放在符号表中
 */
class Symtab {
public:
	static int* symsKey;
	/**
	 * Type
	 */

 	Type* rootType;
 	Type* objectType;
 	Type* classType;
	Type* stringType;
	Type* byteType;
	Type* charType;
	Type* shortType;
	Type* intType;
	Type* longType;
	Type* floatType;
	Type* doubleType;
	Type* booleanType;
	Type* botType;
	Type* errType;
	Type* unknownType;
	NoType* voidType;
	NoType* noType;


	Type* typeOfTag[TypeTags::TypeTagCount];

	/**
	 * Symbol
	 */
	PackageSymbol* rootPackage;
	PackageSymbol* unnamedPackage;
	TypeSymbol* noSymbol;
	ClassSymbol* arrayClass;
	MethodSymbol* arrayCloneMethod;
	ClassSymbol* boundClass;
	ClassSymbol* methodClass;
	ClassSymbol* errSymbol;
	ClassSymbol* predefClass;
	//内置数组属性
	VarSymbol* lengthVar;


	OperatorSymbol* nullcheck;

	static Symtab* instance();

	void initType(Type* type, ClassSymbol* c);
	void initType(Type* type, string name);
	OperatorSymbol* enterUnop(string name, Type* arg, Type* res, int opcode);
	void enterBinop(string name, Type* left, Type* right, Type* res,
			int opcode);
	void enterBinop(string name, Type* left, Type* right, Type* res,
			int opcode1, int opcode2) ;
	Symtab();
	virtual ~Symtab();
};

#endif /* CODE_SYMTAB_HPP_ */
