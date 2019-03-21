/*
 * Parser.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef PARSER_PARSER_HPP_
#define PARSER_PARSER_HPP_
#include "../util/includeGlobal.hpp"
#include "../tree/TreeMaker.hpp"
/**
 * 语法解析器  生成语法树
 */
class Parser {
public:

	Parser();
	Parser(Lexer* lexer,Log* log);
	CompilationUnit* parse(){return parseCompilationUnit();};
protected:
	//解析编译单元(整个源文件)
	CompilationUnit* parseCompilationUnit();
	ClassDecl* ClassDeclaration();

	void accept(int token);

	string ident();
	Expression* literal(string name,int pos);



	vector<Tree* > classBody(string className);
	vector<Tree* > classBodyDeclaration(string className);
	Modifiers* modifiersOpt();
	//形参
	vector<VariableDecl* > formalParameters();
	VariableDecl* formalParameter();

	MethodInvocation* arguments(Expression* t);
	vector<Expression* > arguments();
	Expression* argumentsOpt(Expression* t);

	//方法声明剩余部分  参数开始
	Tree* methodDeclaratorRest(int pos,Modifiers* mods,Expression* restype,string name,bool isVoid);
	//{ ... }
	Block* block(int pos,long long flgas);

	vector<Statement* > blockStatements();
	Statement* parseStatement();
	template<class T>
	vector<T> moreStatementExpressions(int pos,Expression* first,vector<T> stats);


	vector<Statement*> forInit();
	vector<ExpressionStatement*> forUpdate();
	vector<Case* > switchBlockStatementGroups();


	VariableDecl* variableDeclaratorId(Modifiers* mods,Expression* type);
	template<class T>
	vector<T> variableDeclarators(Modifiers* mods,Expression* type,vector<T> vdefs);
	template<class T>
	vector<T> variableDeclaratorsRest(int pos, Modifiers* mods,Expression* type,string name,vector<T > vdefs);
	VariableDecl* variableDeclaratorRest(int pos,Modifiers* mods,Expression* type,string name);
	Expression* variableInitializer();

	Expression* arrayInitializer(int pos,Expression* t);

	Expression* arrayCreatorRest(int pos,Expression* t);

	Expression* parseType();
	PrimitiveTypeTree* basicType();

	Expression* parExpression();

	Expression* parseExpression();
	/**
	 * 表达式部分
	 */
	Expression* term(int newmode);
	Expression* term();
	Expression* termRest(Expression* t);
	Expression* term1();
	Expression* term1Rest(Expression* t);
	Expression* term2();
	Expression* term2Rest(Expression* t,int minprec);
	Expression* term3();


	Expression* bracketsOpt(Expression* type);

	Expression* bracketsSuffix(Expression* t);

	ArrayTypeTree* bracketsOptCont(Expression* t,int pos);



	Expression* creator(int newpos);
	//修饰符
	Modifiers* optFinal(long long flags);
	//检查表达式语句
	Expression* checkExprStat(Expression* t);


	int typetag(int token);

	void skip(bool stopAtMemberDecl, bool stopAtIdentifier, bool stopAtStatement );

	static int optag(int token);
	int unoptag(int token);
	static int prec(int token);

	//错误定位
	void reportSyntaxError(int pos,string msg);
	void illegal();

	void setErrorEndPos(int errPos);
	virtual ~Parser();
private :
	Lexer* L;
	Log* log;
	KeyWords* keywords;

	int mode;
	int lastmode;
	int errorEndPos;

    static const int EXPR = 0x1;
    static const int TYPE = 0x2;
    static const int NOPARAMS = 0x4;
    static const int TYPEARG = 0x8;
    static const int DIAMOND = 0x10;
};

#endif /* PARSER_PARSER_HPP_ */
