/*
 * NewArray.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_NEWARRAY_HPP_
#define TREE_NEWARRAY_HPP_
#include "Expression.hpp"
class NewArray :public Expression{

public:
	Expression* elemtype;//元素类型
	vector<Expression* > dims;//维度
	vector<Expression* > elems;//元素

	NewArray();
	NewArray(int pos,Expression* elemtype,
	vector<Expression* > dims,
	vector<Expression* > elems):Expression(pos),elemtype(elemtype),dims(dims),elems(elems){

	};
	virtual void accept(Visitor* v) {
		v->visitNewArray(this);
	}
	virtual int getTag() {
		return NEWARRAY;
	}
	virtual ~NewArray();
};

#endif /* TREE_NEWARRAY_HPP_ */
