/*
 * Parser.cpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#include "Parser.hpp"

Parser::Parser(Lexer* L, Log* log) :
		L(L), log(log) {
	this->keywords = L->getKeyWords();
	L->nextToken();
	mode = 0;
	lastmode = 0;
	errorEndPos = -1;
}

/**
 *CompilationUnit =  ClassDeclaration
 */
CompilationUnit* Parser::parseCompilationUnit() {
	if (util::debug)
		cout << endl << "==========Parser start:" << endl;

	int pos = L->getToken()->pos;
	vector<Tree*> defs;
	while (L->getToken()->id != Token::EOF_) {
		defs.push_back(ClassDeclaration());
	}
	return TreeMaker::TopLevel(pos, defs, L->getSource());
}

/**
 * ClassDeclaration= CLASS Ident ClassBody
 *
 */
ClassDecl* Parser::ClassDeclaration() {

	int pos = L->getToken()->pos;
	Modifiers* mods = modifiersOpt();
	accept(Token::CLASS);
	string class_name = this->ident();
	vector<Tree*> defs = classBody(class_name);
	return TreeMaker::makeClassDecl(pos, mods, class_name, defs);
}
//ClassBody     = "{" {ClassBodyDeclaration} "}"
vector<Tree*> Parser::classBody(string className) {
	accept(Token::LBRACE);
	if (L->getToken()->pos <= errorEndPos) {
		//错误恢复
		skip(true, false, false);
		if (L->getToken()->id == Token::LBRACE)
			L->nextToken();
	}

	vector<Tree*> defs;
	while (L->getToken()->id != Token::RBRACE
			&& L->getToken()->id != Token::EOF_) {

		util::appendList(defs, classBodyDeclaration(className));
		if (L->getToken()->pos <= errorEndPos) {
			//错误恢复
			skip(true, true, false);
			if (L->getToken()->id == Token::LBRACE)
				L->nextToken();
		}

	}

	accept(Token::RBRACE);
	return defs;
}

/**
 * ClassBodyDeclaration=
 “;”
 |	[STATIC] BLOCK
 |	ModifiersOpt
 ( Type Ident
 ( VariableDeclaratorsRest ";" | MethodDeclaratorRest )
 | VOID Ident MethodDeclaratorRest
 | 	Ident ConstructorDeclaratorRest )
 *
 */
vector<Tree*> Parser::classBodyDeclaration(string className) {
	if (L->getToken()->id == Token::SEMI) {
		L->nextToken();
		vector<Tree*> nullList;
		return nullList;
	} else {
		int pos = L->getToken()->pos;
		Modifiers* mods = modifiersOpt();
		//[静态]{构造块}
		if (L->getToken()->id == Token::LBRACE
				&& (mods->flags & Flags::StandardFlags & ~Flags::STATIC) == 0) {
			return util::ListOf((Tree*) block(pos, mods->flags));
		} else {
			//提前保存name,后面需要函数名称需要判断
			string name = L->getToken()->name;
			pos = L->getToken()->pos;
			Expression* type;
			bool isVoid = (L->getToken()->id == Token::VOID);
			if (isVoid) {
				type = TreeMaker::makeTypeIdent(pos, TypeTags::VOID);
				L->nextToken();
			} else {
				type = parseType();
			}

			//构造方法
			if (L->getToken()->id == Token::LPAREN
					&& type->getTag() == Tree::IDENT) {
				if (className.compare(name) != 0)
					reportSyntaxError(pos, "错误的方法声明");
				return util::ListOf(
						methodDeclaratorRest(pos, mods, 0, Name::init, true));
			} else {
				pos = L->getToken()->pos;
				name = ident();
				//一般方法
				if (L->getToken()->id == Token::LPAREN) {
					return util::ListOf(
							(Tree*) methodDeclaratorRest(pos, mods, type, name,
									isVoid));
				} else { //成员变量
					vector<Tree*> defs;
					defs = variableDeclaratorsRest(pos, mods, type, name, defs);
					accept(Token::SEMI);
					return defs;
				}
			}

		}
	}
}

/** MethodDeclaratorRest =
 *      FormalParameters  ( MethodBody)
 *  ConstructorDeclaratorRest =
 *      "(" FormalParameterListOpt ")" MethodBody
 */
Tree* Parser::methodDeclaratorRest(int pos, Modifiers* mods,
		Expression* restype, string name, bool isVoid) {
	vector<VariableDecl*> params = formalParameters();
	Block* body = NULL;
	if (L->getToken()->id == Token::LBRACE) {
		body = block(L->getToken()->pos, 0);
	} else {
		// test abstract method
		accept(Token::SEMI);
	}
	MethodDecl* method = TreeMaker::makeMethodDecl(pos, mods, restype, name,
			params, body);
	return method;
}

/**
 *  Block = "{" BlockStatements "}"
 */

Block* Parser::block(int pos, long long flags) {
	accept(Token::LBRACE);
	vector<Statement*> stats = blockStatements();
	Block* block = TreeMaker::makeBlock(pos, flags, stats);
	accept(Token::RBRACE);
	block->endPos = L->getToken()->pos;
	return block;
}
/*
 * !!!
 *
 * BlockStatements = ( BlockStatement )+
 *  BlockStatement  = LocalVariableDeclarationStatement
 *                   | [Ident ":"] Statement
 *  LocalVariableDeclarationStatement
 *                  = Type VariableDeclarators ";"
 */
vector<Statement*> Parser::blockStatements() {
	vector<Statement*> stats;
	int lastErrPos = -1;
	while (true) {
		int pos = L->getToken()->pos;
		switch (L->getToken()->id) {
		//直接返回的情况
		case Token::RBRACE:
		case Token::CASE:
		case Token::DEFAULT:
		case Token::EOF_:
			return stats;
		case Token::LBRACE:
		case Token::IF:
		case Token::FOR:
		case Token::WHILE:
		case Token::DO:
		case Token::SWITCH:
		case Token::RETURN:
		case Token::BREAK:
		case Token::CONTINUE:
		case Token::SEMI:
		case Token::ELSE:
			//暂不支持
//	    case Token::FINALLY: case Token::CATCH: case Token::TRY:case Token::THROW:case Token::SYNCHRONIZED:
			stats.push_back(parseStatement());
			break;
			//还可以支持其他情况:  内部类...
			//case Token::CLASS:

		default:
			string name = L->getToken()->name;
			Expression* t = term(TYPE | EXPR);
			if ((lastmode & TYPE) != 0
					&& L->getToken()->id == Token::IDENTIFIER) {
				//变量声明

				pos = L->getToken()->pos;
				Modifiers* mods = TreeMaker::Mods(-1, 0);
				vector<Statement*> vdefs;
				util::appendList(stats, variableDeclarators(mods, t, vdefs));
				accept(Token::SEMI);
			} else {
				//一条表达式语句
				stats.push_back(TreeMaker::makeExec(pos, checkExprStat(t)));
				accept(Token::SEMI);
			}

			break;
		}

		if (L->getToken()->pos == lastErrPos)
			return stats;
		if (L->getToken()->pos <= lastErrPos) {
			skip(true, true, true);
			lastErrPos = L->getToken()->pos;
		}

	}
	return stats;
}

Statement* Parser::parseStatement() {
	vector<Statement*> nulllist;
	int pos = L->getToken()->pos;
	switch (L->getToken()->id) {
	case Token::LBRACE:
		return block(L->getToken()->pos, 0);
		break;
	case Token::IF: {
		L->nextToken();
		Expression* cond = parExpression();
		Statement* thenpart = parseStatement();
		Statement* elsepart = NULL;
		if (L->getToken()->id == Token::ELSE) {
			L->nextToken();
			elsepart = parseStatement();
		}
		return TreeMaker::makerIf(pos, cond, thenpart, elsepart);
	}
	case Token::FOR: {
		L->nextToken();
		accept(Token::LPAREN);
		vector<Statement*> inits =
				L->getToken()->id == Token::SEMI ? nulllist : forInit();
		accept(Token::SEMI);
		Expression* cond =
				L->getToken()->id == Token::SEMI ? NULL : parseExpression();
		accept(Token::SEMI);
		vector<ExpressionStatement*> steps;
		if (L->getToken()->id != Token::LPAREN)
			steps = forUpdate();
		accept(Token::RPAREN);
		Statement* body = parseStatement();
		return TreeMaker::makeForLoop(pos, inits, cond, steps, body);
	}
	case Token::WHILE: {
		L->nextToken();
		Expression* cond = parExpression();
		Statement* body = parseStatement();
		return TreeMaker::makeWhileLoop(pos, cond, body);
	}
	case Token::DO: {
		L->nextToken();
		Statement* body = parseStatement();
		accept(Token::WHILE);
		Expression* cond = parExpression();
		accept(Token::SEMI);
		return TreeMaker::makeDoLoop(pos, body, cond);
	}
	case Token::SWITCH: {
		L->nextToken();
		Expression* selector = parExpression();
		accept(Token::LBRACE);
		vector<Case*> cases = switchBlockStatementGroups();
		accept(Token::RBRACE);
		return TreeMaker::makeSwitch(pos, selector, cases);
	}
	case Token::RETURN: {
		L->nextToken();
		Expression* result =
				L->getToken()->id == Token::SEMI ? NULL : parseExpression();
		accept(Token::SEMI);
		return TreeMaker::makeReturn(pos, result);
	}
	case Token::BREAK: {
		L->nextToken();
		string label = L->getToken()->id == Token::IDENTIFIER ? ident() : "";
		accept(Token::SEMI);
		return TreeMaker::makeBreak(pos, label);
	}
	case Token::CONTINUE: {
		L->nextToken();
		string label = L->getToken()->id == Token::IDENTIFIER ? ident() : "";
		accept(Token::SEMI);
		return TreeMaker::makeContinue(pos, label);
	}
	case Token::ELSE: {
		reportSyntaxError(pos, "没有前置 if 的 else ");
		L->nextToken();
		//暂时跳过这个错误
		return TreeMaker::makeSkip(pos);
	}
	case Token::SEMI: {
		L->nextToken();
		return TreeMaker::makeSkip(pos);
	}
	default:
		Expression* expr = parseExpression();
		ExpressionStatement * stat = TreeMaker::makeExec(pos, expr);
		accept(Token::SEMI);
		return stat;
	}
}

template<class T>
vector<T> Parser::moreStatementExpressions(int pos, Expression* first,
		vector<T> stats) {
	stats.push_back(TreeMaker::makeExec(pos, checkExprStat(first)));
	while (L->getToken()->pos == Token::COMMA) {
		L->nextToken();
		pos = L->getToken()->pos;
		Expression* t = parseExpression();
		stats.push_back(TreeMaker::makeExec(pos, checkExprStat(t)));
	}
	return stats;
}

/*
 * ForInit = StatementExpression MoreStatementExpressions
 |  { FINAL } Type VariableDeclarators
 */
vector<Statement*> Parser::forInit() {
	vector<Statement*> stats;
	int pos = L->getToken()->pos;
	if (L->getToken()->id == Token::FINAL) {
		return variableDeclarators(optFinal(0), parseType(), stats);
	} else {
		Expression* t = term(EXPR | TYPE);
		if ((lastmode & TYPE) != 0 && L->getToken()->id == Token::IDENTIFIER)
			return variableDeclarators(modifiersOpt(), t, stats);
		else
			return moreStatementExpressions(pos, t, stats);
	}

}
/**
 * ForUpdate = StatementExpression MoreStatementExpressions
 */
vector<ExpressionStatement*> Parser::forUpdate() {
	vector<ExpressionStatement*> stats;
	return moreStatementExpressions(L->getToken()->pos, parseExpression(),
			stats);
}

vector<Case*> Parser::switchBlockStatementGroups() {
	vector<Case*> cases;
	while (true) {
		int pos = L->getToken()->pos;
		switch (L->getToken()->id) {
		case Token::CASE: {

			L->nextToken();
			Expression* pat = parseExpression();
			accept(Token::COLON);
			vector<Statement*> stats = blockStatements();
			cases.push_back(TreeMaker::makeCase(pos, pat, stats));
			break;
		}
		case Token::DEFAULT: {

			L->nextToken();
			accept(Token::COLON);
			vector<Statement*> stats = blockStatements();
			cases.push_back(TreeMaker::makeCase(pos, NULL, stats));
			break;
		}
		case Token::RBRACE:
		case Token::EOF_:
			return cases;
		default:
			L->nextToken();			// 保证进程.
			reportSyntaxError(pos,
					Token::tokenStr(Token::CASE) + ","
							+ Token::tokenStr(Token::DEFAULT) + ","
							+ Token::tokenStr(Token::RBRACE));
		}
	}
}

/**
 * FormalParameters = "(" [ FormalParameterList ] ")"
 */
vector<VariableDecl*> Parser::formalParameters() {
	vector<VariableDecl*> params;
	VariableDecl* lastParam = 0;
	accept(Token::LPAREN);
	if (L->getToken()->id != Token::RPAREN) {
		params.push_back(lastParam = formalParameter());
		while ((lastParam->mods->flags & Flags::VARARGS) == 0
				&& L->getToken()->id == Token::COMMA) { //参数1,参数2...
			L->nextToken();
			params.push_back(lastParam = formalParameter());
		}
	}
	accept(Token::RPAREN);
	return params;
}
VariableDecl* Parser::formalParameter() {
	Modifiers* mods = optFinal(Flags::PARAMETER);
	Expression* type = parseType();
	return variableDeclaratorId(mods, type);

}

MethodInvocation* Parser::arguments(Expression* t) {
	int pos = L->getToken()->pos;
	vector<Expression*> args = arguments();
	return TreeMaker::makeApply(pos, t, args);
}
vector<Expression*> Parser::arguments() {
	vector<Expression*> args;
	if (L->getToken()->id == Token::LPAREN) {
		L->nextToken();
		if (L->getToken()->id != Token::RPAREN) {
			args.push_back(parseExpression());
			while (L->getToken()->id == Token::COMMA) {
				L->nextToken();
				args.push_back(parseExpression());
			}
		}
		accept(Token::RPAREN);

	} else {
		reportSyntaxError(L->getToken()->pos,
				"需要 " + Token::tokenStr(Token::LPAREN));
	}
	return args;

}
Expression* Parser::argumentsOpt(Expression* t) {

	if ((mode & EXPR) != 0 && L->getToken()->id == Token::LPAREN) {
		mode = EXPR;
		return arguments(t);
	} else {
		return t;
	}
}

/**
 * VariableDeclaratorId = Ident BracketsOpt
 */
VariableDecl* Parser::variableDeclaratorId(Modifiers* mods, Expression* type) {
	int pos = L->getToken()->pos;
	string name = ident();
	type = bracketsOpt(type);
	return TreeMaker::makeVarDef(pos, mods, name, type, NULL);

}
template<class T>
vector<T> Parser::variableDeclarators(Modifiers* mods, Expression* type,
		vector<T> vdefs) {
	int pos = L->getToken()->pos;
	return variableDeclaratorsRest(pos, mods, type, ident(), vdefs);
	/**
	 * 原来使用,pos是ident下个token的位置,参数入栈：从右向左
	 */
//	return variableDeclaratorsRest(L->getToken()->pos, mods, type, ident(),
//			vdefs);
}

/**
 *VariableDeclaratorsRest = VariableDeclaratorRest { "," VariableDeclarator }
 */
template<class T>
vector<T> Parser::variableDeclaratorsRest(int pos, Modifiers* mods,
		Expression* type, string name, vector<T> vdefs) {
	vdefs.push_back(variableDeclaratorRest(pos, mods, type, name));
	while (L->getToken()->id == Token::COMMA) {
		L->nextToken();
		vdefs.push_back(
				variableDeclaratorRest(L->getToken()->pos, mods, type,
						ident()));
	}
	return vdefs;
}

/**
 * VariableDeclaratorRest = BracketsOpt ["=" VariableInitializer]
 */

VariableDecl* Parser::variableDeclaratorRest(int pos, Modifiers* mods,
		Expression* type, string name) {
	type = bracketsOpt(type);
	Expression* init = NULL;
	if (L->getToken()->id == Token::EQ) {
		L->nextToken();
		init = variableInitializer();

	}
	return TreeMaker::makeVarDef(pos, mods, name, type, init);

}
/**
 * VariableInitializer = ArrayInitializer | Expression
 */
Expression* Parser::variableInitializer() {
	return L->getToken()->id == Token::LBRACE ?
			arrayInitializer(L->getToken()->pos, NULL) : parseExpression();
}

/**
 * ArrayInitializer = "{" [VariableInitializer {"," VariableInitializer}] [","] "}"
 *
 */
Expression* Parser::arrayInitializer(int pos, Expression* t) {
	accept(Token::LBRACE);
	vector<Expression*> defs;
	if (L->getToken()->id == Token::RBRACE) {
		defs.push_back(variableInitializer());
		while (L->getToken()->id == Token::COMMA) {
			L->nextToken();
			if (L->getToken()->id == Token::RBRACE)
				break;
			defs.push_back(variableInitializer());
		}
	}
	accept(Token::RBRACKET);
	vector<Expression*> nullList;
	return TreeMaker::makeNewArray(pos, t, nullList, defs);

}

Expression* Parser::arrayCreatorRest(int pos, Expression* elemtype) {
	accept(Token::LBRACKET);
	vector<Expression*> nulllist;
	//int a[][] = new int[]{0,123}
	if (L->getToken()->id == Token::RBRACKET) {
		accept(Token::RBRACKET);
		elemtype = bracketsOpt(elemtype);
		if (L->getToken()->id == Token::LBRACE) {
			return arrayInitializer(pos, elemtype);
		} else {

			Expression* t = TreeMaker::makeNewArray(pos, elemtype, nulllist,
					nulllist);
			reportSyntaxError(pos, "缺少数组维度");
			return t;
		}
	} else {  // int a[][] = new int[5][6];
		vector<Expression*> dims;
		dims.push_back(parseExpression());
		accept(Token::RBRACKET);
		while (L->getToken()->id == Token::LBRACKET) {  //[
			int pos = L->getToken()->pos;
			L->nextToken();
			if (L->getToken()->id == Token::RBRACKET) {  //]  允许new int[5][][]
				elemtype = bracketsOptCont(elemtype, pos);
			} else {
				dims.push_back(parseExpression());
				accept(Token::RBRACKET);
			}
		}
		return TreeMaker::makeNewArray(pos, elemtype, dims, nulllist);
	}

}

PrimitiveTypeTree* Parser::basicType() {
	PrimitiveTypeTree* t = TreeMaker::makeTypeIdent(L->getToken()->pos,
			typetag(L->getToken()->id));
	L->nextToken();
	return t;
}

/**
 *
 */
Expression* Parser::parseType() {
	return term(TYPE);
}

Expression* Parser::parExpression() {
	accept(Token::LPAREN);
	Expression* t = parseExpression();
	accept(Token::RPAREN);
	return t;
}

/**
 *
 */
Expression* Parser::parseExpression() {
	return term(EXPR);
}
//在递归调用过程中可能会改变当前mode
Expression* Parser::term(int newmode) {

	int premode = mode;
	mode = newmode;
	Expression* t = term();
	lastmode = mode;
	mode = premode;
	return t;
}
/**
 *  Expression = Expression1 [ExpressionRest]
 *  ExpressionRest = [AssignmentOperator Expression1]
 *  AssignmentOperator = "=" | "+=" | "-=" | "*=" | "/=" |
 *                       "&=" | "|=" | "^=" |
 *                       "%=" | "<<=" | ">>=" | ">>>="
 *  Type = Type1
 *  TypeNoParams = TypeNoParams1
 *  StatementExpression = Expression
 *  ConstantExpression = Expression
 *  一条语句 int a = a+3*b; 表达式: x++; fun(1,3);
 *  一个（不是因子）项（term）是一个可能被高优先级的运算符x和/分开，但不能被低优先级运算符分开的表达式。
 *
 *
 *
 *
 */
Expression* Parser::term() {

	Expression* t = term1();
	if ((mode & EXPR) != 0 && L->getToken()->id == Token::EQ
			|| L->getToken()->id >= Token::PLUSEQ
					&& L->getToken()->id <= Token::GTGTGTEQ)
		return termRest(t);
	else
		return t;

}
Expression* Parser::termRest(Expression* t) {
	switch (L->getToken()->id) {
	case Token::EQ: {
		int pos = L->getToken()->pos;
		L->nextToken();
		mode = EXPR;
		Expression* t1 = term();
		return TreeMaker::makeAssign(pos, t, t1);
	}
	case Token::PLUSEQ:
	case Token::SUBEQ:
	case Token::STAREQ:
	case Token::SLASHEQ:
	case Token::PERCENTEQ:
	case Token::AMPEQ:
	case Token::BAREQ:
	case Token::CARETEQ:
	case Token::LTLTEQ:
	case Token::GTGTEQ:
	case Token::GTGTGTEQ: {
		int pos = L->getToken()->pos;
		Token* tt = L->getToken();
		L->nextToken();
		mode = EXPR;
		Expression* t1 = term();
		return TreeMaker::makeAssignOp(pos, optag(tt->id), t, t1);
	}

	default:
		return t;
	}

}
/** Expression1   = Expression2 [Expression1Rest]
 *
 */

Expression* Parser::term1() {
	Expression* t = term2();
	if ((mode & EXPR) != 0 && L->getToken()->id == Token::QUES)
		return term1Rest(t);
	else
		return t;

}

/** Expression1Rest = ["?" Expression ":" Expression1]
 *    三元表达式
 */
Expression* Parser::term1Rest(Expression* t) {
	if (L->getToken()->id == Token::QUES) {
		int pos = L->getToken()->pos;
		L->nextToken();
		Expression* t1 = term();
		accept(Token::COLON);
		Expression* t2 = term();
		return TreeMaker::makeConditional(pos, t, t1, t2);
	} else
		return t;

}

/**
 * Expression2   = Expression3 [Expression2Rest]
 */
Expression* Parser::term2() {
	Expression* t = term3();
	if ((mode & EXPR) != 0 && prec(L->getToken()->id) >= TreeInfo::orPrec) {
		mode = EXPR;
		return term2Rest(t, TreeInfo::orPrec);
	}
	return t;
}

/*
 * Expression2Rest = {infixop Expression3}
 *                  | Expression3 instanceof Type
 *  infixop         = "||"
 *                  | "&&"
 *                  | "|"
 *                  | "^"
 *                  | "&"
 *                  | "==" | "!="
 *                  | "<" | ">" | "<=" | ">="
 *                  | "<<" | ">>" | ">>>"
 *                  | "+" | "-"
 *                  | "*" | "/" | "%"
 *   二元表达式
 *   x+y*z-((x+y)/x -x)
 */
Expression* Parser::term2Rest(Expression* t, int minprec) {
	vector<Expression*> odStack; //值栈
	vector<int> opStack; //操作数栈
	vector<int> posStack; // pos栈
	//20个子表达式 够用了
	odStack.resize(20);
	opStack.resize(20);
	posStack.resize(20);
	int top = 0;
	odStack[0] = t;
	//int startPos = L->getToken()->pos;
	//第一个操作符优先级 最低，退出解析
	int topOp = Token::ERROR;
	int topOpPos = -1;
	while (prec(L->getToken()->id) >= minprec) {
		opStack[top] = topOp;
		posStack[top] = topOpPos;
		topOp = L->getToken()->id;
		topOpPos = L->getToken()->pos;
		top++;
		L->nextToken();
		odStack[top] = term3();
		//栈顶操作符优先级大于当前操作符优先级  ，将前两个数(top,top-1)合并，放入top-1
		while (top > 0 && prec(topOp) >= prec(L->getToken()->id)) {
			odStack[top - 1] = TreeMaker::makeBinary(topOpPos, optag(topOp),
					odStack[top - 1], odStack[top]);
			top--;
			topOp = opStack[top];
			topOpPos = posStack[top];
		}
	}
	t = odStack[0];
	return t;

}

/** Expression3    = PrefixOp Expression3
 *                 | "(" Expr | TypeNoParams ")" Expression3
 *                 | Primary {Selector} {PostfixOp}
 *  Primary        = "(" Expression ")"
 *                 | Literal
 *                 | [TypeArguments] THIS [Arguments]
 *                 | [TypeArguments] SUPER SuperSuffix
 *                 | NEW [TypeArguments] Creator
 *                 | Ident { "." Ident }
 *                   [ "[" ( "]" BracketsOpt "." CLASS | Expression "]" )
 *                   | Arguments
 *                   | "." ( CLASS | THIS | [TypeArguments] SUPER Arguments | NEW [TypeArguments] InnerCreator )
 *                   ]
 *                 | BasicType BracketsOpt "." CLASS
 *  PrefixOp       = "++" | "--" | "!" | "~" | "+" | "-"
 *  PostfixOp      = "++" | "--"
 *  Type3          = Ident { "." Ident } [TypeArguments] {TypeSelector} BracketsOpt
 *                 | BasicType
 *  TypeNoParams3  = Ident { "." Ident } BracketsOpt
 *  Selector       = "." [TypeArguments] Ident [Arguments]
 *                 | "." THIS
 *                 | "." [TypeArguments] SUPER SuperSuffix
 *                 | "." NEW [TypeArguments] InnerCreator
 *                 | "[" Expression "]"
 *  TypeSelector   = "." Ident [TypeArguments]
 *  SuperSuffix    = Arguments | "." Ident [Arguments]
 *
 *  不能被操作符分隔的最小子表达式
 */

Expression* Parser::term3() {
	int pos = L->getToken()->pos;
	Expression* t = NULL;
	bool loop = true;
	switch (L->getToken()->id) {
	//单目操作符  ++ -- ！ ~ + -
	case Token::PLUSPLUS:
	case Token::SUBSUB:
	case Token::BANG:
	case Token::TILDE:
	case Token::PLUS:
	case Token::SUB:
		if ((mode & EXPR) != 0) {
			int token = L->getToken()->id;
			L->nextToken();
			mode = EXPR;
			// -整数 直接处理， -浮点数 由一元表达式处理
			if (token == Token::SUB
					&& (L->getToken()->id == Token::INTLITERAL
							|| L->getToken()->id == Token::LONGLITERAL)) {
				mode = EXPR;
				t = literal(Name::hyphen, pos);
			} else {
				t = term3();
				return TreeMaker::makeUnary(pos, unoptag(token), t);
			}

		} else {
			illegal();
			return t;
		}
		break;
		//括号表达式
	case Token::LPAREN:
		if ((mode & EXPR) != 0) {
			L->nextToken();
			mode = EXPR | TYPE | NOPARAMS;
			//先获取下一个term3
			t = term3();
			//再分别判断 term-rest表达式
			t = termRest(term1Rest(term2Rest(t, TreeInfo::orPrec)));
			accept(Token::RPAREN);
			lastmode = mode;
			mode = EXPR;
			//强制类型转换
			if ((lastmode & EXPR) == 0) {
				Expression* t1 = term3();
				return TreeMaker::makeTypeCast(pos, t, t1);
			} else if ((lastmode & TYPE) != 0) {
				switch (L->getToken()->id) {
				case Token::BANG:
				case Token::TILDE:
				case Token::LPAREN:
				case Token::THIS:
				case Token::SUPER:
				case Token::INTLITERAL:
				case Token::LONGLITERAL:
				case Token::FLOATLITERAL:
				case Token::DOUBLELITERAL:
				case Token::CHARLITERAL:
				case Token::STRINGLITERAL:
				case Token::TRUE:
				case Token::FALSE:
				case Token::NULL_:
				case Token::NEW:
				case Token::IDENTIFIER:
				case Token::ASSERT:
				case Token::ENUM:
				case Token::BYTE:
				case Token::SHORT:
				case Token::CHAR:
				case Token::INT:
				case Token::LONG:
				case Token::FLOAT:
				case Token::DOUBLE:
				case Token::BOOLEAN:
				case Token::VOID:
					Expression* t1 = term3();
					return TreeMaker::makeTypeCast(pos, t, t1);
				}
			}
		}
		t = TreeMaker::makeParens(pos, t);
		break;
	case Token::THIS:
		if ((mode & EXPR) != 0) {
			mode = EXPR;
			t = TreeMaker::makeIdent(pos, Name::_this);
			L->nextToken();
			//this(...)  调用构造方法
			t = argumentsOpt(t);
		} else {
			illegal();
			return t;
		}
		break;
		//暂不支持
		//case Token::SUPER
		//字面常量
	case Token::INTLITERAL:
	case Token::LONGLITERAL:
	case Token::FLOATLITERAL:
	case Token::DOUBLELITERAL:
	case Token::CHARLITERAL:
	case Token::STRINGLITERAL:
	case Token::TRUE:
	case Token::FALSE:
	case Token::NULL_:
		if ((mode & EXPR) != 0) {
			mode = EXPR;
			t = literal("", L->getToken()->pos);
		} else {
			illegal();
			return t;
		}
		break;
		//new 构造
	case Token::NEW:
		if ((mode & EXPR) != 0) {
			mode = EXPR;
			L->nextToken();
			t = creator(pos);
		} else {
			illegal();
			return t;
		}
		break;
		//以标识符开始的情况
	case Token::IDENTIFIER:
		t = TreeMaker::makeIdent(pos, ident());
		loop = true;
		while (loop) {
			pos = L->getToken()->pos;
			switch (L->getToken()->id) {
			//数组声明   int a[][]
			case Token::LBRACKET:
				L->nextToken();
				if (L->getToken()->id == Token::RBRACKET) {
					L->nextToken();
					t = TreeMaker::makeArray(pos, t);
					// a[].CLASS情况   ---暂不支持
				} else {	//数组访问情况 a[0]
					if ((mode & EXPR) != 0) {
						mode = EXPR;
						Expression* t1 = term();
						t = TreeMaker::makeIndexed(pos, t, t1);
					}
					accept(Token::RBRACKET);
				}
				loop = false;
				break;
				//方法调用   ident(args)
			case Token::LPAREN:
				if ((mode & EXPR) != 0) {
					mode = EXPR;
					t = arguments(t);
				}
				loop = false;
				break;
				//ident.{CLASS|SUPER|THIS|NEW} 无需继续循环
				//ident.ident. ...需要继续循环(IDENTIFIER)，  直到找到符合终结语法
			case Token::DOT:
				L->nextToken();
				if ((mode & EXPR) != 0) {
					switch (L->getToken()->id) {
					case Token::THIS:
						mode = EXPR;
						t = TreeMaker::makeSelect(pos, t, Name::_this);
						L->nextToken();
						loop = false;
						break;
					case Token::SUPER:
						mode = EXPR;
						t = TreeMaker::makeSelect(pos, t, Name::super);
						L->nextToken();
						loop = false;
						break;
					case Token::CLASS:
						mode = EXPR;
						t = TreeMaker::makeSelect(pos, t, Name::_class);
						L->nextToken();
						loop = false;
						break;
					default:
						break;
					}
				}
				if (!loop)
					break;
				t = TreeMaker::makeSelect(pos, t, ident());
				break;
			default:
				loop = false;
				break;
			}
		}
		break;
	case Token::BYTE:
	case Token::SHORT:
	case Token::CHAR:
	case Token::INT:
	case Token::LONG:
	case Token::FLOAT:
	case Token::DOUBLE:
	case Token::BOOLEAN:
	case Token::STRING:				//string类型
		t = bracketsSuffix(bracketsOpt(basicType()));
		break;
	default:
		illegal();
		return t;
	}

	//完成剩余部分  主要是方法调用后语法部分
	//fun()[1] fun().fun2()...
	while (true) {
		int pos2 = L->getToken()->pos;
		if (L->getToken()->id == Token::LBRACKET) {
			L->nextToken();
			if ((mode & TYPE) != 0) {
				int oldmode = mode;
				mode = TYPE;
				if (L->getToken()->id == Token::RBRACKET) {
					L->nextToken();
					t = bracketsOpt(t);
					t = TreeMaker::makeArray(pos2, t);
					//Type 声明，找到直接返回
					return t;
				}
				mode = oldmode;
			}
			if ((mode & EXPR) != 0) {
				mode = EXPR;
				Expression* t1 = term();
				t = TreeMaker::makeIndexed(pos2, t, t1);
				//表达式需要循环下去
			}
			accept(Token::RBRACKET);
		} else if (L->getToken()->id == Token::DOT) {
			L->nextToken();
			if (L->getToken()->id == Token::SUPER && (mode & EXPR) != 0) {
				mode = EXPR;
				t = TreeMaker::makeSelect(pos2, t, Name::super);
				L->nextToken();
				t = argumentsOpt(t);
			} else {
				t = TreeMaker::makeSelect(pos2, t, ident());
				t = argumentsOpt(t);
			}
		} else {
			break;
		}

	}
	while ((L->getToken()->id == Token::PLUSPLUS
			|| L->getToken()->id == Token::SUBSUB) && (mode & EXPR) != 0) {
		mode = EXPR;
		int tag = 0;
		if (L->getToken()->id == Token::PLUSPLUS) {
			tag = Tree::POSTINC;
		} else {
			tag = Tree::POSTDEC;
		}
		t = TreeMaker::makeUnary(L->getToken()->pos, tag, t);
		L->nextToken();
	}

	return t;

}

/** BracketsOpt = {"[" "]"}
 */
Expression* Parser::bracketsOpt(Expression* t) {
	if (L->getToken()->id == Token::LBRACKET) {
		int pos = L->getToken()->pos;
		L->nextToken();
		t = bracketsOptCont(t, pos);
	}
	return t;
}
/**
 * BracketsSuffixExpr = "." CLASS
 */

Expression* Parser::bracketsSuffix(Expression* t) {
	if ((mode & EXPR) != 0 && L->getToken()->id == Token::DOT) {
		mode = EXPR;
		int pos = L->getToken()->pos;
		L->nextToken();
		accept(Token::CLASS);
		if (L->getToken()->pos == errorEndPos) {
			string name = NULL;
			if (L->getToken()->id == Token::IDENTIFIER) {
				name = L->getToken()->name;
				L->nextToken();
			} else {
				name = Name::error;
			}
			t = TreeMaker::makeSelect(pos, t, name);
		} else {
			t = TreeMaker::makeSelect(pos, t, Name::_class);
		}
	} else if ((mode & TYPE) != 0) {
		mode = TYPE;
	} else {
		reportSyntaxError(L->getToken()->pos, "需要  .class");
	}
	return t;
}

/**
 * 多维数组
 */
ArrayTypeTree* Parser::bracketsOptCont(Expression* t, int pos) {
	accept(Token::RBRACKET);
	t = bracketsOpt(t);
	return TreeMaker::makeArray(pos, t);

}
/**
 * /** Creator = Qualident [TypeArguments] ( ArrayCreatorRest | ClassCreatorRest )
 */

Expression* Parser::creator(int newpos) {
	//基本类型数组情况   new int[5][5]
	switch (L->getToken()->id) {
	case Token::BYTE:
	case Token::SHORT:
	case Token::CHAR:
	case Token::INT:
	case Token::LONG:
	case Token::FLOAT:
	case Token::DOUBLE:
	case Token::BOOLEAN:
		return arrayCreatorRest(newpos, basicType());
	default:
		break;
	}
	/**
	 * !!!!
	 */
	//类 类型  new A(args)  , new  A[5][5]...
	//先只支持 直接调用构造函数  ，不支持内部类(new A.B)
	Ident* t = TreeMaker::makeIdent(L->getToken()->pos, ident());
	//类数组
	if (L->getToken()->id == Token::LBRACKET) {
		Expression* e = arrayCreatorRest(newpos, t);
		return e;

	} else if (L->getToken()->id == Token::LPAREN) {
		vector<Expression*> args = arguments();

		return TreeMaker::makeNewClass(newpos, args, t);
	} else {
		reportSyntaxError(newpos, "new 语法错误");
		return NULL;
	}

}

Modifiers* Parser::optFinal(long long flags) {
	Modifiers* mods = modifiersOpt();
	mods->flags |= flags;
	return mods;
}

Expression* Parser::checkExprStat(Expression* t) {

	switch (t->getTag()) {
	case Tree::PREINC:
	case Tree::PREDEC:
	case Tree::POSTINC:
	case Tree::POSTDEC:
	case Tree::ASSIGN:
	case Tree::BITOR_ASG:
	case Tree::BITXOR_ASG:
	case Tree::BITAND_ASG:
	case Tree::SL_ASG:
	case Tree::SR_ASG:
	case Tree::USR_ASG:
	case Tree::PLUS_ASG:
	case Tree::MINUS_ASG:
	case Tree::MUL_ASG:
	case Tree::DIV_ASG:
	case Tree::MOD_ASG:
	case Tree::APPLY:
	case Tree::NEWCLASS:
	case Tree::ERRONEOUS:
		return t;
	default:
		reportSyntaxError(t->pos, "无法识别的语句");
		return t;
	}
}

Modifiers* Parser::modifiersOpt() {
	long long flags = 0;
	int pos = L->getToken()->pos;
	bool t = false;
	while (true) {
		long long flag = 0;
		switch (L->getToken()->id) {
		case Token::PRIVATE:
			flag = Flags::PRIVATE;
			break;
		case Token::PROTECTED:
			flag = Flags::PROTECTED;
			break;
		case Token::PUBLIC:
			flag = Flags::PUBLIC;
			break;
		case Token::STATIC:
			flag = Flags::STATIC;
			break;
//		case Token::TRANSIENT:
//			flag = Flags::TRANSIENT;
			break;
		case Token::FINAL:
			flag = Flags::FINAL;
			break;
//		case ABSTRACT:
//			flag = Flags.ABSTRACT;
//			break;
//		case NATIVE:
//			flag = Flags.NATIVE;
//			break;
//		case Token::VOLATILE:
//			flag = Flags.VOLATILE;
//			break;
//		case Token::SYNCHRONIZED:
//			flag = Flags::SYNCHRONIZED;
//			break;
//		case Token::STRICTFP:
//			flag = Flags::STRICTFP;
			break;
		case Token::ERROR:
			flag = 0;
			L->nextToken();
			break;
		default:
			t = true;
		}
		if (t)
			break;
		if ((flags & flag) != 0)
			reportSyntaxError(L->getToken()->pos, "重复的修饰符");
		L->nextToken();
		flags |= flag;
	}
	return TreeMaker::Mods(pos, flags);

}

//Ident = IDENTIFIER
string Parser::ident() {
	if (L->getToken()->id == Token::IDENTIFIER) {
		string name = L->getToken()->name;
		L->nextToken();
		return name;
	} else {
		accept(Token::IDENTIFIER);
		return "error";
	}

}
/**
 * !!!
 * 字面常量  没有通用基类，没有想到比较好的办法
 * 先粗糙解决。
 */
Expression* Parser::literal(string prefix, int pos) {

	Expression* t = NULL;
	switch (L->getToken()->id) {
	case Token::INTLITERAL:
		t = TreeMaker::makeLiteral(pos, TypeTags::INT,
				prefix + L->getToken()->name);
		break;
	case Token::LONGLITERAL:
		t = TreeMaker::makeLiteral(pos, TypeTags::LONG,
				prefix + L->getToken()->name);
		break;
	case Token::DOUBLELITERAL:
		t = TreeMaker::makeLiteral(pos, TypeTags::DOUBLE, L->getToken()->name);
		break;
	case Token::FLOATLITERAL:
		t = TreeMaker::makeLiteral(pos, TypeTags::FLOAT, L->getToken()->name);
		break;
	case Token::CHARLITERAL:
		t = TreeMaker::makeLiteral(pos, TypeTags::CHAR, L->getToken()->name);
		break;
	case Token::STRINGLITERAL:
//		t = TreeMaker::makeLiteral(pos, TypeTags::CLASS, L->getToken()->name);
		t = TreeMaker::makeLiteral(pos, TypeTags::STRING, L->getToken()->name);
		break;
	case Token::TRUE:
	case Token::FALSE:
		t = TreeMaker::makeLiteral(pos, TypeTags::BOOLEAN,
				L->getToken()->name == "true" ? "1" : "0");
		break;
	case Token::NULL_:
		t = TreeMaker::makeLiteral(pos, TypeTags::BOT, L->getToken()->name);
		break;
	default:
		reportSyntaxError(pos, "解析语法(literal)异常");
	}
	L->nextToken();
	return t;
}

void Parser::accept(int token) {
	if (L->getToken()->id == token) {
		L->nextToken();
	} else {
		reportSyntaxError(L->getToken()->pos, "需要  " + Token::tokenStr(token));
	}

}
int Parser::typetag(int token) {
	switch (token) {
	case Token::BYTE:
		return TypeTags::BYTE;
	case Token::CHAR:
		return TypeTags::CHAR;
	case Token::SHORT:
		return TypeTags::SHORT;
	case Token::INT:
		return TypeTags::INT;
	case Token::LONG:
		return TypeTags::LONG;
	case Token::FLOAT:
		return TypeTags::FLOAT;
	case Token::DOUBLE:
		return TypeTags::DOUBLE;
	case Token::BOOLEAN:
		return TypeTags::BOOLEAN;
	case Token::STRING:
		return TypeTags::STRING;
	default:
		return -1;
	}

}

//跳过一些词元  用来错误恢复
void Parser::skip(bool stopAtMemberDecl, bool stopAtIdentifier,
		bool stopAtStatement) {

	while (true) {
		switch (L->getToken()->id) {
		case Token::SEMI:
			L->nextToken();
			return;
		case Token::PUBLIC:
		case Token::FINAL:
		case Token::ABSTRACT:
		case Token::MONKEYS_AT:
		case Token::EOF_:
		case Token::CLASS:
		case Token::INTERFACE:
		case Token::ENUM:
			return;
		case Token::IMPORT:
		case Token::LBRACE:
		case Token::RBRACE:
		case Token::PRIVATE:
		case Token::PROTECTED:
		case Token::STATIC:
		case Token::TRANSIENT:
		case Token::NATIVE:
		case Token::VOLATILE:
		case Token::SYNCHRONIZED:
		case Token::STRICTFP:
		case Token::LT:
		case Token::BYTE:
		case Token::SHORT:
		case Token::CHAR:
		case Token::INT:
		case Token::LONG:
		case Token::FLOAT:
		case Token::DOUBLE:
		case Token::BOOLEAN:
		case Token::VOID:
			if (stopAtMemberDecl)
				return;
			break;
		case Token::IDENTIFIER:
			if (stopAtIdentifier)
				return;
			break;
		case Token::CASE:
		case Token::DEFAULT:
		case Token::IF:
		case Token::FOR:
		case Token::WHILE:
		case Token::DO:
		case Token::TRY:
		case Token::SWITCH:
		case Token::RETURN:
		case Token::THROW:
		case Token::BREAK:
		case Token::CONTINUE:
		case Token::ELSE:
		case Token::FINALLY:
		case Token::CATCH:
			if (stopAtStatement)
				return;
			break;
		}
		L->nextToken();
	}

}
int Parser::optag(int token) {
	switch (token) {
	case Token::BARBAR:
		return Tree::OR;
	case Token::AMPAMP:
		return Tree::AND;
	case Token::BAR:
		return Tree::BITOR;
	case Token::BAREQ:
		return Tree::BITOR_ASG;
	case Token::CARET:
		return Tree::BITXOR;
	case Token::CARETEQ:
		return Tree::BITXOR_ASG;
	case Token::AMP:
		return Tree::BITAND;
	case Token::AMPEQ:
		return Tree::BITAND_ASG;
	case Token::EQEQ:
		return Tree::EQ;
	case Token::BANGEQ:
		return Tree::NE;
	case Token::LT:
		return Tree::LT;
	case Token::GT:
		return Tree::GT;
	case Token::LTEQ:
		return Tree::LE;
	case Token::GTEQ:
		return Tree::GE;
	case Token::LTLT:
		return Tree::SL;
	case Token::LTLTEQ:
		return Tree::SL_ASG;
	case Token::GTGT:
		return Tree::SR;
	case Token::GTGTEQ:
		return Tree::SR_ASG;
	case Token::GTGTGT:
		return Tree::USR;
	case Token::GTGTGTEQ:
		return Tree::USR_ASG;
	case Token::PLUS:
		return Tree::PLUS;
	case Token::PLUSEQ:
		return Tree::PLUS_ASG;
	case Token::SUB:
		return Tree::MINUS;
	case Token::SUBEQ:
		return Tree::MINUS_ASG;
	case Token::STAR:
		return Tree::MUL;
	case Token::STAREQ:
		return Tree::MUL_ASG;
	case Token::SLASH:
		return Tree::DIV;
	case Token::SLASHEQ:
		return Tree::DIV_ASG;
	case Token::PERCENT:
		return Tree::MOD;
	case Token::PERCENTEQ:
		return Tree::MOD_ASG;
	case Token::INSTANCEOF:
		return Tree::TYPETEST;
	default:
		return -1;
	}

}

int Parser::unoptag(int token) {
	switch (token) {
	case Token::PLUS:
		return Tree::POS;
	case Token::SUB:
		return Tree::NEG;
	case Token::BANG:
		return Tree::NOT;
	case Token::TILDE:
		return Tree::COMPL;
	case Token::PLUSPLUS:
		return Tree::PREINC;
	case Token::SUBSUB:
		return Tree::PREDEC;
	default:
		return -1;
	}
}

int Parser::prec(int token) {
	int op = optag(token);
	return (op >= 0) ? TreeInfo::opPrec(op) : -1;

}

void Parser::reportSyntaxError(int pos, string msg) {
	setErrorEndPos(pos);
	log->report(pos, Log::ERROR_Parser, msg);
}

void Parser::illegal() {
	if ((mode & EXPR) != 0)
		reportSyntaxError(L->getToken()->pos, "错误的表达式的开始");
	else
		reportSyntaxError(L->getToken()->pos, "错误的类型的开始");
}

void Parser::setErrorEndPos(int errPos) {
	if (errPos > errorEndPos)
		errorEndPos = errPos;
}

Parser::~Parser() {
}

