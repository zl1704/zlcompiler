/*
 * Parens.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_PARENS_HPP_
#define TREE_PARENS_HPP_
#include "Expression.hpp"
/**
 * 括号表达式
 * ( ... )
 */
class Parens:public Expression {
public:
	Expression* expr;
	Parens();
	Parens(int pos,Expression* expr):Expression(pos),expr(expr){	};
	virtual void accept(Visitor* v) {
		v->visitParens(this);
	}
	virtual int getTag() {
		return PARENS;
	}
	virtual ~Parens();
};

#endif /* TREE_PARENS_HPP_ */
