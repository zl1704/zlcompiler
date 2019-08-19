/*
 * MethodInvocation.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_METHODINVOCATION_HPP_
#define TREE_METHODINVOCATION_HPP_
#include "Expression.hpp"
/**
 * 方法调用
 */
class MethodInvocation:public Expression {

public:
	//参数
	vector<Expression* > args;
	Expression* meth;
	MethodInvocation();
	MethodInvocation(int pos,Expression* meth,vector<Expression* > args):Expression(pos),meth(meth){
		this->args.assign(args.begin(),args.end());
	};
	virtual void accept(Visitor* v) {
		v->visitApply(this);
	}
	virtual int getTag() {
		return APPLY;
	}
	virtual ~MethodInvocation();
};

#endif /* TREE_METHODINVOCATION_HPP_ */
