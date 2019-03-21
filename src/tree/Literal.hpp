/*
 * Literal.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */
#ifndef TREE_LITERAL_HPP_
#define TREE_LITERAL_HPP_

/**
 * 字面常量
 */
#include "Expression.hpp"
class Literal :public Expression {
public:
	int typetag;
	string value;
	Literal();
	Literal(int pos,int typetag,
	string value):Expression(pos),typetag(typetag),value(value){};
	int getKind(){
		return typetag;
	};

	virtual void accept(Visitor* v) {
		v->visitLiteral(this);
	}
	virtual int getTag() {
		return LITERAL;
	}
	virtual ~Literal();
};

#endif /* TREE_LITERAL_HPP_ */
