/*
 * Symbol.h
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef CODE_SYMBOL_HPP_
#define CODE_SYMBOL_HPP_
#include "../util/global.hpp"
#include "../util/includeGlobal.hpp"
#include "Symtab.hpp"
#include "Type.hpp"
#include "../code/Scope.hpp"
#include "../code/Kinds.hpp"
class Completer;
class Type;
class Scope;
/**
 * 符号类
 */

class Symbol {
public:
	int kind;
	long long flags_field;
	//符号的名称
	string name;
	//类型 Type
	Type* type;
	//符号拥有者,上级符号
	Symbol* owner;

	//完成符号填充
	Completer* completer;
	//
	Type* erasure_field;

	Symbol();
	Symbol(int kind, long long flags, string name, Type* type, Symbol* owner) :
			kind(kind), flags_field(flags), name(name), type(type), owner(owner) {

	}
	;
	void complete();
	;
	virtual Symbol* clone(Symbol* newOwner) {
		return NULL;
	}
	;
	virtual string getQualifiedName() {
		return name;
	}
	virtual string flatName() {
		return getQualifiedName();
	}
	//class 或 package 有用， 其他返回NULL
	virtual Scope* members() {
		return NULL;
	}

	virtual ~Symbol();
};

class DelegatedSymbol: public Symbol {
	Symbol* other;
};

class TypeSymbol: public Symbol {
public:
	TypeSymbol() {
	}
	TypeSymbol(long long flags, string name, Type* type, Symbol* owner) :
			Symbol(Kinds::TYP, flags, name, type, owner) {
	}
	virtual void complete() {
	}
	;
	virtual Symbol* clone(Symbol* newOwner) {
		return NULL;
	}
	;
	static string formFullName(string name, Symbol* owner);
	static string formFlatName(string name, Symbol* owner);
};

class VarSymbol: public Symbol {
public :
	int adr ;
	int pos;
	VarSymbol(long long flags,string name,Type* type,Symbol* owner):Symbol(Kinds::VAR,flags,name,type,owner){
		adr = -1;
		pos = -1;
	}

};

class ClassSymbol: public TypeSymbol {
public:
	Scope* members_field;
	string fullname;
	string flatname;
	string srcFile;
	string classFile;
	ClassSymbol(long flags, string name, Type* type, Symbol* owner);
	ClassSymbol(long flags, string name, Symbol* owner);
	virtual Scope* members() {
//		if (completer != NULL)
//			complete();
		return members_field;

	}

};
class MethodSymbol: public Symbol {
public:
	vector<VarSymbol*> params;
	vector<string> savedParamNames;
	MethodSymbol(long long flags, string name, Type* type, Symbol* owner) :
			Symbol(Kinds::MTH, flags, name, type, owner) {

	}
	inline MethodSymbol* clone(Symbol* newOwner) {
		MethodSymbol* m = new MethodSymbol(flags_field, name, type, newOwner);
//		            m->code = code;
		return m;
	}
};

class OperatorSymbol: public MethodSymbol {
public:
	int opcode;
	OperatorSymbol(string name,Type* type,int opcode,Symbol* owner):
		MethodSymbol(Flags::PUBLIC | Flags::STATIC,name,type,owner),opcode(opcode){}
};
class PackageSymbol: public TypeSymbol {
public:
	Scope* members_field;
	string fullName;
	PackageSymbol(string name, Symbol* owner);
	virtual void complete();
};

class Completer {
public:
	Completer();
	virtual void complete(Symbol* sym) = 0;
	virtual ~Completer();
};

template<class P, class R>
class SymVisitor {
	R visitClassSymbol(ClassSymbol* s, P arg);
	R visitMethodSymbol(MethodSymbol* s, P arg);
	R visitPackageSymbol(PackageSymbol* s, P arg);
	R visitOperatorSymbol(OperatorSymbol* s, P arg);
	R visitVarSymbol(VarSymbol* s, P arg);
	R visitTypeSymbol(TypeSymbol* s, P arg);
	R visitSymbol(Symbol* s, P arg);

};

#endif /* CODE_SYMBOL_HPP_ */
