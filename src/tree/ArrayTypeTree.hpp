/*
 * ArrayTypeTree.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_ARRAYTYPETREE_HPP_
#define TREE_ARRAYTYPETREE_HPP_
#include "Expression.hpp"
/**
 * 数组类型  A[]
 */
class ArrayTypeTree: public Expression {
public:
	Expression* elemtype;
	ArrayTypeTree();
	ArrayTypeTree(int pos,Expression* elemtype) :
		Expression(pos),elemtype(elemtype) {
	}
	;

	virtual void accept(Visitor* v) {
		v->visitTypeArray(this);
	}
	int getTag() {
		return TYPEARRAY;
	}
	virtual ~ArrayTypeTree();
};

#endif /* TREE_ARRAYTYPETREE_HPP_ */
