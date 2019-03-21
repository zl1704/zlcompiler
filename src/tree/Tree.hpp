/*
 * Tree.hpp
 *
 *  Created on: 2017年12月7日
 *      Author: ZL
 */

#ifndef TREE_TREE_HPP_
#define TREE_TREE_HPP_
#include "../util/global.hpp"
#include "../util/includeGlobal.hpp"
#include "../code/Symbol.hpp"
/**
 * 前置声明
 */
class Type;
class Visitor;
class Tree;
class ClassDecl;
class CompilationUnit;
class ArrayAccess;
class ArrayTypeTree;
class Assign;
class AssignOp;
class Binary;
class Block;
class Break;
class Case;
class ClassDecl;
class CompilationUnit;
class Conditional;
class Continue;
class DoWhileLoop;
class Expression;
class ExpressionStatement;
class FieldAccess;
class ForLoop;
class Ident;
class If;
class InstanceOf;
class Literal;
class MethodDecl;
class MethodInvocation;
class Modifiers;
class NewArray;
class NewClass;
class Parens;
class PrimitiveTypeTree;
class Return;
class Skip;
class Statement;
class Switch;
class TypeCast;
class Unary;
class VariableDecl;
class WhileLoop;
class Tree {
public:
	/* Tree tag values, identifying kinds of trees */

	/** Toplevel nodes, of type TopLevel, representing entire source files.
	 */
	static const int TOPLEVEL = 1;

	/** Import clauses, of type Import.
	 */
	static const int IMPORT = TOPLEVEL + 1;

	/** Class definitions, of type ClassDef.
	 */
	static const int CLASSDEF = IMPORT + 1;

	/** Method definitions, of type MethodDef.
	 */
	static const int METHODDEF = CLASSDEF + 1;

	/** Variable definitions, of type VarDef.
	 */
	static const int VARDEF = METHODDEF + 1;

	/** The no-op statement ";", of type Skip
	 */
	static const int SKIP = VARDEF + 1;

	/** Blocks, of type Block.
	 */
	static const int BLOCK = SKIP + 1;

	/** Do-while loops, of type DoLoop.
	 */
	static const int DOLOOP = BLOCK + 1;

	/** While-loops, of type WhileLoop.
	 */
	static const int WHILELOOP = DOLOOP + 1;

	/** For-loops, of type ForLoop.
	 */
	static const int FORLOOP = WHILELOOP + 1;

	/** Foreach-loops, of type ForeachLoop.
	 */
	static const int FOREACHLOOP = FORLOOP + 1;

	/** Labelled statements, of type Labelled.
	 */
	static const int LABELLED = FOREACHLOOP + 1;

	/** Switch statements, of type Switch.
	 */
	static const int SWITCH = LABELLED + 1;

	/** Case parts in switch statements, of type Case.
	 */
	static const int CASE = SWITCH + 1;

	/** Synchronized statements, of type Synchonized.
	 */
	static const int SYNCHRONIZED = CASE + 1;

	/** Try statements, of type Try.
	 */
	static const int TRY = SYNCHRONIZED + 1;

	/** Catch clauses in try statements, of type Catch.
	 */
	static const int CATCH = TRY + 1;

	/** Conditional expressions, of type Conditional.
	 */
	static const int CONDEXPR = CATCH + 1;

	/** Conditional statements, of type If.
	 */
	static const int IF = CONDEXPR + 1;

	/** Expression statements, of type Exec.
	 */
	static const int EXEC = IF + 1;

	/** Break statements, of type Break.
	 */
	static const int BREAK = EXEC + 1;

	/** Continue statements, of type Continue.
	 */
	static const int CONTINUE = BREAK + 1;

	/** Return statements, of type Return.
	 */
	static const int RETURN = CONTINUE + 1;

	/** Throw statements, of type Throw.
	 */
	static const int THROW = RETURN + 1;

	/** Assert statements, of type Assert.
	 */
	static const int ASSERT = THROW + 1;

	/** Method invocation expressions, of type Apply.
	 */
	static const int APPLY = ASSERT + 1;

	/** Class instance creation expressions, of type NewClass.
	 */
	static const int NEWCLASS = APPLY + 1;

	/** Array creation expressions, of type NewArray.
	 */
	static const int NEWARRAY = NEWCLASS + 1;

	/** Parenthesized subexpressions, of type Parens.
	 */
	static const int PARENS = NEWARRAY + 1;

	/** Assignment expressions, of type Assign.
	 */
	static const int ASSIGN = PARENS + 1;

	/** Type cast expressions, of type TypeCast.
	 */
	static const int TYPECAST = ASSIGN + 1;

	/** Type test expressions, of type TypeTest.
	 */
	static const int TYPETEST = TYPECAST + 1;

	/** Indexed array expressions, of type Indexed.
	 */
	static const int INDEXED = TYPETEST + 1;

	/** Selections, of type Select.
	 */
	static const int SELECT = INDEXED + 1;

	/** Simple identifiers, of type Ident.
	 */
	static const int IDENT = SELECT + 1;

	/** Literals, of type Literal.
	 */
	static const int LITERAL = IDENT + 1;

	/** Basic type identifiers, of type TypeIdent.
	 */
	static const int TYPEIDENT = LITERAL + 1;

	/** Array types, of type TypeArray.
	 */
	static const int TYPEARRAY = TYPEIDENT + 1;

	/** Parameterized types, of type TypeApply.
	 */
	static const int TYPEAPPLY = TYPEARRAY + 1;

	/** Union types, of type TypeUnion
	 */
	static const int TYPEUNION = TYPEAPPLY + 1;

	/** Formal type parameters, of type TypeParameter.
	 */
	static const int TYPEPARAMETER = TYPEUNION + 1;

	/** Type argument.
	 */
	static const int WILDCARD = TYPEPARAMETER + 1;

	/** Bound kind: extends, super, exact, or unbound
	 */
	static const int TYPEBOUNDKIND = WILDCARD + 1;

	/** metadata: Annotation.
	 */
	static const int ANNOTATION = TYPEBOUNDKIND + 1;

	/** metadata: Modifiers
	 */
	static const int MODIFIERS = ANNOTATION + 1;

	static const int ANNOTATED_TYPE = MODIFIERS + 1;

	/** Error trees, of type Erroneous.
	 */
	static const int ERRONEOUS = ANNOTATED_TYPE + 1;

	/** Unary operators, of type Unary.
	 */
	static const int POS = ERRONEOUS + 1;             // +
	static const int NEG = POS + 1;                   // -
	static const int NOT = NEG + 1;                   // !
	static const int COMPL = NOT + 1;                 // ~
	static const int PREINC = COMPL + 1;              // ++ _
	static const int PREDEC = PREINC + 1;             // -- _
	static const int POSTINC = PREDEC + 1;            // _ ++
	static const int POSTDEC = POSTINC + 1;           // _ --

	/** unary operator for null reference checks, only used internally.
	 */
	static const int NULLCHK = POSTDEC + 1;

	/** Binary operators, of type Binary.
	 */
	static const int OR = NULLCHK + 1;                // ||
	static const int AND = OR + 1;                    // &&
	static const int BITOR = AND + 1;                 // |
	static const int BITXOR = BITOR + 1;              // ^
	static const int BITAND = BITXOR + 1;             // &
	static const int EQ = BITAND + 1;                 // ==
	static const int NE = EQ + 1;                     // !=
	static const int LT = NE + 1;                     // <
	static const int GT = LT + 1;                     // >
	static const int LE = GT + 1;                     // <=
	static const int GE = LE + 1;                     // >=
	static const int SL = GE + 1;                     // <<
	static const int SR = SL + 1;                     // >>
	static const int USR = SR + 1;                    // >>>
	static const int PLUS = USR + 1;                  // +
	static const int MINUS = PLUS + 1;                // -
	static const int MUL = MINUS + 1;                 // *
	static const int DIV = MUL + 1;                   // /
	static const int MOD = DIV + 1;                   // %

	/** Assignment operators, of type Assignop.
	 */
	static const int BITOR_ASG = MOD + 1;             // |=
	static const int BITXOR_ASG = BITOR_ASG + 1;      // ^=
	static const int BITAND_ASG = BITXOR_ASG + 1;     // &=

	static const int SL_ASG = SL + BITOR_ASG - BITOR; // <<=
	static const int SR_ASG = SL_ASG + 1;             // >>=
	static const int USR_ASG = SR_ASG + 1;            // >>>=
	static const int PLUS_ASG = USR_ASG + 1;          // +=
	static const int MINUS_ASG = PLUS_ASG + 1;        // -=
	static const int MUL_ASG = MINUS_ASG + 1;         // *=
	static const int DIV_ASG = MUL_ASG + 1;           // /=
	static const int MOD_ASG = DIV_ASG + 1;           // %=

	/** A synthetic let expression, of type LetExpr.
	 */
	static const int LETEXPR = MOD_ASG + 1;           // ala scheme

	/** The offset between assignment operators and normal operators.
	 */
	static const int ASGOffset = BITOR_ASG - BITOR;

	int pos;
	Type* type;
	Tree(){}
	Tree(int pos):pos(pos) {
		type = NULL;
	}
	;
	virtual void accept(Visitor* v)=0;
	virtual int getTag()=0;

};
class TreeInfo{
public:
 static const int
        notExpression = -1,   // not an expression
        noPrec = 0,           // no enclosing expression
        assignPrec = 1,
        assignopPrec = 2,
        condPrec = 3,
        orPrec = 4,
        andPrec = 5,
        bitorPrec = 6,
        bitxorPrec = 7,
        bitandPrec = 8,
        eqPrec = 9,
        ordPrec = 10,
        shiftPrec = 11,
        addPrec = 12,
        mulPrec = 13,
        prefixPrec = 14,
        postfixPrec = 15,
        precCount = 16;


static int opPrec(int op);
static string flagNames(long long flags);
static string operatorName(int tag) ;
};




class Visitor {
public:
	Visitor(){};
	virtual void visitTopLevel(CompilationUnit* that)=0;
	virtual void visitClassDef(ClassDecl* that)=0;
	virtual void visitMethodDef(MethodDecl* that) =0;
	virtual void visitVarDef(VariableDecl* that)=0;
	virtual void visitSkip(Skip* that)=0;
	virtual void visitBlock(Block* that)=0;
	virtual void visitDoLoop(DoWhileLoop* that)=0;
	virtual void visitWhileLoop(WhileLoop* that)=0;
	virtual void visitForLoop(ForLoop* that) =0;

	virtual void visitSwitch(Switch* that)=0;
	virtual void visitCase(Case* that) =0;

	virtual void visitConditional(Conditional* that)=0;
	virtual void visitIf(If* that) =0;
	virtual void visitExec(ExpressionStatement* that) =0;
	virtual void visitBreak(Break* that) =0;
	virtual void visitContinue(Continue* that)=0;
	virtual void visitReturn(Return* that)=0;
	virtual void visitApply(MethodInvocation* that) =0;
	virtual void visitNewClass(NewClass* that) =0;
	virtual void visitNewArray(NewArray* that)=0;
	virtual void visitParens(Parens* that) =0;
	virtual void visitAssign(Assign* that)=0;
	virtual void visitAssignop(AssignOp* that) =0;
	virtual void visitUnary(Unary* that) =0;
	virtual void visitBinary(Binary* that) =0;
	virtual void visitTypeCast(TypeCast* that) =0;
	virtual void visitTypeTest(InstanceOf* that)=0;
	virtual void visitIndexed(ArrayAccess* that)=0;
	virtual void visitSelect(FieldAccess* that)=0;
	virtual void visitIdent(Ident* that) =0;
	virtual void visitLiteral(Literal* that) =0;
	virtual void visitTypeIdent(PrimitiveTypeTree* that) =0;
	virtual void visitTypeArray(ArrayTypeTree* that)=0;
	virtual void visitModifiers(Modifiers* that) =0;
	virtual void visitTree(Tree* that)=0;
	 ~Visitor(){};
//	virtual void visitTypeApply(TypeApply that)=0;
	//	virtual void visitImport(Import that) {
	//		visitTree(that);
	//	}
	//	virtual void visitSynchronized(Synchronized that) {
	//		visitTree(that);
	//	}
	//	virtual void visitTry(Try that) {
	//		visitTree(that);
	//	}
	//	virtual void visitCatch(Catch that) {
	//		visitTree(that);
	//	}
	//	virtual void visitForeachLoop(EnhancedForLoop that) {
	//		visitTree(that);
	//	}
	//	virtual void visitLabelled(LabeledStatement that) {
	//		visitTree(that);
	//	}
//	virtual void visitThrow(Throw that) {
//		visitTree(that);
//	}
//	virtual void visitAssert(Assert that) {
//		visitTree(that);
//	}
//	virtual void visitTypeUnion(TypeUnion that) {
//		visitTree(that);
//	}
//	virtual void visitTypeParameter(TypeParameter that) {
//		visitTree(that);
//	}
//	virtual void visitWildcard(Wildcard that) {
//		visitTree(that);
//	}
//	virtual void visitTypeBoundKind(TypeBoundKind that) {
//		visitTree(that);
//	}
////	          void visitErroneous(Erroneous that)         { visitTree(that); }
////	          void visitLetExpr(LetExpr that)               { visitTree(that); }
};
#endif /* TREE_TREE_HPP_ */
