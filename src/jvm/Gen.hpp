/*
 * Gen.hpp
 *
 *  Created on: 2018年1月5日
 *      Author: ZL
 */

#ifndef JVM_GEN_HPP_
#define JVM_GEN_HPP_
#include "../util/global.hpp"
#include "../util/includeGlobal.hpp"
#include "../visitor/Pretty.hpp"
#include "../tree/Tree.hpp"
#include "../util/Name.hpp"
#include "../compiler/Check.hpp"
#include "Code.hpp"
#include "Item.hpp"
using namespace util;
/**
 * 代码生成阶段
 * start 2019.7.7
 */

class GenContext{
public:
	//当前环境exit
	Chain* exit;
	//当前环境continue
	Chain* cont;
	bool isSwitch;

	GenContext();


	void addExit(Chain* c);
	void addCont(Chain* c);

};


class Gen: public Visitor {
public:
	static int* genKey ;
	static Gen* instance();
	Gen();
	virtual ~Gen();

	/**
	 * gen methods
	 */
	void genClass(Env<AttrContext*>* env,ClassDecl* cdef);
	void genMethod(MethodDecl* md,Env<GenContext*>* env);
	int initCode(MethodDecl* md,Env<GenContext*>* env);

	void genDef(Tree* tree,Env<GenContext*>* env);
	void genStat(Tree* tree,Env<GenContext*>* env);
	template<class T>
	void genStats(vector<T> trees,Env<GenContext*>* env);
	void genArgs(vector<Expression*> trees,vector<Type* > ts);
	CondItem* genCond(Tree* tree);
	Item* genExpr(Tree* tree ,Type* pt);
	void genLoop(Statement* loop,Statement* body,Expression* cond,vector<ExpressionStatement*> step,bool testFirst);

	Item* completeBinop(Tree* lhs,Tree* rhs,OperatorSymbol* opsym);

	/**
	 * other method
	 */
	vector<Tree*> fillInitDefs(vector<Tree*> defs,ClassSymbol* csym);
	void fillMethod(MethodDecl* md ,vector<Statement*> initCode);
	//load 0
	int zero(int tc);

	int makeRef(Type* type);


	/**
	 * visit method
	 */

	virtual void visitTopLevel(CompilationUnit* tree);
	virtual void visitClassDef(ClassDecl* tree);
	virtual void visitMethodDef(MethodDecl* tree);
	virtual void visitVarDef(VariableDecl* tree);
	virtual void visitSkip(Skip* tree);
	virtual void visitBlock(Block* tree);
	virtual void visitDoLoop(DoWhileLoop* tree);
	virtual void visitWhileLoop(WhileLoop* tree);
	virtual void visitForLoop(ForLoop* tree);

	virtual void visitSwitch(Switch* tree);
	virtual void visitCase(Case* tree);

	virtual void visitConditional(Conditional* tree);
	virtual void visitIf(If* tree);
	virtual void visitExec(ExpressionStatement* tree);
	virtual void visitBreak(Break* tree);
	virtual void visitContinue(Continue* tree);
	virtual void visitReturn(Return* tree);
	virtual void visitApply(MethodInvocation* tree);
	virtual void visitNewClass(NewClass* tree);
	virtual void visitNewArray(NewArray* tree);
	virtual void visitParens(Parens* tree);
	virtual void visitAssign(Assign* tree);
	virtual void visitAssignop(AssignOp* tree);
	virtual void visitUnary(Unary* tree);
	virtual void visitBinary(Binary* tree);
	virtual void visitTypeCast(TypeCast* tree);
	virtual void visitTypeTest(InstanceOf* tree);
	virtual void visitIndexed(ArrayAccess* tree);
	virtual void visitSelect(FieldAccess* tree);
	virtual void visitIdent(Ident* tree);
	virtual void visitLiteral(Literal* tree);
	virtual void visitTypeIdent(PrimitiveTypeTree* tree);
	virtual void visitTypeArray(ArrayTypeTree* tree);
	virtual void visitModifiers(Modifiers* tree);
	virtual void visitTree(Tree* tree);


private :
	Symtab* syms;
	Check*  chk;
	Log* log;
	Type* methodType;
	Env<AttrContext*>* attrEnv;
	CompilationUnit* toplevel;

	// set by genMethod
	Code* code;
	Env<GenContext*>* env;
	Type* pt;
	Pool* pool;
	Items* items;


	Item* result;
	int nerrs ;
	int endPositions;

};



#endif /* JVM_GEN_HPP_ */
