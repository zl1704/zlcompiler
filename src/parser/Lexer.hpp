/*
 * Lexer.h
 *
 *  Created on: 2017年12月4日
 *      Author: ZL
 */

#ifndef PARSER_LEXER_HPP_
#define PARSER_LEXER_HPP_
#include "../util/global.hpp"
#include "Token.hpp"
#include "KeyWords.hpp"
#include "../util/Log.hpp"
#include "../util/Source.hpp"

/**
 * 词法分析器
 */
class Lexer {
public:
	static const int lexerKey = 2;
	static Lexer* instance();

	Lexer(){};

	Lexer(Log *log,Source* source);
	void parse(); //词法分析
	//当前token
	Token* getToken(){return token;};
	//下一个token
	Token* nextToken();
	//keywords
	KeyWords* getKeyWords(){return keyWords;}
	Source* getSource(){return source;}

	//debug
	void printTokens();
private:
	char* buf; //字符流
	char* sbuf; //字符缓冲
	int sbuflen;
	int buflen; //字符流大小
	int pos; //当前位置
	int sp; //缓冲区位置
	int bp;//扫描的位置
	char ch; //当前char

	Log *log;//日志

	Source* source;

	Token* token;
	int tp;//token 的索引

	vector<Token* > tokens;

	//关键字 集合
	KeyWords* keyWords;


	/** 最后一个token最后一个字符位置
	 */
	int endPos;

	/** 前一个token最后一个字符位置
	 */
	int prevEndPos;

	/**
	 * 制表符
	 */
	static const char TAB = 0x9;

	/**
	 * 换行 \n
	 */
	static const char LF = 0xA;

	/**
	 * 换页
	 */
	static const char FF = 0xC;

	/**
	 * 回车 \r
	 */
	static const char CR = 0xD;

	/**
	 * end of input
	 */
	static const char EOI = 0x1A;
	//读取一个字符
	void scanChar();
	//放入缓冲区
	void putChar(char c);
	void scan();
	//标识符
	void scanIdent();
	//生成token 并保存
	void genToken(int from,int to,int id=0);
	//扫描字符常量
	void scanLitChar();
	//扫描 数字
	void scanNumber();
	//0-9
	void scanDigits();
	//小数部分
	void scanFraction();
	//小数部分   f F d D 前缀
	void scanFractionAndSuffix();
	//错误提示
	void lexError(string msg);
	//判断特殊字符   操作符
	bool isSpecial(char);
	//扫描操作符
	void scanOperator();


};


#endif /* PARSER_LEXER_HPP_ */
