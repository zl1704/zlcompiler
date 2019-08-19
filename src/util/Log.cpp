/*
 * Log.cpp
 *
 *  Created on: 2017年12月6日
 *      Author: ZL
 */

#include "Log.hpp"
int* Log::logKey = new int;
Log* Log::instance() {
	Log* log = (Log*) Context::get(logKey);
	if (log == NULL)
		log = new Log();
	return log;
}

Log::Log(Source* source) :
		errorNums(0), source(source) {
	Context::put(logKey, this);

}
Log::Log() :
		errorNums(0), source(NULL) {
	Context::put(logKey, this);
}

Log::~Log() {
	// TODO Auto-generated destructor stub
}

//文件名:行号  :类型: msg
//错误行
//   ^ --定位到错误位置
void Log::report(int pos, int kind, string msg) {
	errorNums++;
	cout.flush();
	int line = source->getLine(pos);
	int col = source->getCol(pos);
	string lineStr = source->getLineStr(line);
	cout << source->getFileName() << ":" << line << ": ";
	cout << getErrorKind(kind);
	cout << msg << endl;
	cout << lineStr << endl;
	for (int i = 0; i < col; i++) {
		cout << (lineStr[i] == '\t' ? '\t' : ' ');
	}
	cout << "^" << endl;

}

void Log::report(int pos, int kind, string msg, string expected) {
	report(pos, kind, msg);
	if (expected != "")
		cout << "  需要:\t" + expected << endl;

}
void Log::report(int pos, int kind, string msg, string expected, string find) {
	report(pos, kind, msg, expected);
	if (find != "")
		cout << "  找到:\t" + find << endl;

}

string Log::getErrorKind(int kind){
	switch(kind){
	case ERROR_Lexer: return "错误(Lexer): ";
	case ERROR_Parser:return "错误(Parser): ";
	case ERROR_Check: return "错误(Check): ";
	case ERROR_Attr:  return "错误(Attr): ";
	default :
		return "未知错误: ";
	}
}

