/*
 * Lexer.cpp
 *
 *  Created on: 2017年12月2日
 *      Author: ZL
 */
#include "Lexer.hpp"
#include <exception>

Lexer::Lexer(Log* log,Source* source):source(source),log(log) {
	vector<char > srcBuf = source->getSrcBuf();
	this->buf = new char[srcBuf.size()];
	memcpy(this->buf, srcBuf.data(), srcBuf.size());
//	this->source = source;
	keyWords =  new KeyWords();
	buflen = srcBuf.size();
	buf[buflen-1] = EOI;
	sbuflen = 4096;
	sbuf = new char[sbuflen];
	pos = 0;
	bp = -1;
	sp = 0;
	token = NULL;
	tp = 0;
	ch = 0;
	prevEndPos = 0;
	endPos = 0;
	scanChar();
}

Token* Lexer::nextToken(){
	if(tp<tokens.size()){
		 return token = tokens.at(tp++);
	}
}


void Lexer::parse() {
	while (bp<buflen) {
		scan();
	}
	if(util::debug)
		printTokens();
}

void Lexer::scan() {
	prevEndPos = endPos;
	sp = 0;
	try {

		while (true) {
			pos = bp;
			switch (ch) {
			case ' ':
			case '\t':
			case FF:
				do {
					scanChar();
				} while (ch == ' ' || ch == '\t' || ch == FF);
				endPos = bp;
				break;
			case LF:
				scanChar();
				endPos = bp;
				break;
			case CR: //\r
				scanChar();
				if (ch == FF) //windows \r\n
					scanChar();
				endPos = bp;
				break;
			case 'A':case 'B':case 'C':case 'D':case 'E':
			case 'F':case 'G':case 'H':case 'I':case 'J':
			case 'K':case 'L':case 'M':case 'N':case 'O':
			case 'P':case 'Q':case 'R':case 'S':case 'T':
			case 'U':case 'V':case 'W':case 'X':case 'Y':
			case 'Z':
			case 'a':case 'b':case 'c':case 'd':
			case 'e':case 'f':case 'g':case 'h':case 'i':
			case 'j':case 'k':case 'l':case 'm':case 'n':
			case 'o':case 'p':case 'q':case 'r':case 's':
			case 't':case 'u':case 'v':case 'w':case 'x':
			case 'y':case 'z':
			case '$': case '_':
				scanIdent();

				return;
			case ',':
				putChar(ch);
				genToken(0,sp);
				scanChar();
				return;
			case ';':
				putChar(ch);
				genToken(0,sp);
				scanChar();
				return;
			case '(':
				putChar(ch);
				genToken(0, sp);
				scanChar();
				return;
			case ')':
				putChar(ch);
				genToken(0, sp);
				scanChar();
				return;
			case '[':
				putChar(ch);
				genToken(0, sp);
				scanChar();
				return;
			case ']':
				putChar(ch);
				genToken(0, sp);
				scanChar();
				return;
			case '{':
				putChar(ch);
				genToken(0, sp);
				scanChar();
				return;
			case '}':
				putChar(ch);
				genToken(0, sp);
				scanChar();
				return;
			case'0':case'1':case'2':case'3':
			case'4':case'5':case'6':case'7':
			case'8':case'9':
				scanNumber();
				return;
			case'.':
				scanChar();
				if('0'<=ch&&ch<='9'){
					putChar('.');
					scanFraction();
				}else if(ch=='.'){
					lexError("错误的'..' ");
					scanChar();
				}else{
					putChar('.');
					genToken(0,sp,Token::DOT);
				}
				return;
			case'/':
				scanChar();

				if(ch == '/'){//注释部分   行注释
					do{
						scanChar();
					}while(ch!=CR && ch!=LF &&bp<buflen);
					break;//允许注释隔断
				}else if(ch =='='){//    /=
					putChar('/');
					putChar('=');
					genToken(0,sp,Token::SLASHEQ);
				}else{//     /
					putChar('/');
					genToken(0,sp,Token::SLASH);
				}
				return;
			case'\'':
				scanChar();
				if(ch=='\''){
					lexError("空字符");
				}else{
					if(ch ==LF ||ch == CR){
						lexError("字符包含非法行末尾");
					}
					scanLitChar();
					if(ch == '\''){
						scanChar();
						genToken(0,sp,Token::CHARLITERAL);
					}else{
						lexError("错误的char常量");
					}
				}
				return;
			case'\"':
				scanChar();
				while(ch!='\"'&& ch != CR && ch != LF && bp < buflen)
					scanLitChar();
				if(ch == '\"'){
					genToken(0,sp,Token::STRINGLITERAL);
					scanChar();
				}else{
					lexError("错误的字符串常量");
				}
				return;
			default:
				if(isSpecial(ch)){
					scanOperator();
				}else if(bp == buflen || (ch == EOI && bp+1 == buflen)){
					genToken(0,0,Token::EOF_);
					pos = bp;
					bp = buflen;
				}else{
					lexError("非法字符"+util::toString((int)ch));
					scanChar();
				}
				return;

			}
		}

	} catch (...) {

	}

}
//标识符
void Lexer::scanIdent(){
	while(true){
		putChar(ch);
		scanChar();
		switch(ch){
			case 'A':case 'B':case 'C':case 'D':case 'E':
			case 'F':case 'G':case 'H':case 'I':case 'J':
			case 'K':case 'L':case 'M':case 'N':case 'O':
			case 'P':case 'Q':case 'R':case 'S':case 'T':
			case 'U':case 'V':case 'W':case 'X':case 'Y':
			case 'Z':
			case 'a':case 'b':case 'c':case 'd':
			case 'e':case 'f':case 'g':case 'h':case 'i':
			case 'j':case 'k':case 'l':case 'm':case 'n':
			case 'o':case 'p':case 'q':case 'r':case 's':
			case 't':case 'u':case 'v':case 'w':case 'x':
			case 'y':case 'z':
			case '$': case '_':case '0': case '1':
			case '2': case '3': case '4':case '5': case '6':
			case '7': case '8': case '9':
				break;
			case '\u001A': //EOI
				if(bp>=bp){

				}
				break;
			default://其他编码支持

				genToken(0,sp);
				return;

		}
	}


}

void Lexer::genToken(int from,int to,int id){//有些特殊token需要制定id
	char* tmp = new char[to-from];
	memcpy(tmp,sbuf+from,to-from);
	tmp[to-from] =0;
	string s(tmp);
	int token_id= keyWords->key(s);
	if(token_id==0){
		tokens.push_back(new Token(id>0?id:Token::IDENTIFIER,s,pos));
	}else{
		tokens.push_back(new Token(token_id,s,pos));
	}
}
//暂时只支持10进制数
void Lexer::scanNumber(){

	scanDigits();
	if(ch=='.'){
		putChar(ch);
		scanChar();
		scanFraction();

	}else{
		if (ch == 'l' || ch == 'L') {
			scanChar();
			genToken(0,sp,Token::LONGLITERAL);
		} else {
			genToken(0,sp,Token::INTLITERAL);
		}

	}




}

void Lexer::scanDigits(){
	do {
		putChar(ch);
		scanChar();
	} while (ch >= '0' && ch <= '9');


}
void Lexer::scanFraction(){
	scanDigits();
	if (ch == 'f' || ch == 'F') {
		putChar(ch);
		scanChar();
		genToken(0,sp,Token::FLOATLITERAL);
	} else {
		if (ch == 'd' || ch == 'D') {
			putChar(ch);
			scanChar();
		}
		genToken(0,sp,Token::DOUBLELITERAL);
	}

}
void Lexer::scanLitChar(){
	if(ch=='\\'){//转义字符
		if(buf[bp+1] == '\\'){
			bp++;
			putChar('\\');
			scanChar();
		}else{
			scanChar();
			switch(ch){
			case 'b':
				putChar('\b');
				scanChar();
				break;
			case 't':
				putChar('\t');
				scanChar();
				break;
			case 'n':
				putChar('\n');
				scanChar();
				break;
			case 'f':
				putChar('\f');
				scanChar();
				break;
			case 'r':
				putChar('\r');
				scanChar();
				break;
			case '\'':
				putChar('\'');
				scanChar();
				break;
			case '\"':
				putChar('\"');
				scanChar();
				break;
			case '\\':
				putChar('\\');
				scanChar();
				break;
			default:
				lexError("非法转义字符");

			}

		}

	}else if(bp!=buflen){
		putChar(ch);
		scanChar();
	}
}



void Lexer::scanOperator() {
	vector<char> chs;
	while (true) {
		chs.push_back(ch) ;
		putChar(ch);
		chs.push_back(0) ;
		string s(chs.data());
		int token_id = keyWords->key(s);
		if (token_id == Token::IDENTIFIER) {
			sp--;//回退
			break;
		}
		scanChar();
		if (!isSpecial(ch))
			break;
	}
	genToken(0,sp);

}



void Lexer::scanChar() {
	ch = buf[++bp];
}

void Lexer::putChar(char c) {
	if (sp == sbuflen) {
		delete sbuf;
		sbuflen = sbuflen * 2;
		sbuf = new char[sbuflen];
	}
	sbuf[sp++] = c;
}

bool Lexer::isSpecial(char ch) {
	switch (ch) {
		case '!':
		case '%':
		case '&':
		case '*':
		case '?':
		case '+':
		case '-':
		case ':':
		case '<':
		case '=':
		case '>':
		case '^':
		case '|':
		case '~':
		case '@':
			return true;
		default:
			return false;
	}
}

void Lexer::lexError(string msg){
	log->report(pos,Log::ERROR_Lexer,msg);
}



void Lexer::printTokens(){
	cout<<"    [单词流]   "<<endl;
	int size = tokens.size();
	Token* t ;
	for(int i=0;i<size;i++){
		t = tokens[i];
		cout<<"{ "<<t->name<<" , "<<Token::tokenName(t->id)<<","<<t->pos<<" }" <<endl;
	}

//	for(int i=0;i<size;i++){
//		t = tokens[i];
//		log->report(t->pos,Log::ERROR,"");
//
//	}
}

