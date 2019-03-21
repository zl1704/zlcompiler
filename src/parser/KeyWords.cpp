/*
 * KeyWords.cpp
 *
 *  Created on: 2017年12月4日
 *      Author: ZL
 */

#include "KeyWords.hpp"

KeyWords::KeyWords() {

	initKeyWords();
}

void KeyWords::initKeyWords() {
	keywords["abstract"] = Token::ABSTRACT;
	keywords["assert"] = Token::ASSERT;
	keywords["boolean"] = Token::BOOLEAN;
	keywords["break"] = Token::BREAK;
	keywords["byte"] = Token::BYTE;
	keywords["case"] = Token::CASE;
	keywords["catch"] = Token::CATCH;
	keywords["char"] = Token::CHAR;
	keywords["class"] = Token::CLASS;
	keywords["const"] = Token::CONST;
	keywords["continue"] = Token::CONTINUE;
	keywords["default"] = Token::DEFAULT;
	keywords["do"] = Token::DO;
	keywords["double"] = Token::DOUBLE;
	keywords["else"] = Token::ELSE;
	keywords["enum"] = Token::ENUM;
	keywords["extends"] = Token::EXTENDS;
	keywords["final"] = Token::FINAL;
	keywords["finally"] = Token::FINALLY;
	keywords["float"] = Token::FLOAT;
	keywords["for"] = Token::FOR;
	keywords["if"] = Token::IF;
	keywords["goto"] = Token::GOTO;
	keywords["implements"] = Token::IMPLEMENTS;
	keywords["import"] = Token::INSTANCEOF;
	keywords["instanceof"] = Token::INTERFACE;
	keywords["int"] = Token::INT;
	keywords["interface"] = Token::INTERFACE;
	keywords["naive"] = Token::NATIVE;
	keywords["long"] = Token::LONG;
	keywords["new"] = Token::NEW;
	keywords["package"] = Token::PACKAGE;
	keywords["private"] = Token::PRIVATE;
	keywords["protected"] = Token::PROTECTED;
	keywords["public"] = Token::PUBLIC;
	keywords["return"] = Token::RETURN;
	keywords["short"] = Token::SHORT;
	keywords["static"] = Token::STATIC;
	keywords["strictfp"] = Token::STRICTFP;
	keywords["String"] = Token::STRING;
	keywords["super"] = Token::SUPER;
	keywords["switch"] = Token::SWITCH;
	keywords["synchronized"] = Token::SYNCHRONIZED;
	keywords["this"] = Token::THIS;
	keywords["throw"] = Token::THROW;
	keywords["throws"] = Token::THROWS;
	keywords["transient"] = Token::TRANSIENT;
	keywords["try"] = Token::TRY;
	keywords["void"] = Token::VOID;
	keywords["volatile"] = Token::VOLATILE;
	keywords["while"] = Token::WHILE;
	keywords["true"] = Token::TRUE;
	keywords["false"] = Token::FALSE;
	keywords["null"] = Token::NULL_;
	keywords["("] = Token::LPAREN;
	keywords[")"] = Token::RPAREN;
	keywords["{"] = Token::LBRACE;
	keywords["}"] = Token::RBRACE;
	keywords["["] = Token::LBRACKET;
	keywords["]"] = Token::RBRACKET;
	keywords[";"] = Token::SEMI;
	keywords[","] = Token::COMMA;
	keywords["."] = Token::DOT;
	keywords["..."] = Token::ELLIPSIS;
	keywords["="] = Token::EQ;
	keywords[">"] = Token::GT;
	keywords["<"] = Token::LT;
	keywords["!"] = Token::BANG;
	keywords["~"] = Token::TILDE;
	keywords["?"] = Token::QUES;
	keywords[":"] = Token::COLON;
	keywords["=="] = Token::EQEQ;
	keywords["<="] = Token::LTEQ;
	keywords[">="] = Token::GTEQ;
	keywords["!="] = Token::BANGEQ;
	keywords["&&"] = Token::AMPAMP;
	keywords["||"] = Token::BARBAR;
	keywords["++"] = Token::PLUSPLUS;
	keywords["--"] = Token::SUBSUB;
	keywords["+"] = Token::PLUS;
	keywords["-"] = Token::SUB;
	keywords["*"] = Token::STAR;
	keywords["/"] = Token::SLASH;
	keywords["&"] = Token::AMP;
	keywords["|"] = Token::BAR;
	keywords["^"] = Token::CARET;
	keywords["%"] = Token::PERCENT;
	keywords["<<"] = Token::LTLT;
	keywords[">>"] = Token::GTGT;
	keywords[">>>"] = Token::GTGTGT;
	keywords["+="] = Token::PLUSEQ;
	keywords["-="] = Token::SUBEQ;
	keywords["*="] = Token::STAREQ;
	keywords["/="] = Token::SLASHEQ;
	keywords["&="] = Token::AMPEQ;
	keywords["|="] = Token::BAREQ;
	keywords["^="] = Token::CARETEQ;
	keywords["%="] = Token::PERCENTEQ;
	keywords["<<="] = Token::LTLTEQ;
	keywords[">>="] = Token::GTGTEQ;
	keywords[">>>="] = Token::GTGTGTEQ;
	keywords["@"] = Token::MONKEYS_AT;
}
