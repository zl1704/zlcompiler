/*
 * VariableDecl.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_VARIABLEDECL_HPP_
#define TREE_VARIABLEDECL_HPP_
#include "Statement.hpp"
/**
 * 变量声明
 */
class VariableDecl:public Statement {
public:
	Modifiers* mods;
	string name;
	//变量类型
	Expression* vartype;
	//初始化值
	Expression* init;

	VarSymbol* sym;
	VariableDecl();
	VariableDecl(int pos,Modifiers* mods,string name,Expression* vartype,Expression* init)
	:Statement(pos),mods(mods),name(name.data()),vartype(vartype),init(init ),sym(0){};
	virtual void accept(Visitor* v){v->visitVarDef(this);};
	virtual int getTag(){return VARDEF;};
	virtual ~VariableDecl();
};

#endif /* TREE_VARIABLEDECL_HPP_ */
