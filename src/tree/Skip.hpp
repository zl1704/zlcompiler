/*
 * Skip.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_SKIP_HPP_
#define TREE_SKIP_HPP_
#include "Statement.hpp"
/**
 * 空语句  ，例如 单独一个;
 * 可以直接跳过此节点
b */
class Skip:public Statement {
public:
	Skip();
	Skip(int pos):Statement(pos){};
	virtual void accept(Visitor* v){v->visitSkip(this);};
	virtual int getTag(){return SKIP;};
	virtual ~Skip();
};

#endif /* TREE_SKIP_HPP_ */
