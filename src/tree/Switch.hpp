/*
 * Switch.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_SWITCH_HPP_
#define TREE_SWITCH_HPP_
#include "Statement.hpp"
/**
 * switch ( ) { }
 */
class Switch: public Statement {
public:
	//选择表达式
	Expression* selector;
	//cases
	vector<Case*> cases;

	Switch();
	Switch(int pos,Expression* selector, vector<Case*> cases) :
		Statement(pos),selector(selector) {
		this->cases.assign(cases.begin(), cases.end());
	}
	;
	virtual void accept(Visitor* v) {
		v->visitSwitch(this);
	}
	virtual int getTag() {
		return SWITCH;
	}
	virtual ~Switch();
};

#endif /* TREE_SWITCH_HPP_ */
