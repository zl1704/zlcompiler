/*
 * Unary.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_UNARY_HPP_
#define TREE_UNARY_HPP_
#include "Expression.hpp"
/**
 * 一元操作符
 */
class Unary:public Expression {
public:
	int opcode;
	Expression* arg;
	Symbol* sym;
	Unary();
	Unary(int pos,int opcode,
	Expression* arg):Expression(pos),opcode(opcode),arg(arg),sym(0){};
	virtual void accept(Visitor* v) {
		v->visitUnary(this);
	}
	virtual int getTag() {
		return opcode;
	}
	virtual ~Unary();
};

#endif /* TREE_UNARY_HPP_ */
