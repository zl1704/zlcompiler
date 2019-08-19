/*
 * Binary.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_BINARY_HPP_
#define TREE_BINARY_HPP_
#include "Expression.hpp"
/**
 * 二元操作符
 */
class Binary:public Expression {
public:
	//操作符
	int opcode;
	Expression* lhs;
	Expression* rhs;
	Symbol* opsym;
	Binary();
	Binary(int pos,int opcode,
	Expression* lhs,
	Expression* rhs):Expression(pos),opcode(opcode),lhs(lhs),rhs(rhs),opsym(NULL){};
	virtual void accept(Visitor* v) {
		v->visitBinary(this);
	}
	virtual int getTag() {
		return opcode;
	}
	virtual ~Binary();
};

#endif /* TREE_BINARY_HPP_ */
