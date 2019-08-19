/*
 * NewClass.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_NEWCLASS_HPP_
#define TREE_NEWCLASS_HPP_
#include "Expression.hpp"
class NewClass :public Expression{
public:
	Expression* encl;
	vector<Expression* > args;
	ClassDecl* def;
	Expression* clazz;
	Symbol* constructor;
	Type* varargsElement;
	Type* constructorType;
	NewClass();
	NewClass(int pos,Expression* encl,
	vector<Expression* > args,
	ClassDecl* def,
	Expression* clazz):Expression(pos),encl(encl),def(def),clazz(clazz){
		this->args.assign(args.begin(),args.end());
		constructor = NULL;
		varargsElement = NULL;
		constructorType = NULL;
	};
	virtual void accept(Visitor* v) {
		v->visitNewClass(this);
	}
	virtual int getTag() {
		return NEWCLASS;
	}
	virtual ~NewClass();
};

#endif /* TREE_NEWCLASS_HPP_ */
