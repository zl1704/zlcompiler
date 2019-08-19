/*
 * ClassDecl.hpp
 *
 *  Created on: 2017年12月7日
 *      Author: ZL
 */

#ifndef TREE_CLASSDECL_HPP_
#define TREE_CLASSDECL_HPP_
#include "Statement.hpp"
class ClassDecl: public Statement{
public:
	Modifiers* mods ;//修饰符
	string name;//名字
	vector<Tree* > defs;//成员属性
	Expression* extending;//暂不支持继承， 在语法分析时默认为Object
	ClassSymbol* sym;
	ClassDecl();
	ClassDecl(int pos,Modifiers* mods,string name,Expression* extending,vector<Tree*> defs):Statement(pos),
			mods(mods),name(name.data()),extending(extending){
		this->defs.assign(defs.begin(),defs.end());
	};
	virtual void accept(Visitor* v){v->visitClassDef(this);};
	virtual int getTag(){return CLASSDEF;};
	Modifiers* getModifiers(){return mods;}
	virtual ~ClassDecl();
};

#endif /* TREE_CLASSDECL_HPP_ */
