/*
 * Break.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_BREAK_HPP_
#define TREE_BREAK_HPP_
#include "Statement.hpp"
class Break :public Statement{

public:
	string label;
	Tree* target;
	Break();
	Break(int pos,string label,
	Tree* target):Statement(pos),label(label),target(target){};
	virtual void accept(Visitor* v) {
		v->visitBreak(this);
	}
	virtual int getTag() {
		return BREAK;
	}
	virtual ~Break();
};

#endif /* TREE_BREAK_HPP_ */
