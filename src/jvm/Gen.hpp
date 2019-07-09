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
#include "../tree/Tree.hpp"
#include "../util/Name.hpp"
#include "../compiler/Check.hpp"
/**
 * 代码生成阶段
 * start 2019.7.7
 */
class Gen: public Visitor {
public:
	static int* genKey ;
	static Gen* instance();
	Gen();
	virtual ~Gen();

	/**
	 * gen method
	 */
	void genClass(Env<AttrContext*>* env,ClassDecl* cdef);
	void genDef(Tree* tree,Env<AttrContext*>* env);
	void genStat(Tree* tree,Env<AttrContext*>* env);
	template<class T>
	void genStats(vector<T> trees,Env<AttrContext*>* env);
	void genArgs(vector<Expression*> trees,vector<Type* > ts);

	/**
	 * other method
	 */
	vector<Tree*> fillInitDefs(vector<Tree*> defs,ClassSymbol* csym);
	void fillMethod(MethodDecl* md ,vector<Statement*> initCode);


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
	CompilationUnit toplevel;

	int nerrs ;
	int endPositions;

};

#endif /* JVM_GEN_HPP_ */
