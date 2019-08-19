/*
 * MethodDecl.hpp
 *
 *  Created on: 2017年12月7日
 *      Author: ZL
 */

#ifndef TREE_METHODDECL_HPP_
#define TREE_METHODDECL_HPP_
#include "Tree.hpp"
class MethodDecl:public Tree {
public:
	Modifiers* mods;
	string name;
	//返回类型
	Expression* restype;
	//参数
	vector<VariableDecl*> params;
	//方法体
	Block* body;

	MethodSymbol* sym;


	MethodDecl();
	MethodDecl(int pos,Modifiers* mods,Expression* restype,string name,vector<VariableDecl* > params, Block* body)
	:Tree(pos),mods(mods),name(name),params(params),restype(restype),body(body){};
	virtual void accept(Visitor* v){v->visitMethodDef(this);};
	virtual int getTag(){return METHODDEF;};
	virtual ~MethodDecl();
};

#endif /* TREE_METHODDECL_HPP_ */
