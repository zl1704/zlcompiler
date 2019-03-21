/*
 * Token.hpp
 *
 *  Created on: 2017年12月2日
 *      Author: ZL
 */

#ifndef PARSER_TOKEN_HPP_
#define PARSER_TOKEN_HPP_

#include "../util/global.hpp"

/**
 * 词法单元
 */
class Token {
public:
	static const int EOF_ = 1;
	static const int ERROR = EOF_ + 1;
	static const int IDENTIFIER = ERROR + 1;
	static const int ABSTRACT = IDENTIFIER + 1;  //("abstract"),
	static const int ASSERT = ABSTRACT + 1;  //("assert"),
	static const int BOOLEAN = ASSERT + 1;  //("boolean"),
	static const int BREAK = BOOLEAN + 1;  //("break"),
	static const int BYTE = BREAK + 1;  // ("byte"),
	static const int CASE = BYTE + 1;  //("case"),
	static const int CATCH = CASE + 1;  //("catch"),
	static const int CHAR = CATCH + 1;  //("char"),
	static const int CLASS = CHAR + 1;  //("class"),
	static const int CONST = CLASS + 1;  //("const"),
	static const int CONTINUE = CONST + 1;  //("continue"),
	static const int DEFAULT = CONTINUE + 1;  //("default"),
	static const int DO = DEFAULT + 1;  //("do"),
	static const int DOUBLE = DO + 1;  //("double"),
	static const int ELSE = DOUBLE + 1;  //("else"),
	static const int ENUM = ELSE + 1;  //("enum"),
	static const int EXTENDS = ENUM + 1;  //("extends"),
	static const int FINAL = EXTENDS + 1;  //("final"),
	static const int FINALLY = FINAL + 1;  //("finally"),
	static const int FLOAT = FINALLY + 1;  //("float"),
	static const int FOR = FLOAT + 1;  //("for"),
	static const int GOTO = FOR + 1;  //("goto"),
	static const int IF = GOTO + 1;  //("if"),
	static const int IMPLEMENTS = IF + 1;  //("implements"),
	static const int IMPORT = IMPLEMENTS + 1;  //("import"),
	static const int INSTANCEOF = IMPORT + 1;  //("instanceof"),
	static const int INT = INSTANCEOF + 1;  //("int"),
	static const int INTERFACE = INT + 1;  //("interface"),
	static const int LONG = INTERFACE + 1;  //("long"),
	static const int NATIVE = LONG + 1;  //("native"),
	static const int NEW = NATIVE + 1;  //("new"),
	static const int PACKAGE = NEW + 1;  //("package"),
	static const int PRIVATE = PACKAGE + 1;  //("private"),
	static const int PROTECTED = PRIVATE + 1;  //("protected"),
	static const int PUBLIC = PROTECTED + 1;  //("public"),
	static const int RETURN = PUBLIC + 1;  //("return"),
	static const int SHORT = RETURN + 1;  //("short"),
	static const int STATIC = SHORT + 1;  //("static const"),
	static const int STRICTFP = STATIC + 1;  //("strictfp"),
	static const int STRING = STRICTFP + 1;  //("String"),
	static const int SUPER = STRING + 1;  //("super"),
	static const int SWITCH = SUPER + 1;  //("switch"),
	static const int SYNCHRONIZED = SWITCH + 1;  //("synchronized"),
	static const int THIS = SYNCHRONIZED + 1;  //("this"),
	static const int THROW = THIS + 1;  //("throw"),
	static const int THROWS = THROW + 1;  //("throws"),
	static const int TRANSIENT = THROWS + 1;  //("transient"),
	static const int TRY = TRANSIENT + 1;  //("try"),
	static const int VOID = TRY + 1;  //("void"),
	static const int VOLATILE = VOID + 1;  //("volatile"),
	static const int WHILE = VOLATILE + 1;  //("while"),
	static const int INTLITERAL = WHILE + 1;
	static const int LONGLITERAL = INTLITERAL + 1;
	static const int FLOATLITERAL = LONGLITERAL + 1;
	static const int DOUBLELITERAL = FLOATLITERAL + 1;
	static const int CHARLITERAL = DOUBLELITERAL + 1;
	static const int STRINGLITERAL = CHARLITERAL + 1;
	static const int TRUE = STRINGLITERAL + 1;  //("true"),
	static const int FALSE = TRUE + 1;  //("false"),
	static const int NULL_ = FALSE +1;  //("null"),
	static const int LPAREN = NULL_ + 1;  //("("),
	static const int RPAREN = LPAREN + 1;  //(")"),
	static const int LBRACE = RPAREN + 1;  //("{"),
	static const int RBRACE = LBRACE + 1;  //("}"),
	static const int LBRACKET = RBRACE + 1;  //("["),
	static const int RBRACKET = LBRACKET + 1;  //("]"),
	static const int SEMI = RBRACKET + 1;  //(";"),
	static const int COMMA = SEMI + 1;  //(","),
	static const int DOT = COMMA + 1;  //("."),
	static const int ELLIPSIS = DOT + 1;  //("..."),
	static const int EQ = ELLIPSIS + 1;  //("="),
	static const int GT = EQ + 1;  //(">"),
	static const int LT = GT + 1;  //("<"),
	static const int BANG = LT + 1;  //("!"),
	static const int TILDE = BANG + 1;  //("~"),
	static const int QUES = TILDE + 1;  //("?"),
	static const int COLON = QUES + 1;  //(":"),
	static const int EQEQ = COLON + 1;  //("=="),
	static const int LTEQ = EQEQ + 1;  //("<="),
	static const int GTEQ = LTEQ + 1;  //(">="),
	static const int BANGEQ = GTEQ + 1;  //("!="),
	static const int AMPAMP = BANGEQ + 1;  //("&&"),
	static const int BARBAR = AMPAMP + 1;  //("||"),
	static const int PLUSPLUS = BARBAR + 1;  //("++"),
	static const int SUBSUB = PLUSPLUS + 1;  //("--"),
	static const int PLUS = SUBSUB + 1;  //("+"),
	static const int SUB = PLUS + 1;  //("-"),
	static const int STAR = SUB + 1;  //("*"),
	static const int SLASH = STAR + 1;  //("/"),
	static const int AMP = SLASH + 1;  //("&"),
	static const int BAR = AMP + 1;  //("|"),
	static const int CARET = BAR + 1;  //("^"),
	static const int PERCENT = CARET + 1;  //("%"),
	static const int LTLT = PERCENT + 1;  //("<<"),
	static const int GTGT = LTLT + 1;  //(">>"),
	static const int GTGTGT = GTGT + 1;  //(">>>"),
	static const int PLUSEQ = GTGTGT + 1;  //("+="),
	static const int SUBEQ = PLUSEQ + 1;  //("-="),
	static const int STAREQ = SUBEQ + 1;  //("*="),
	static const int SLASHEQ = STAREQ + 1;  //("/="),
	static const int AMPEQ = SLASHEQ + 1;  //("&="),
	static const int BAREQ = AMPEQ + 1;  //("|="),
	static const int CARETEQ = BAREQ + 1;  //("^="),
	static const int PERCENTEQ = CARETEQ + 1;  //("%="),
	static const int LTLTEQ = PERCENTEQ + 1;  //("<<="),
	static const int GTGTEQ = LTLTEQ + 1;  //(">>="),
	static const int GTGTGTEQ = GTGTEQ + 1;  //(">>>="),
	static const int MONKEYS_AT = GTGTGTEQ + 1;  //("@"),
	static const int CUSTOM = MONKEYS_AT + 1;
	static vector<string> tokens;
	static vector<string> tokenStrs;
	static void init();
	static string tokenName(int id){return tokens[id];};
	static string tokenStr(int id){return tokenStrs[id];};

	int id;
	string name;
	int pos;
	Token(int id ,string name, int pos) {
		this->id = id;
		this->name = name;
		this->pos = pos;

	}


};

#endif /* PARSER_TOKEN_HPP_ */
