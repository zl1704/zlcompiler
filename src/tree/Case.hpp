/*
 * Case.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_CASE_HPP_
#define TREE_CASE_HPP_
#include "Statement.hpp"
/**
 * case  :" of a switch
 */
class Case: public Statement {
public:
	//
	Expression* pat;
	//
	vector<Statement*> stats;

	Case();
	Case(int pos,Expression* pat, vector<Statement*> stats) :
		Statement(pos),pat(pat) {
		this->stats.assign(stats.begin(), stats.end());
	}
	;
	virtual void accept(Visitor* v) {
		v->visitCase(this);
	}
	virtual int getTag() {
		return CASE;
	}
	virtual ~Case();
};

#endif /* TREE_CASE_HPP_ */
