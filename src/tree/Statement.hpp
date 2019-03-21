/*
 * Statement.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_STATEMENT_HPP_
#define TREE_STATEMENT_HPP_
#include "Tree.hpp"
/**
 * 语句;
 */
class Statement :public Tree{
public:
	Statement();
	Statement(int pos):Tree(pos){};
	virtual ~Statement();
};

#endif /* TREE_STATEMENT_HPP_ */
