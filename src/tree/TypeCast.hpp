/*
 * TypeCast.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_TYPECAST_HPP_
#define TREE_TYPECAST_HPP_
#include "Expression.hpp"
/**
 * 类型转换
 */
class TypeCast :public Expression{
public:
	Tree* clazz;
	Expression* expr;
	TypeCast();
	TypeCast(int pos,Tree* clazz,
	Expression* expr):Expression(pos),clazz(clazz),expr(expr){};
	virtual void accept(Visitor* v) {
		v->visitTypeCast(this);
	}
	virtual int getTag() {
		return TYPECAST;
	}
	virtual ~TypeCast();
};

#endif /* TREE_TYPECAST_HPP_ */
