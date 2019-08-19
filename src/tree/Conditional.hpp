/*
 * Conditional.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_CONDITIONAL_HPP_
#define TREE_CONDITIONAL_HPP_
#include "Expression.hpp"
/**
 * 三目运算
 *  ( ) ? ( ) : ( ) conditional expression
 */
class Conditional:public Expression {
public:
	Expression* cond;
	Expression* truepart;
	Expression* falsepart;
	Conditional();
	Conditional(int pos,Expression* cond,Expression* truepart,Expression* falsepart):
		Expression(pos),cond(cond),truepart(truepart),falsepart(falsepart){};
	virtual void accept(Visitor* v) {
		v->visitConditional(this);
	}
	virtual int getTag() {
		return CONDEXPR;
	}
	virtual ~Conditional();
};

#endif /* TREE_CONDITIONAL_HPP_ */
