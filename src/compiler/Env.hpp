/*
 * Env.hpp
 *
 *  Created on: 2018年1月1日
 *      Author: ZL
 */

#ifndef COMPILER_ENV_HPP_
#define COMPILER_ENV_HPP_
#include "../tree/Tree.hpp"
#include "../util/global.hpp"
#include "../util/includeGlobal.hpp"
#include "../code/Symbol.hpp"
class Tree;
class CompilationUnit;
class ClassDecl;
class MethodDecl;
class Scope;
class Type;
class Symbol;
/**
 * 遍历AST所处环境
 */
template<class T>
class Env {
public:
	Env<T>* next;
	Env<T>* outer;
	//当前关联
	Tree* tree;

	CompilationUnit* toplevel;
	ClassDecl* enclClass;
	MethodDecl* enclMethod;
	T info;

	Env();
	Env(Tree* tree, T info) :
			tree(tree), info(info) {
		toplevel = NULL;
		enclClass = NULL;
		enclMethod = NULL;
		next = NULL;
		outer = NULL;
	}
	Env<T>* dup(Tree* tree, T info) {
		return dupto(new Env<T>(tree, info));
	}
	Env<T>* dup(Tree* tree) {
			return dup(tree,this->info);
		}
	Env<T>* dupto(Env<T>* that) {
		that->next = this;
		that->outer = this->outer;
		that->toplevel = this->toplevel;
		that->enclClass = this->enclClass;
		that->enclMethod = this->enclMethod;
		return that;
	}
	;
};

class AttrContext {
public:
	Scope* scope;
	int staticLevel;
	bool isSelfCall;
	bool varArgs;
	vector<Type*> tvars;
	Symbol* enclVar;
	AttrContext() {
	}
	AttrContext* dup(Scope* scope);
	AttrContext* dup();
};

//class AttrContextEnv :public Env<AttrContext*>{
//public:
//	AttrContextEnv(Tree* tree,AttrContext* info):Env(tree,info){}
//
//};

#endif /* COMPILER_ENV_HPP_ */
