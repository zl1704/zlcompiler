/*
 * Symtab.cpp
 *
 *  Created on: 2017年12月31日
 *      Author: ZL
 */

#include "Symtab.hpp"
int* Symtab::symsKey = new int;
Symtab* Symtab::instance() {
	Symtab* syms = (Symtab*) Context::get(symsKey);
	if (syms == NULL) {
		syms = new Symtab();
		Context::put(symsKey, syms);
	}
	return syms;
}

void Symtab::initType(Type* type, ClassSymbol* c) {
	type->tsym = c;
	typeOfTag[type->tag] = type;
}

void Symtab::initType(Type* type, string name) {
	initType(type, new ClassSymbol(Flags::PUBLIC, name, type, rootPackage));
}

OperatorSymbol* Symtab::enterUnop(string name, Type* arg, Type* res,
		int opcode) {
	vector<Type*> typelist;
	typelist.push_back(arg);
	OperatorSymbol* sym = new OperatorSymbol(name, new MethodType(typelist, res,

	methodClass), opcode, predefClass);
	predefClass->members()->enter(sym);
	return sym;
}

void Symtab::enterBinop(string name, Type* left, Type* right, Type* res,
		int opcode1, int opcode2) {
	enterBinop(name, left, right, res,
			(opcode1 << ByteCodes::preShift) | opcode2);

}
void Symtab::enterBinop(string name, Type* left, Type* right, Type* res,
		int opcode) {
	vector<Type*> typelist;
	typelist.push_back(left);
	typelist.push_back(right);
	predefClass->members()->enter(
			new OperatorSymbol(name, new MethodType(typelist, res, methodClass),
					opcode, predefClass));
}

Symtab::Symtab() {
	byteType = new Type(TypeTags::BYTE, NULL);
	charType = new Type(TypeTags::CHAR, NULL);
	shortType = new Type(TypeTags::SHORT, NULL);
	intType = new Type(TypeTags::INT, NULL);
	longType = new Type(TypeTags::LONG, NULL);
	floatType = new Type(TypeTags::FLOAT, NULL);
	doubleType = new Type(TypeTags::DOUBLE, NULL);
	booleanType = new Type(TypeTags::BOOLEAN, NULL);
	botType = new BottomType();
	voidType = new NoType(TypeTags::VOID);
	stringType = new Type(TypeTags::STRING,NULL);
	objectType = new ClassType(NULL);

//	noType = new NoType(TypeTags::NONE);
	unknownType = new NoType(TypeTags::UNKNOWN);
	rootType = new Type(TypeTags::ROOT,NULL);
	rootPackage = new PackageSymbol(Name::empty, NULL);
	Scope* s = new Scope(rootPackage);
	rootPackage->members_field = s;
	unnamedPackage = new PackageSymbol(Name::empty, rootPackage);

	noSymbol = new TypeSymbol(0, Name::empty, Type::noType, rootPackage);
	noSymbol->kind = Kinds::NIL;
	errSymbol = new ClassSymbol(Flags::PUBLIC | Flags::STATIC | Flags::ACYCLIC,
			Name::any, NULL, rootPackage);
	errType = new ErrorType(Type::noType,errSymbol);
	methodClass = new ClassSymbol(Flags::PUBLIC|Flags::ACYCLIC, Name::Method, noSymbol);
	methodClass->members_field = new Scope(methodClass);




	//暂时当基本类型实现
	initType(stringType, "String");
	initType(objectType, "Object");


	initType(byteType, "byte");
	initType(shortType, "short");
	initType(charType, "char");
	initType(intType, "int");
	initType(longType, "long");
	initType(floatType, "float");
	initType(doubleType, "double");
	initType(booleanType, "boolean");
	initType(voidType, "void");
	initType(botType, "<nulltype>");
	initType(errType, errSymbol);
	// the builtin class of all arrays
	arrayClass = new ClassSymbol(Flags::PUBLIC | Flags::ACYCLIC, Name::Array,
			noSymbol);

	// 预定义操作和常量
	predefClass = new ClassSymbol(Flags::PUBLIC | Flags::ACYCLIC, Name::empty,
			rootPackage);
	Scope* scope = new Scope(predefClass);
	predefClass->members_field = scope;
	//设定数组内置属性length
	ClassType* arrayClassType = ((ClassType*)arrayClass->type);
	arrayClass->members_field = new Scope(arrayClass);
	lengthVar = new VarSymbol(
            Flags::PUBLIC | Flags::FINAL,
            Name::length,//"length"
            intType,
            arrayClass);
	arrayClass->members_field->enter(lengthVar);

	scope->enter(stringType->tsym);
	scope->enter(objectType->tsym);

	scope->enter(byteType->tsym);
	scope->enter(shortType->tsym);
	scope->enter(charType->tsym);
	scope->enter(intType->tsym);
	scope->enter(longType->tsym);
	scope->enter(floatType->tsym);
	scope->enter(doubleType->tsym);
	scope->enter(booleanType->tsym);
	scope->enter(errType->tsym);

	scope->enter(errSymbol);

	// Enter operators.
	enterUnop("+", doubleType, doubleType, ByteCodes::nop);
	enterUnop("+", floatType, floatType, ByteCodes::nop);
	enterUnop("+", longType, longType, ByteCodes::nop);
	enterUnop("+", intType, intType, ByteCodes::nop);

	enterUnop("-", doubleType, doubleType, ByteCodes::dneg);
	enterUnop("-", floatType, floatType, ByteCodes::fneg);
	enterUnop("-", longType, longType, ByteCodes::lneg);
	enterUnop("-", intType, intType, ByteCodes::ineg);

	enterUnop("~", longType, longType, ByteCodes::lxor);
	enterUnop("~", intType, intType, ByteCodes::ixor);

	enterUnop("++", doubleType, doubleType, ByteCodes::dadd);
	enterUnop("++", floatType, floatType, ByteCodes::fadd);
	enterUnop("++", longType, longType, ByteCodes::ladd);
	enterUnop("++", intType, intType, ByteCodes::iadd);
	enterUnop("++", charType, charType, ByteCodes::iadd);
	enterUnop("++", shortType, shortType, ByteCodes::iadd);
	enterUnop("++", byteType, byteType, ByteCodes::iadd);

	enterUnop("--", doubleType, doubleType, ByteCodes::dsub);
	enterUnop("--", floatType, floatType, ByteCodes::fsub);
	enterUnop("--", longType, longType, ByteCodes::lsub);
	enterUnop("--", intType, intType, ByteCodes::isub);
	enterUnop("--", charType, charType, ByteCodes::isub);
	enterUnop("--", shortType, shortType, ByteCodes::isub);
	enterUnop("--", byteType, byteType, ByteCodes::isub);

	enterUnop("!", booleanType, booleanType, ByteCodes::bool_not);
	nullcheck = enterUnop("<*nullchk*>", objectType, objectType, ByteCodes::nullchk);

	// string concatenation
	enterBinop("+", stringType, objectType, stringType, ByteCodes::string_add);
	enterBinop("+", objectType, stringType, stringType, ByteCodes::string_add);
	enterBinop("+", stringType, stringType, stringType, ByteCodes::string_add);
	enterBinop("+", stringType, intType, stringType, ByteCodes::string_add);
	enterBinop("+", stringType, longType, stringType, ByteCodes::string_add);
	enterBinop("+", stringType, floatType, stringType, ByteCodes::string_add);
	enterBinop("+", stringType, doubleType, stringType, ByteCodes::string_add);
	enterBinop("+", stringType, booleanType, stringType, ByteCodes::string_add);
	enterBinop("+", stringType, botType, stringType, ByteCodes::string_add);
	enterBinop("+", intType, stringType, stringType, ByteCodes::string_add);
	enterBinop("+", longType, stringType, stringType, ByteCodes::string_add);
	enterBinop("+", floatType, stringType, stringType, ByteCodes::string_add);
	enterBinop("+", doubleType, stringType, stringType, ByteCodes::string_add);
	enterBinop("+", booleanType, stringType, stringType, ByteCodes::string_add);
	enterBinop("+", botType, stringType, stringType, ByteCodes::string_add);

	// these errors would otherwise be matched as string concatenation
	enterBinop("+", botType, botType, botType, ByteCodes::error);
	enterBinop("+", botType, intType, botType, ByteCodes::error);
	enterBinop("+", botType, longType, botType, ByteCodes::error);
	enterBinop("+", botType, floatType, botType, ByteCodes::error);
	enterBinop("+", botType, doubleType, botType, ByteCodes::error);
	enterBinop("+", botType, booleanType, botType, ByteCodes::error);
	enterBinop("+", botType, objectType, botType, ByteCodes::error);
	enterBinop("+", intType, botType, botType, ByteCodes::error);
	enterBinop("+", longType, botType, botType, ByteCodes::error);
	enterBinop("+", floatType, botType, botType, ByteCodes::error);
	enterBinop("+", doubleType, botType, botType, ByteCodes::error);
	enterBinop("+", booleanType, botType, botType, ByteCodes::error);
	enterBinop("+", objectType, botType, botType, ByteCodes::error);

	enterBinop("+", doubleType, doubleType, doubleType, ByteCodes::dadd);
	enterBinop("+", floatType, floatType, floatType, ByteCodes::fadd);
	enterBinop("+", longType, longType, longType, ByteCodes::ladd);
	enterBinop("+", intType, intType, intType, ByteCodes::iadd);



	enterBinop("-", doubleType, doubleType, doubleType, ByteCodes::dsub);
	enterBinop("-", floatType, floatType, floatType, ByteCodes::fsub);
	enterBinop("-", longType, longType, longType, ByteCodes::lsub);
	enterBinop("-", intType, intType, intType, ByteCodes::isub);

	enterBinop("*", doubleType, doubleType, doubleType, ByteCodes::dmul);
	enterBinop("*", floatType, floatType, floatType, ByteCodes::fmul);
	enterBinop("*", longType, longType, longType, ByteCodes::lmul);
	enterBinop("*", intType, intType, intType, ByteCodes::imul);

	enterBinop("/", doubleType, doubleType, doubleType, ByteCodes::ddiv);
	enterBinop("/", floatType, floatType, floatType, ByteCodes::fdiv);
	enterBinop("/", longType, longType, longType, ByteCodes::ldiv);
	enterBinop("/", intType, intType, intType, ByteCodes::idiv);

	enterBinop("%", doubleType, doubleType, doubleType, ByteCodes::dmod);
	enterBinop("%", floatType, floatType, floatType, ByteCodes::fmod);
	enterBinop("%", longType, longType, longType, ByteCodes::lmod);
	enterBinop("%", intType, intType, intType, ByteCodes::imod);

	enterBinop("&", booleanType, booleanType, booleanType, ByteCodes::iand);
	enterBinop("&", longType, longType, longType, ByteCodes::land);
	enterBinop("&", intType, intType, intType, ByteCodes::iand);

	enterBinop("|", booleanType, booleanType, booleanType, ByteCodes::ior);
	enterBinop("|", longType, longType, longType, ByteCodes::lor);
	enterBinop("|", intType, intType, intType, ByteCodes::ior);

	enterBinop("^", booleanType, booleanType, booleanType, ByteCodes::ixor);
	enterBinop("^", longType, longType, longType, ByteCodes::lxor);
	enterBinop("^", intType, intType, intType, ByteCodes::ixor);

	enterBinop("<<", longType, longType, longType, ByteCodes::lshll);
	enterBinop("<<", intType, longType, intType, ByteCodes::ishll);
	enterBinop("<<", longType, intType, longType, ByteCodes::lshl);
	enterBinop("<<", intType, intType, intType, ByteCodes::ishl);

	enterBinop(">>", longType, longType, longType, ByteCodes::lshrl);
	enterBinop(">>", intType, longType, intType, ByteCodes::ishrl);
	enterBinop(">>", longType, intType, longType, ByteCodes::lshr);
	enterBinop(">>", intType, intType, intType, ByteCodes::ishr);

	enterBinop(">>>", longType, longType, longType, ByteCodes::lushrl);
	enterBinop(">>>", intType, longType, intType, ByteCodes::iushrl);
	enterBinop(">>>", longType, intType, longType, ByteCodes::lushr);
	enterBinop(">>>", intType, intType, intType, ByteCodes::iushr);

	enterBinop("<", doubleType, doubleType, booleanType, ByteCodes::dcmpg, ByteCodes::iflt);
	enterBinop("<", floatType, floatType, booleanType,ByteCodes::fcmpg, ByteCodes::iflt);
	enterBinop("<", longType, longType, booleanType, ByteCodes::lcmp, ByteCodes::iflt);
	enterBinop("<", intType, intType, booleanType, ByteCodes::if_icmplt);

	enterBinop(">", doubleType, doubleType, booleanType, ByteCodes::dcmpl, ByteCodes::ifgt);
	enterBinop(">", floatType, floatType, booleanType, ByteCodes::fcmpl, ByteCodes::ifgt);
	enterBinop(">", longType, longType, booleanType, ByteCodes::lcmp, ByteCodes::ifgt);
	enterBinop(">", intType, intType, booleanType, ByteCodes::if_icmpgt);

	enterBinop("<=", doubleType, doubleType, booleanType, ByteCodes::dcmpg, ByteCodes::ifle);
	enterBinop("<=", floatType, floatType, booleanType, ByteCodes::fcmpg, ByteCodes::ifle);
	enterBinop("<=", longType, longType, booleanType, ByteCodes::lcmp, ByteCodes::ifle);
	enterBinop("<=", intType, intType, booleanType, ByteCodes::if_icmple);

	enterBinop(">=", doubleType, doubleType, booleanType, ByteCodes::dcmpl, ByteCodes::ifge);
	enterBinop(">=", floatType, floatType, booleanType, ByteCodes::fcmpl, ByteCodes::ifge);
	enterBinop(">=", longType, longType, booleanType, ByteCodes::lcmp, ByteCodes::ifge);
	enterBinop(">=", intType, intType, booleanType,ByteCodes::if_icmpge);

	enterBinop("==", objectType, objectType, booleanType, ByteCodes::if_acmpeq);
	enterBinop("==", booleanType, booleanType, booleanType, ByteCodes::if_icmpeq);
	enterBinop("==", doubleType, doubleType, booleanType, ByteCodes::dcmpl, ByteCodes::ifeq);
	enterBinop("==", floatType, floatType, booleanType, ByteCodes::fcmpl, ByteCodes::ifeq);
	enterBinop("==", longType, longType, booleanType, ByteCodes::lcmp, ByteCodes::ifeq);
	enterBinop("==", intType, intType, booleanType, ByteCodes::if_icmpeq);

	enterBinop("!=", objectType, objectType, booleanType, ByteCodes::if_acmpne);
	enterBinop("!=", booleanType, booleanType, booleanType, ByteCodes::if_icmpne);
	enterBinop("!=", doubleType, doubleType, booleanType, ByteCodes::dcmpl, ByteCodes::ifne);
	enterBinop("!=", floatType, floatType, booleanType, ByteCodes::fcmpl, ByteCodes::ifne);
	enterBinop("!=", longType, longType, booleanType, ByteCodes::lcmp, ByteCodes::ifne);
	enterBinop("!=", intType, intType, booleanType, ByteCodes::if_icmpne);

	enterBinop("&&", booleanType, booleanType, booleanType, ByteCodes::bool_and);
	enterBinop("||", booleanType, booleanType, booleanType, ByteCodes::bool_or);





}

Symtab::~Symtab() {
	// TODO Auto-generated destructor stub
}

