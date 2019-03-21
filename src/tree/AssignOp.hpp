/*
 * AssignOp.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_ASSIGNOP_HPP_
#define TREE_ASSIGNOP_HPP_
#include "Expression.hpp"
/**
 * An assignment with "+=", "|=" ...
 */
class AssignOp:public Expression {
public:
	//操作符
	int opcode;
	Expression* lhs;
	Expression* rhs;

	Symbol* opsym;
	AssignOp();
	AssignOp(int pos,int opcode,
	Expression* lhs,
	Expression* rhs):Expression(pos),opcode(opcode),lhs(lhs),rhs(rhs){};
	virtual void accept(Visitor* v) {
		v->visitAssignop(this);
	}
	virtual int getTag() {
		return opcode;
	}
	virtual ~AssignOp();
};

#endif /* TREE_ASSIGNOP_HPP_ */
