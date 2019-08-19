/*
 * InstanceOf.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */
#ifndef TREE_INSTANCEOF_HPP_
#define TREE_INSTANCEOF_HPP_
#include "Expression.hpp"
/**
 * instance of
 */
class InstanceOf: public Expression {
public:
	Tree* clazz;
	Expression* expr;
	InstanceOf();
	InstanceOf(Tree* clazz, Expression* expr) :
			clazz(clazz), expr(expr) {
	}
	;
	virtual void accept(Visitor* v) {
		v->visitTypeTest(this);
	}
	virtual int getTag() {
		return TYPECAST;
	}
	virtual ~InstanceOf();
};

#endif /* TREE_INSTANCEOF_HPP_ */
