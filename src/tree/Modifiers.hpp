/*
 * Modifiers.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */
/**
 * 修饰符
 */
#ifndef TREE_MODIFIERS_HPP_
#define TREE_MODIFIERS_HPP_
#include "Tree.hpp"
class Modifiers :public Tree{
public:
	long long flags;
	Modifiers();
	Modifiers(int pos,long long flags):Tree(pos),flags(flags){};
	virtual void accept(Visitor* v) {
		v->visitModifiers(this);
	}
	int getTag() {
		return MODIFIERS;
	}
	virtual ~Modifiers();
};

#endif /* TREE_MODIFIERS_HPP_ */
