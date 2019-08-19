/*
 * Type.cpp
 *
 *  Created on: 2017年12月30日
 *      Author: ZL
 */

#include "Type.hpp"

NoType* Type::noType = new NoType(TypeTags::NONE);

Type* Type::elemtype(Type* t){
	switch (t->tag) {
		case TypeTags::ARRAY:
			return ((ArrayType*)t)->elemtype;
		default:
			return NULL;
	}
}


Type* Type::elemtypeOrType(Type* t){
	Type* elemType = elemtype(t);
	if(elemType != NULL)
		return elemType;
	return t;

}

string Type::typeName(Type* t){
		switch(t->tag){
			case TypeTags::STRING: return "String";
			case TypeTags::BYTE: return "byte";
			case TypeTags::CHAR: return "char";
			case TypeTags::SHORT: return "short";
			case TypeTags::INT: return "int";
			case TypeTags::LONG: return "long";
			case TypeTags::DOUBLE: return "double";
			case TypeTags::FLOAT: return "float";
			case TypeTags::BOOLEAN: return "boolean";
			case TypeTags::VOID: return "void";
			case TypeTags::ARRAY: return "数组";
			case TypeTags::CLASS:
				if(t->tsym)
					return t->tsym->name;
				else
					return "error";
			default: return "error";
			}
	}
/**
 * 碰到class tag时，需要获取类符号
 * 改用上面方法  传入Type
 */
string Type::typeName(int tag){
			switch(tag){
				case TypeTags::STRING: return "String";
				case TypeTags::BYTE: return "byte";
				case TypeTags::CHAR: return "char";
				case TypeTags::SHORT: return "short";
				case TypeTags::INT: return "int";
				case TypeTags::LONG: return "long";
				case TypeTags::DOUBLE: return "double";
				case TypeTags::FLOAT: return "float";
				case TypeTags::BOOLEAN: return "boolean";
				case TypeTags::VOID: return "void";
				case TypeTags::CLASS: return "class";
				default: return "error";
				}
		}
Type::Type() {
	// TODO Auto-generated constructor stub

}

Type::~Type() {
	// TODO Auto-generated destructor stub
}

void MethodType::complete() {
	for (int i = 0; i < argtypes.size(); i++) {
		argtypes.at(i)->complete();
	}
	restype->complete();

}

void ClassType::complete() {
	if (tsym->completer != NULL)
		tsym->complete();

}

ErrorType::ErrorType(Type* originalType, TypeSymbol* tsym) :
		ClassType(tsym) {
	tag = TypeTags::ERROR;
	this->originType = (originalType == NULL ? noType : originalType);

}
ErrorType::ErrorType(ClassSymbol* c, Type* originalType) :
		ClassType(tsym) {
	c->type = this;
	c->kind = Kinds::ERR;
	c->members_field = new Scope(c);
}
ErrorType::ErrorType(string name, TypeSymbol* container, Type* originalType) :
		ClassType(tsym) {
	ClassSymbol* c = new ClassSymbol(
			Flags::PUBLIC | Flags::STATIC | Flags::ACYCLIC, name, NULL,
			container);
	c->type = this;
	c->kind = Kinds::ERR;
	c->members_field = new Scope(c);

}

/**
 * ConstType
 */
ConstType* ConstType::create(int tag,string value){
	ConstType* constType = new ConstType();
	constType->tag = tag;
	switch(tag){
	case TypeTags::BYTE:
		constType->val.b = strToNum<jbyte>(value);
		break;
	case TypeTags::CHAR:
		constType->val.c = strToNum<jchar>(value);
		break;
	case TypeTags::SHORT:
		constType->val.s = strToNum<jshort>(value);
		break;
	case TypeTags::INT:
		constType->val.i = strToNum<jint>(value);
		break;
	case TypeTags::LONG:
		constType->val.l = strToNum<jlong>(value);
		break;
	case TypeTags::FLOAT:
		constType->val.f = strToNum<jfloat>(value);
		break;
	case TypeTags::DOUBLE:
		constType->val.d = strToNum<jdouble>(value);
		break;
	case TypeTags::BOOLEAN:
		constType->val._b = strToNum<jboolean>(value);
		break;
	default:
		break;
	}
	constType->str = value;

	return constType ;
}
