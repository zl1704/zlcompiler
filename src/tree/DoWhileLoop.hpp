/*
 * DoWhileLoop.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_DOWHILELOOP_HPP_
#define TREE_DOWHILELOOP_HPP_
#include "Statement.hpp"
/**
 * do{
 * 	...
 * }while();
 */
class DoWhileLoop: public Statement {
public:
	//循环体
	Statement* body;
	//条件
	Expression* cond;
	DoWhileLoop();
	DoWhileLoop(int pos,Statement* body,Expression* cond):Statement(pos),body(body),cond(cond){};
	virtual void accept(Visitor* v) { v->visitDoLoop(this); }
	virtual int getTag() {
		return DOLOOP;
	}
	virtual ~DoWhileLoop();
};

#endif /* TREE_DOWHILELOOP_HPP_ */
