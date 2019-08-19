/*
 * PrimitiveTypeTree.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */
#ifndef TREE_PRIMITIVETYPETREE_HPP_
#define TREE_PRIMITIVETYPETREE_HPP_
#include "Expression.hpp"
/**
 * 基本类型
 */
class PrimitiveTypeTree: public Expression {
public:
	int typetag;
	PrimitiveTypeTree();
	PrimitiveTypeTree(int pos,int typetag) :Expression(pos),typetag(typetag) {
	}
	;
	int getKind() {
		return typetag;
	}
	int getTag() {
		return TYPEIDENT;
	}

	virtual void accept(Visitor* v) {
		v->visitTypeIdent(this);
	}
	virtual ~PrimitiveTypeTree();
};

#endif /* TREE_PRIMITIVETYPETREE_HPP_ */
