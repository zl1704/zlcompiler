/*
 * Expression.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_EXPRESSION_HPP_
#define TREE_EXPRESSION_HPP_
#include "Tree.hpp"
class Expression :public Tree{
public:
	Expression();
	Expression(int pos):Tree(pos){};
	virtual ~Expression();
};

#endif /* TREE_EXPRESSION_HPP_ */
