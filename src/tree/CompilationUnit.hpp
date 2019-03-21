/*
 * TCompilationUnit.hpp
 *
 *  Created on: 2017年12月7日
 *      Author: ZL
 */

#ifndef TREE_COMPILATIONUNIT_HPP_
#define TREE_COMPILATIONUNIT_HPP_
#include "Tree.hpp"

/**
 * 一个编译单元  ，也就是一个源文件
 */
class CompilationUnit:public Tree {
public:
	long flags;
	vector<Tree*> defs;//classes
	Source* source;
	//用于最后填充全局符号表
	PackageSymbol* package;
	CompilationUnit();
	CompilationUnit(int pos,vector<Tree* > defs,Source* s):Tree(pos),source(s),defs(defs),flags(0),package(NULL){
	};
	virtual void accept(Visitor* v){ v->visitTopLevel(this); }
	virtual int getTag(){return Tree::TOPLEVEL;}
	virtual ~CompilationUnit();
};

#endif /* TREE_COMPILATIONUNIT_HPP_ */
