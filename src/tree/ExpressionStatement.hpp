/*
 * ExpressionStatement.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_EXPRESSIONSTATEMENT_HPP_
#define TREE_EXPRESSIONSTATEMENT_HPP_
#include "Statement.hpp"
/**
 * 表达式语句
 */

class ExpressionStatement :public Statement{
public:
	Expression* expr;

	ExpressionStatement();
	ExpressionStatement(int pos,Expression* expr):Statement(pos),expr(expr){};
	virtual void accept(Visitor* v) {
		v->visitExec(this);
	}
	virtual int getTag() {
		return EXEC;
	}
	virtual ~ExpressionStatement();
};

#endif /* TREE_EXPRESSIONSTATEMENT_HPP_ */
