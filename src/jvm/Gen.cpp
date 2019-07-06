/*
 * Gen.cpp
 *
 *  Created on: 2018年1月5日
 *      Author: ZL
 */

#include "Gen.hpp"
int* Gen::genKey = new int ;
Gen* Gen::instance(){

}


Gen::Gen() {
	 Context::put(genKey, this);
	 log = Log::instance();
	 syms = Symtab::instance();
	 chk = Check::instance();


}

Gen::~Gen() {
}
/**
 * gen method
 */
void Gen::genDef(Tree* tree, Env<AttrContext*> env);
void Gen::genStat(Tree* tree, Env<AttrContext*> env);
template<class T>
void Gen::genStats(vector<T> trees, Env<AttrContext*> env);
void Gen::genArgs(vector<Expression*> trees, vector<Type*> ts);
/**
 *	方法代码生成前准备工作
 *	1.(静态)成员变量初始化和(静态)构造块中代码添加到(静态)构造方法中
 *	2. 进入各方法生成代码
 */
void Gen::genClass(Env<AttrContext*>* env, ClassDecl* cdef){



}







void Gen::visitTopLevel(CompilationUnit* tree) {
}



void Gen::visitClassDef(ClassDecl* tree) {
}
void Gen::visitMethodDef(MethodDecl* tree) {
}
void Gen::visitVarDef(VariableDecl* tree) {
}
void Gen::visitSkip(Skip* tree) {
}
void Gen::visitBlock(Block* tree) {
}
void Gen::visitDoLoop(DoWhileLoop* tree) {
}
void Gen::visitWhileLoop(WhileLoop* tree) {
}
void Gen::visitForLoop(ForLoop* tree) {
}

void Gen::visitSwitch(Switch* tree) {
}
void Gen::visitCase(Case* tree) {
}

void Gen::visitConditional(Conditional* tree) {
}
void Gen::visitIf(If* tree) {
}
void Gen::visitExec(ExpressionStatement* tree) {
}
void Gen::visitBreak(Break* tree) {
}
void Gen::visitContinue(Continue* tree) {
}
void Gen::visitReturn(Return* tree) {
}
void Gen::visitApply(MethodInvocation* tree) {
}
void Gen::visitNewClass(NewClass* tree) {
}
void Gen::visitNewArray(NewArray* tree) {
}
void Gen::visitParens(Parens* tree) {
}
void Gen::visitAssign(Assign* tree) {
}
void Gen::visitAssignop(AssignOp* tree) {
}
void Gen::visitUnary(Unary* tree) {
}
void Gen::visitBinary(Binary* tree) {
}
void Gen::visitTypeCast(TypeCast* tree) {
}
void Gen::visitTypeTest(InstanceOf* tree) {
}
void Gen::visitIndexed(ArrayAccess* tree) {
}
void Gen::visitSelect(FieldAccess* tree) {
}
void Gen::visitIdent(Ident* tree) {
}
void Gen::visitLiteral(Literal* tree) {
}
void Gen::visitTypeIdent(PrimitiveTypeTree* tree) {
}
void Gen::visitTypeArray(ArrayTypeTree* tree) {
}
void Gen::visitModifiers(Modifiers* tree) {
}
void Gen::visitTree(Tree* tree) {
}
