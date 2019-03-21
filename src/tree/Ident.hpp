/*
 * Ident.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */
#ifndef TREE_IDENT_HPP_
#define TREE_IDENT_HPP_
#include "Expression.hpp"
/**
 * 标识符
 */
class Ident:public Expression {
public:
	string name;
	Symbol* sym;
	Ident();
	Ident(int pos,string name):Expression(pos),name(name.data()){
		sym = NULL;
	};
	virtual void accept(Visitor* v) {
		v->visitIdent(this);
	}
	virtual int getTag() {
		return IDENT;
	}
	virtual ~Ident();
};

#endif /* TREE_IDENT_HPP_ */
