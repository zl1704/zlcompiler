/*
 * WhileLoop.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_WHILELOOP_HPP_
#define TREE_WHILELOOP_HPP_
#include "Statement.hpp"
/**
 * while(){
 * 	...
 * };
 */
class WhileLoop: public Statement {
public:

	//循环体
	Statement* body;
	//条件
	Expression* cond;
	WhileLoop();
	WhileLoop(int pos,Expression* cond,Statement* body) :
		Statement(pos),body(body), cond(cond) {
	}
	;
	virtual void accept(Visitor* v) {
		v->visitWhileLoop(this);
	}
	virtual int getTag() {
		return WHILELOOP;
	}
	virtual ~WhileLoop();
};

#endif /* TREE_WHILELOOP_HPP_ */
