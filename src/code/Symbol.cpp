/*
 * Symbol.cpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#include "Symbol.hpp"

Symbol::Symbol() {
	// TODO Auto-generated constructor stub

}

Symbol::~Symbol() {
	// TODO Auto-generated destructor stub
}

void Symbol::complete() {
	if (completer != NULL) {
		Completer* c = completer;
		completer = NULL;
		c->complete(this);
	}

}

//TypeSymbol

string TypeSymbol::formFullName(string name, Symbol* owner) {
	if (owner == NULL)
		return name;
	if (((owner->kind != Kinds::ERR))
			&& ((owner->kind & (Kinds::VAR | Kinds::MTH)) != 0
					|| (owner->kind == Kinds::TYP
							&& owner->type->tag == TypeTags::TYPEVAR)))
		return name;
	string prefix = owner->getQualifiedName();
	if (prefix == Name::empty)
		return name;
	else
		return prefix + '.' + name;

}
string TypeSymbol::formFlatName(string name, Symbol* owner) {
	if (owner == NULL || (owner->kind & (Kinds::VAR | Kinds::MTH)) != 0
			|| (owner->kind == Kinds::TYP
					&& owner->type->tag == TypeTags::TYPEVAR))
		return name;
	char sep = owner->kind == Kinds::TYP ? '$' : '.';
	string prefix = owner->flatName();
	if (prefix == Name::empty)
		return name;
	else
		return prefix + '.' + name;

}

/**
 * package sym
 */
PackageSymbol::PackageSymbol(string name, Symbol* owner) {
	this->kind = Kinds::TYP;
	this->name = name;
	this->owner = owner;
	this->type = new Type(TypeTags::PACKAGE, this);
	this->fullName = formFullName(name, owner);
}

void PackageSymbol::complete() {
	if (completer != NULL) {
		Completer* c = completer;
		completer = NULL;
		c->complete(this);
	}
}

/**
 * ClassSymbol
 */

ClassSymbol::ClassSymbol(long flags, string name, Type* type, Symbol* owner) :
		TypeSymbol(flags, name, type, owner) {
	this->type = new ClassType(NULL);
	this->fullname = formFlatName(name, owner);
	this->flatname = formFlatName(name, owner);
	this->srcFile = "";
	this->classFile = "";

}
ClassSymbol::ClassSymbol(long flags, string name, Symbol* owner) :
		TypeSymbol(flags, name, NULL, owner) {
	type = new ClassType(NULL);
	type->tsym = this;
	this->fullname = formFlatName(name, owner);
	this->flatname = formFlatName(name, owner);
	this->srcFile = "";
	this->classFile = "";
}

