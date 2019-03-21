/*
 * ForLoop.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_FORLOOP_HPP_
#define TREE_FORLOOP_HPP_
#include "Statement.hpp"
/**
 * for 循环
 */
class ForLoop:public Statement {
public:
	//初始化语句
	vector<Statement* > init;
	//条件
	Expression* cond;
	//步长
	vector<ExpressionStatement* > step;
	//循环体
	Statement* body;

	ForLoop();
	ForLoop(int pos,vector<Statement* > init,Expression* cond,vector<ExpressionStatement* > step,Statement* body):
		Statement(pos),cond(cond),body(body){
		this->init.assign(init.begin(),init.end());
		this->step.assign(step.begin(),step.end());
	};
	virtual void accept(Visitor* v) {
		v->visitForLoop(this);
	}
	virtual int getTag() {
		return FOREACHLOOP;
	}
	virtual ~ForLoop();
};

#endif /* TREE_FORLOOP_HPP_ */
