/*
 * ArrayAccess.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_ARRAYACCESS_HPP_
#define TREE_ARRAYACCESS_HPP_
#include "Expression.hpp"
/**
 * 数组访问
 */
class ArrayAccess:public Expression {
public:
	//被访问数组
	Expression* indexed;
	//索引
	Expression* index;

	ArrayAccess();
	ArrayAccess(int pos,
			Expression* indexed,
			Expression* index):Expression(pos),indexed(indexed),index(index){};
	virtual void accept(Visitor* v) {
		v->visitIndexed(this);
	}
	virtual int getTag() {
		return INDEXED;
	}
	virtual ~ArrayAccess();
};

#endif /* TREE_ARRAYACCESS_HPP_ */
