/*
 * Compiler.h
 *
 *  Created on: 2017年12月4日
 *      Author: ZL
 */

#ifndef COMPILER_COMPILER_HPP_
#define COMPILER_COMPILER_HPP_

#include "../util/global.hpp"
#include "../parser/Lexer.hpp"
#include "../parser/Parser.hpp"
#include "../util/Source.hpp"
#include "../visitor/Pretty.hpp"
#include "../util/includeGlobal.hpp"
#include "Enter.hpp"
#include "../jvm/Gen.hpp"

//编译器入口
class Compiler{

public :
	static  int* compilerKey ;
	static Compiler* instance();
	Compiler();
	void compile(string fileName);
protected:
	void compile2();
	void generate(Env<AttrContext* >* env);
	/**
	 * 核心类
	 */

	//词法分析
	Lexer* lexer;
	//语法分析
	Parser* parser;
	//填充符号表  ,第一阶段
	Enter* enter;
	//属性标注+第二阶段填充符号表
	Attr* attr;
	//代码生成
	Gen* gen;
	//日志类
	Log* log;
};

#endif /* COMPILER_COMPILER_HPP_ */
