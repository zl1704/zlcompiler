/*
 * Block.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_BLOCK_HPP_
#define TREE_BLOCK_HPP_
#include "Statement.hpp"
/**
 * 语句块
 * {
 * 	...
 * }
 */
class Block:public Statement {
public:
	//语句块 ，静态语句块
	vector<Statement* > stats;
	long long flags ;
	int endPos;
	Block();
	Block(int pos,long long flags,vector<Statement* > stats):Statement(pos),flags(flags),stats(stats){

	};
	bool isStatic() { return (flags & Flags::STATIC) != 0; }
	virtual void accept(Visitor* v){v->visitBlock(this);};
	virtual int getTag(){return BLOCK;};
	virtual ~Block();
};

#endif /* TREE_BLOCK_HPP_ */
