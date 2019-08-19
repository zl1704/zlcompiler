/*
 * If.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_IF_HPP_
#define TREE_IF_HPP_
#include "Statement.hpp"
/**
 * if ( ) { } else { }
 */
class If:public Statement {
public:
	Expression* cond;
	Statement* thenpart;
	Statement* elsepart;
	If();
	If(int pos,Expression* cond,
	Statement* thenpart,
	Statement* elsepart):Statement(pos),cond(cond),thenpart(thenpart),elsepart(elsepart){};
	virtual void accept(Visitor* v) {
		v->visitIf(this);
	}
	virtual int getTag() {
		return IF;
	}
	virtual ~If();
};

#endif /* TREE_IF_HPP_ */
