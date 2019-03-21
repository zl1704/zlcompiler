/*
 * Enter.hpp
 *
 *  Created on: 2017年12月31日
 *      Author: ZL
 */

#ifndef COMPILER_ENTER_HPP_
#define COMPILER_ENTER_HPP_
#include "../util/includeGlobal.hpp"
#include "../tree/TreeGlobal.hpp"
#include "../util/global.hpp"
#include "Check.hpp"
#include "Env.hpp"
#include "Attr.hpp"
#include "../vistor/Pretty.hpp"
class Attr;
/**
 * 用于填充符号表
 */
class Enter: public Visitor {
public:
	static int* enterKey;
	//attr 阶段 队列
	vector<Env<AttrContext*>*> todos;
	//暂时保存  attr阶段会用到
	map<TypeSymbol*,Env<AttrContext* >*> typeEnvs;
	static Enter* instance();
	static void print(Env<AttrContext*>* env);
	Enter();
	void main(vector<CompilationUnit*> trees);
	void complete(ClassSymbol* c);
	void complete(vector<CompilationUnit*> trees, ClassSymbol* c);
	Env<AttrContext* >* topLevelEnv(CompilationUnit* tree);
	Env<AttrContext*>* classEnv(ClassDecl* tree,Env<AttrContext*>* env);
	Env<AttrContext*>* methodEnv(MethodDecl* tree,Env<AttrContext*>* env);
	Env<AttrContext*>* initEnv(VariableDecl* tree, Env<AttrContext*>*);
	Type* signature(Tree* restype,vector<VariableDecl*> params,Env<AttrContext* > * localEnv);

	Scope* enterScope(Env<AttrContext*>* env);

	//enter method
	vector<Type*> classEnter(vector<Tree*> trees, Env<AttrContext* >* env);
	Type* classEnter(Tree* tree, Env<AttrContext* >* env);

	void  memberEnter(vector<Tree*> trees,Env<AttrContext* >* env);
	void  memberEnter(Tree* tree,Env<AttrContext* >* env);

	bool hashConstructor(vector<Tree*> trees);
	MethodDecl* DefaultConstructor();


	/**
	 * visit method
	 */
	virtual void visitTopLevel(CompilationUnit* that);
	virtual void visitClassDef(ClassDecl* that);
	virtual void visitMethodDef(MethodDecl* that);
	virtual void visitVarDef(VariableDecl* that);
	virtual void visitSkip(Skip* that);
	virtual void visitBlock(Block* that);
	virtual void visitDoLoop(DoWhileLoop* that);
	virtual void visitWhileLoop(WhileLoop* that);
	virtual void visitForLoop(ForLoop* that);

	virtual void visitSwitch(Switch* that);
	virtual void visitCase(Case* that);

	virtual void visitConditional(Conditional* that);
	virtual void visitIf(If* that);
	virtual void visitExec(ExpressionStatement* that);
	virtual void visitBreak(Break* that);
	virtual void visitContinue(Continue* that);
	virtual void visitReturn(Return* that);
	virtual void visitApply(MethodInvocation* that);
	virtual void visitNewClass(NewClass* that);
	virtual void visitNewArray(NewArray* that);
	virtual void visitParens(Parens* that);
	virtual void visitAssign(Assign* that);
	virtual void visitAssignop(AssignOp* that);
	virtual void visitUnary(Unary* that);
	virtual void visitBinary(Binary* that);
	virtual void visitTypeCast(TypeCast* that);
	virtual void visitTypeTest(InstanceOf* that);
	virtual void visitIndexed(ArrayAccess* that);
	virtual void visitSelect(FieldAccess* that);
	virtual void visitIdent(Ident* that);
	virtual void visitLiteral(Literal* that);
	virtual void visitTypeIdent(PrimitiveTypeTree* that);
	virtual void visitTypeArray(ArrayTypeTree* that);
	virtual void visitModifiers(Modifiers* that);
	virtual void visitTree(Tree* that);

	virtual ~Enter();

private:
	Log* log;
	Symtab* syms;
	Check* chk;
	Attr* attr;
	ClassDecl* predefClassDef;
	//未完成队列
	vector<ClassSymbol*> uncompleted;
	Env<AttrContext*>* env;



	/** Visitor result
	 */
	Type* result;
	//true 第一阶段 ，否则第二阶段
	bool  first;


};

#endif /* COMPILER_ENTER_HPP_ */
