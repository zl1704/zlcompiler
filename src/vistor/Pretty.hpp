/*
 * Pretty.hpp
 *
 *  Created on: 2017年12月27日
 *      Author: ZL
 */

#ifndef VISTOR_PRETTY_HPP_
#define VISTOR_PRETTY_HPP_
#include "../tree/TreeGlobal.hpp"
#include "../util/global.hpp"
#include <exception>
/**
 * Prints out a tree as a   source program.
 */
class Pretty: public Visitor{
public:
	static  int* prettyKey ;
	int width;
	static Pretty* instance();

	Pretty();
	virtual ~Pretty();
	/***
	 * print methods
	 */

	void printUnit(CompilationUnit* tree, ClassDecl* cdef);
	void printStat(Tree* tree);
	template<class T>
	void printStats(vector<T> trees);

	void printExpr(Tree* tree,int prec);
	void printExpr(Tree* tree);

	template<class T>
	void printExprs(vector<T> trees ,string sep);

	template<class T>
	void printExprs(vector<T> trees);

	template<class T>
	void printBlock(vector<T > stats) ;

	void printBrackets(ArrayTypeTree* tree) ;

	void printFlags(long long flags);

	/**
	 * visit methods
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
	/** The current left margin.
	 */
	int lmargin;
	int prec;

	string enclClassName;

	inline void indent() {
		lmargin = lmargin + width;
	}
	inline void undent() {
		lmargin = lmargin - width;
	}
	inline void open(int contextPrec, int ownPrec) {
		if (ownPrec < contextPrec)
			cout << "(";
	}
	inline void close(int contextPrec, int ownPrec) {
		if (ownPrec < contextPrec)
			cout << ")";
	}
	inline void println() {
		cout << endl;
	}
	inline void print(string s) {
			cout << s;
	}

	inline void align(){
	        for (int i = 0; i < lmargin; i++)cout <<" ";
	    }
	void printBaseElementType(Tree* tree);
	Tree* innermostType(Tree* tree);
};

#endif /* VISTOR_PRETTY_HPP_ */
