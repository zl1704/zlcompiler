/*
 * FieldAccess.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_FIELDACCESS_HPP_
#define TREE_FIELDACCESS_HPP_
#include "Expression.hpp"
/**
 * 属性字段访问
 */
class FieldAccess :public Expression {

public:
	Expression* selected;
	string name;
	Symbol* sym;
	FieldAccess();
	FieldAccess(int pos,Expression* selected,
	string name	):Expression(pos),selected(selected),name(name.data()),sym(0){};
	virtual void accept(Visitor* v) {
		v->visitSelect(this);
	}
	virtual int getTag() {
		return SELECT;
	}
	virtual ~FieldAccess();
};

#endif /* TREE_FIELDACCESS_HPP_ */
