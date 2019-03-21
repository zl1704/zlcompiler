/*
 * Check.hpp
 *
 *  Created on: 2017年12月31日
 *      Author: ZL
 */

#ifndef COMPILER_CHECK_HPP_
#define COMPILER_CHECK_HPP_
#include "../util/includeGlobal.hpp"
#include "../util/global.hpp"
#include "../tree/TreeGlobal.hpp"

/*
 * 填充符号表阶段的检查器
 */
class Check {
public:
	static int* checkKey;
	static Check* instance();
	static Symbol* symbolFor(Tree* tree);
	static Symbol* symbol(Tree* tree);
	static Tree* skipParens(Tree* tree);
	static Expression* skipParens(Expression* tree);
	static string treeName(Tree* tree);
	//检查符号重复
	bool checkUnique(int pos,Symbol* sym,Scope* scope);
	bool checkUniqueClassName(int pos,string name,Scope* scope);
	//检查类型
	Type* checkType(int pos,Type* t,Type* s,string errKey);
	//检查方法的参数类型是否一致
	bool hasSameArgs(Type* t, Type* s);
	bool sameType(Type* t,Type* s);
	void checkEmptyIf(If* tree);
	Type* checkNonVoid(int pos, Type* t) ;

	void duplicateError(int pos,Symbol* sym);


	string kindName(int kind);
	Check();
	virtual ~Check();


private :
	Log* log;
	Symtab* syms;
};

#endif /* COMPILER_CHECK_HPP_ */
