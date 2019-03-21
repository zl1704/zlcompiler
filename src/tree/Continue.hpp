/*
 * Continue.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_CONTINUE_HPP_
#define TREE_CONTINUE_HPP_
#include "Statement.hpp"
class Continue :public Statement{
public:
	string label;
	Tree* target ;
	Continue();
	Continue(int pos,string label,
			Tree* target):Statement(pos),label(label.data()),target(target){};
	virtual void accept(Visitor* v) {
		v->visitContinue(this);
	}
	virtual int getTag() {
		return CONTINUE;
	}
	virtual ~Continue();
};

#endif /* TREE_CONTINUE_HPP_ */
