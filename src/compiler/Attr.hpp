/*
 * Attr.hpp
 *
 *  Created on: 2018年1月2日
 *      Author: ZL
 */

#ifndef PARSER_ATTR_HPP_
#define PARSER_ATTR_HPP_
#include "ConstFold.hpp"
#include "Enter.hpp"
class Enter;
/**
 * 属性标注
 */
class Attr:public Visitor {
public:

	static Attr* instance();
	Attr();
	virtual ~Attr();

	/**
	 * attr method
	 */
	Type* attribType(Tree* tree,Env<AttrContext*>* env);
	Type* attribType(Tree* tree,Env<AttrContext*>* env,Type* pt);
	Type* attribTree(Tree* tree,Env<AttrContext*>* env,int pkind,Type* pt);
	Env<AttrContext* >* attribute(Env<AttrContext*>* env);
	Type* attribStat(Tree* tree,Env<AttrContext*>* env);
	template<class T>
	Type* attribStats(vector<T> trees,Env<AttrContext*>* env);
	Type* attribExpr(Expression* tree,Env<AttrContext*>* env,Type* pt);
	Type* attribExpr(Expression* tree, Env<AttrContext*>* env,int pkind, Type* pt) ;
	vector<Type*> attribExprs(vector<Expression*> trees,Env<AttrContext*>* env,Type* pt);
	vector<Type*> attribArgs(vector<Expression* >trees,Env<AttrContext*>* env);


	void addVars(vector<Statement*> stats, Scope* switchScope);
	//find method
	Type* check(Tree* tree,Type* owntype, Type* pt,int ownkind,int pkind,string errKey);
	Type* litType(int tag,string strVal);
	Symbol* findIdent(int pos,Env<AttrContext*>* env,string name,int kind);
	Symbol* findType(Env<AttrContext*>* env,string name);
	Symbol* findVar(Env<AttrContext*>* env,string name);
	Symbol* resolveBinaryOperator(int pos,Env<AttrContext*>* env,int optag,Type* lef,Type* right);
	Symbol* resolveUnaryOperator(int pos,int optag,Env<AttrContext*>* env,Type* argtype);
	Symbol* findMethod(int pos,Scope* scope,string name,vector<Type* > argtypes,string className = "");
	Symbol* findField(int pos,Scope* s,string name,Type* t);
	Symbol* selectSym(FieldAccess* tree,Symbol* sitesym,Type* site,Type* pt,int pkind);

	/**
	 * visitor
	 */
	virtual void visitTopLevel(CompilationUnit* tree);
	virtual void visitClassDef(ClassDecl* tree);
	virtual void visitMethodDef(MethodDecl* tree) ;
	virtual void visitVarDef(VariableDecl* tree);
	virtual void visitSkip(Skip* tree);
	virtual void visitBlock(Block* tree);
	virtual void visitDoLoop(DoWhileLoop* tree);
	virtual void visitWhileLoop(WhileLoop* tree);
	virtual void visitForLoop(ForLoop* tree) ;

	virtual void visitSwitch(Switch* tree);
	virtual void visitCase(Case* tree) ;

	virtual void visitConditional(Conditional* tree);
	virtual void visitIf(If* tree) ;
	virtual void visitExec(ExpressionStatement* tree) ;
	virtual void visitBreak(Break* tree) ;
	virtual void visitContinue(Continue* tree);
	virtual void visitReturn(Return* tree);
	virtual void visitApply(MethodInvocation* tree) ;
	virtual void visitNewClass(NewClass* tree) ;
	virtual void visitNewArray(NewArray* tree);
	virtual void visitParens(Parens* tree) ;
	virtual void visitAssign(Assign* tree);
	virtual void visitAssignop(AssignOp* tree) ;
	virtual void visitUnary(Unary* tree) ;
	virtual void visitBinary(Binary* tree) ;
	virtual void visitTypeCast(TypeCast* tree) ;
	virtual void visitTypeTest(InstanceOf* tree);
	virtual void visitIndexed(ArrayAccess* tree);
	virtual void visitSelect(FieldAccess* tree);
	virtual void visitIdent(Ident* tree) ;
	virtual void visitLiteral(Literal* tree) ;
	virtual void visitTypeIdent(PrimitiveTypeTree* tree) ;
	virtual void visitTypeArray(ArrayTypeTree* tree);
	virtual void visitModifiers(Modifiers* tree) ;
	virtual void visitTree(Tree* tree);


private:
	static int* attrKey;
		Type* result;
		Symtab* syms;
		Check* chk;
		Enter* enter;
		Env<AttrContext*>* env;
		Type* pt;
		int pkind;
		Log* log;
		ConstFold* cfolder;
		void attribute(ClassSymbol* c);
		void attriClassBody(Env<AttrContext*>* env,ClassSymbol* c);
};

#endif /* PARSER_ATTR_HPP_ */
