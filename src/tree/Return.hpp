/*
 * Return.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_RETURN_HPP_
#define TREE_RETURN_HPP_
#include "Statement.hpp"

class Return :public Statement{
public:
	Expression* expr;
	Return();
	Return(int pos,Expression* expr):Statement(pos),expr(expr){};
	virtual void accept(Visitor* v) {
		v->visitReturn(this);
	}
	virtual int getTag() {
		return RETURN;
	}
	virtual ~Return();
};

#endif /* TREE_RETURN_HPP_ */
