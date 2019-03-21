/*
 * TreeMaker.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef TREE_TREEMAKER_HPP_
#define TREE_TREEMAKER_HPP_
#include "TreeGlobal.hpp"
/**
 * Tree工厂
 */
class TreeMaker{
public :
	static CompilationUnit* TopLevel(int pos,vector<Tree* > defs,Source* s){
		return new CompilationUnit(pos,defs, s);
	};

	static Modifiers*  Mods(int pos,long long flags){
		return new Modifiers(pos,flags);
	}

	static Block* makeBlock(int pos,long long flags,vector<Statement* > stats){
		return new Block(pos,flags,stats);
	}

	static PrimitiveTypeTree* makeTypeIdent(int pos,int typetag){
		return new PrimitiveTypeTree(pos,typetag);
	}

	static VariableDecl* makeVarDef(int pos,Modifiers* mods,string name,Expression* type,Expression* init){
		return new VariableDecl(pos,mods,name,type,init);
	}

	static ArrayTypeTree* makeArray(int pos,Expression* t){
		return new ArrayTypeTree(pos,t);
	}

	static MethodDecl* makeMethodDecl(int pos,Modifiers* mods ,Expression* type,string name,vector<VariableDecl* > params,Block* body){
		return new MethodDecl(pos,mods,type,name,params,body);

	}

	static NewArray* makeNewArray(int pos,Expression* type,vector<Expression* > dims,vector<Expression*> elems){
		return new NewArray(pos,type,dims,elems);
	}

	static ArrayAccess* makeIndexed(int pos,Expression* indexed,Expression* index){
		return new ArrayAccess(pos,indexed,index);
	}

	static FieldAccess* makeSelect(int pos,Expression* select,string name){
		return new FieldAccess(pos,select,name);
	}

	static Assign*  makeAssign(int pos,Expression* lhs,Expression* rhs){
		return new Assign(pos,lhs,rhs);
	}

	static AssignOp* makeAssignOp(int pos,int opcode,Expression* lhs,Expression* rhs){
		return new AssignOp(pos,opcode,lhs,rhs);
	}

	static Conditional* makeConditional(int pos,Expression* cond,Expression* truepart,Expression* falsepart){
		return new Conditional(pos,cond,truepart,falsepart);
	}

	static Binary* makeBinary(int pos,int opcode,Expression* lhs,Expression* rhs ){

		return new Binary(pos,opcode,lhs,rhs);
	}

	static Unary* makeUnary(int pos,int opcode,Expression* arg){
		return new Unary(pos,opcode,arg);
	}

	static NewClass* makeNewClass(int pos,vector<Expression* > args,Expression* clazz){
		return new NewClass(pos,NULL,args,NULL,clazz);
	}

	static ClassDecl* makeClassDecl(int pos,Modifiers* mods,string name,vector<Tree* >defs){
		return new ClassDecl(pos,mods,name,NULL,defs);
	}

	static Literal* makeLiteral(int pos,int type,string value){
		return new Literal(pos,type,value);
	}

	static TypeCast* makeTypeCast(int pos,Expression* clazz ,Expression* expr ){
		return new TypeCast(pos,clazz,expr);
	}

	static Parens* makeParens(int pos,Expression* expr){
		return new Parens(pos,expr);
	}

	static Ident* makeIdent(int pos,string name){
		return new Ident(pos,name);
	}

	static MethodInvocation* makeApply(int pos,Expression* meth,vector<Expression* > args){
		return new MethodInvocation(pos,meth,args);
	}


	static ExpressionStatement* makeExec(int pos,Expression* expr){
		return new ExpressionStatement(pos,expr);
	}

	static If* makerIf(int pos,Expression* cond,Statement* thenpart,Statement* elsepart){
		return new If(pos,cond,thenpart,elsepart);
	}

	static ForLoop* makeForLoop(int pos,vector<Statement* > init,Expression* cond,vector<ExpressionStatement* > step,Statement* body){
		return new ForLoop(pos,init,cond,step,body);
	}

	static WhileLoop* makeWhileLoop(int pos, Expression* cond,Statement* body){
		return new WhileLoop(pos,cond,body);

	}

	static DoWhileLoop* makeDoLoop(int pos,Statement* body,Expression* cond){
		return new DoWhileLoop(pos,body,cond);

	}

	static Case* makeCase(int pos,Expression* pat, vector<Statement*> stats){
		return new Case(pos,pat,stats);

	}

	static Switch* makeSwitch(int pos,Expression* selector, vector<Case*> cases){
		return new Switch(pos,selector,cases);

	}

	static Return* makeReturn(int pos,Expression* result){
		return new Return(pos,result);
	}

	static Break* makeBreak(int pos,string label){
		return new Break(pos,label,NULL);
	}

	static Continue* makeContinue(int pos,string label){
		return new Continue(pos,label,NULL);
	}

	static Skip* makeSkip(int pos){
		return new Skip(pos);
	}
};




#endif /* TREE_TREEMAKER_HPP_ */
