/*
 * Check.cpp
 *
 *  Created on: 2017年12月31日
 *      Author: ZL
 */

#include "Check.hpp"
int* Check::checkKey = new int;
Check* Check::instance() {
	Check* check = (Check*) Context::get(checkKey);

	if (check == NULL) {
		check = new Check();
	}

	return check;
}
Check::Check() {
	Context::put(checkKey, this);
	log = Log::instance();
	syms = Symtab::instance();
}

Check::~Check() {
}

/**
 * 检查found类型是否可赋值给req类型
 *               char
 *   虚拟机规范 5.1
 *   byte  short int  float double
 *   byte to  short ,  int ,  long ,  float , or  double
 *   short to  int ,  long ,  float , or  double
 *   char to  int ,  long ,  float , or  double
 *   int to  long ,  float , or  double
 *   long to  float or  double
 *
 */

bool Check::isAssignable(Type* found, Type* req){

	if(found->tag == req->tag)
		return true;
	switch(req->tag){
	case TypeTags::BYTE:
		return found->tag <=TypeTags::BYTE ;  //s也可能是常量 ，需判断范围！！！！ 待修改
	case TypeTags::CHAR:
		return found->tag ==TypeTags::CHAR ;
	case TypeTags::SHORT:
		return found->tag <=TypeTags::SHORT && found->tag !=TypeTags::CHAR;
	case TypeTags::INT:
		return found->tag <=TypeTags::INT;
	case TypeTags::LONG:
		return found->tag <=TypeTags::LONG;
	case TypeTags::FLOAT:
		return found->tag <=TypeTags::FLOAT;
	case TypeTags::DOUBLE:
		return found->tag <=TypeTags::DOUBLE;
	default :
		break;
	}
	//其他情况  子类向父类  待添加...
	return  false;
}


/**
 * !!!
 * 检查类型是否兼容
 * 一致或隐式转型 子类转父类
 */
Type* Check::checkType(int pos, Type* t, Type* req, string errKey) {
	if (req->tag == TypeTags::ERROR)
		return t;
	if (req->tag == TypeTags::NONE)
		return t;
	//如果都是数组检查维度是否一致， 如果其中一个非数组类型，检查类型是否一致
	if (t->tag == TypeTags::ARRAY || req->tag == TypeTags::ARRAY) {
		Type* t1 = t;
		Type* t2 = req;
		//其中一个非数组
		if (t->tag != TypeTags::ARRAY || req->tag != TypeTags::ARRAY) {
			log->report(pos, Log::ERROR, errKey, Type::typeName(t2),
					Type::typeName(t1));
			return new ErrorType(t, syms->errSymbol);
		}
		//两个都是数组
		while (t1->tag == TypeTags::ARRAY && t2->tag == TypeTags::ARRAY) {
			t1 = Type::elemtype(t1);
			t2 = Type::elemtype(t2);
		}
		//维度不一样
		if ((t1->tag == TypeTags::ARRAY || t2->tag == TypeTags::ARRAY)) {
			log->report(pos, Log::ERROR, "数组维度不一致");
			return new ErrorType(t, syms->errSymbol);
		}
		//维度一样 检查类型是否一样
		else if (!isAssignable(t1, t2)) {
			log->report(pos, Log::ERROR, errKey, Type::typeName(t2),
					Type::typeName(t1));
			return new ErrorType(t, syms->errSymbol);
		}

	}
	//非数组情况
	else if (!isAssignable(t, req)) {
		log->report(pos, Log::ERROR, errKey, Type::typeName(req),
				Type::typeName(t));
	}

	return t;
}

bool Check::checkUniqueClassName(int pos, string name, Scope* s) {
	for (Entry* e = s->findLocal(name); e; e = e->next) {
		if (e->sym->kind == Kinds::TYP && e->sym->name == name) {
			duplicateError(pos, e->sym);
			return false;
		}
	}
	return true;
}
bool Check::checkUnique(int pos, Symbol* sym, Scope* s) {
	//内置过滤掉
	if (sym->owner->name == Name::any)
		return false;
//	s->print();
//	cout<<endl;

	while(s){
		Entry* e = s->find(sym->name);
		if(!e)
			return true ;
		s = e->scope;
		for (; e; e = e->next) {
			//方法重复定义,参数列表必须不一致
			if ( e->sym->name == sym->name && sym->kind == Kinds::MTH && hasSameArgs(sym->type, e->sym->type)) {
				duplicateError(pos, e->sym);
				return true;
			} else if (sym->kind != Kinds::MTH && e->sym->name == sym->name) {
				//变量定义 检查e是否是类成员变量  kind == TYP
				if(e->sym->owner->kind == Kinds::TYP)
					return true;
				duplicateError(pos, e->sym);
				return false;
			}
		}
		s = s->next;

	}
	return true;
}

bool Check::hasSameArgs(Type* t, Type* s) {
	if (t->tag == TypeTags::METHOD && s->tag == TypeTags::METHOD) {
		MethodType* t1 = (MethodType*) t;
		MethodType* t2 = (MethodType*) s;

		if (t1->argtypes.size() > 0
				&& t1->argtypes.size() == t2->argtypes.size()) {
			int i = 0;
			for (; i < t1->argtypes.size() && i < t2->argtypes.size(); i++) {
				Type* s1 = t1->argtypes.at(i);
				Type* s2 = t2->argtypes.at(i);
				if (!sameType(s1, s2)) {
					break;
				}

			}
			return t2->argtypes.size() == i;
		}

		return (t1->argtypes.size() == t2->argtypes.size())
				&& (t2->argtypes.size() == 0);

	}
	return false;

}




bool Check::sameType(Type* t, Type* s) {
	if (t->tag == TypeTags::TYPEVAR || s->tag == TypeTags::TYPEVAR) {
		return (TypeSymbol*) t->tsym == t->tsym;
	} else {
		return t->tag == s->tag;
	}
}
/**
 * 检查if   是否是空的
 */
void Check::checkEmptyIf(If* tree) {
	if (tree->thenpart->getTag() == Tree::SKIP && tree->elsepart == NULL)
		log->report(tree->pos, Log::ERROR, "空if");
}

Type* Check::checkNonVoid(int pos, Type* t) {
	if(!t){
		log->report(pos, Log::ERROR, "内部错误 ：checkNonVoid ,type = null");
		return t;
	}
	if (t->tag == TypeTags::VOID)
		log->report(pos, Log::ERROR, "这里不允许void");
	return t;
}
void Check::duplicateError(int pos, Symbol* sym) {
	//方法重复定义
//	if (sym->kind == Kinds::MTH) {
//
//	} else {
//		//变量
//		log->report(pos, Log::ERROR, sym->name + "重复定义");
//	}
	log->report(pos, Log::ERROR, kindName(sym->kind) + sym->name + "重复定义");
}

string Check::kindName(int kind) {
	switch (kind) {
	case Kinds::MTH:
		return "方法 ";
	case Kinds::TYP:
		return "";
	default:
		return "变量 ";
	}

}


Tree* Check::skipParens(Tree* tree) {
	if (tree->getTag() == Tree::PARENS)
		return skipParens((Parens*) tree);
	else
		return tree;
}

Expression* Check::skipParens(Expression* tree) {
	while (tree->getTag() == Tree::PARENS) {
		tree = ((Parens*) tree)->expr;
	}
	return tree;
}

Symbol* Check::symbol(Tree* tree) {
	tree = skipParens(tree);
	switch (tree->getTag()) {
	case Tree::IDENT:
		return ((Ident*) tree)->sym;
	case Tree::SELECT:
		return ((FieldAccess*) tree)->sym;
	default:
		return NULL;
	}

}


Symbol* Check::symbolFor(Tree* tree){
	tree = skipParens(tree);
		switch (tree->getTag()) {
		case Tree::CLASSDEF:
			return ((ClassDecl*) tree)->sym;
		case Tree::METHODDEF:
			return ((MethodDecl*) tree)->sym;
		case Tree::VARDEF:
			return ((VariableDecl*) tree)->sym;
		default:
			return NULL;
		}



}
string Check::treeName(Tree* tree){
	 switch (tree->getTag()) {
	        case Tree::IDENT:
	            return ((Ident*) tree)->name;
	        case Tree::SELECT:
	            return ((FieldAccess*) tree)->name;
	        default:
	            return "";
	}
}
