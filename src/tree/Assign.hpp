/*
 * Assign.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_ASSIGN_HPP_
#define TREE_ASSIGN_HPP_
#include "Expression.hpp"
/**
 * '='  赋值
 */
class Assign:public Expression {
public:
	Expression* lhs;
	Expression* rhs;

	Assign();
	Assign(int pos,	Expression* lhs,
	Expression* rhs):Expression(pos),lhs(lhs),rhs(rhs){};
	virtual void accept(Visitor* v) {
		v->visitAssign(this);
	}
	virtual int getTag() {
		return ASSIGN;
	}
	virtual ~Assign();
};

#endif /* TREE_ASSIGN_HPP_ */
