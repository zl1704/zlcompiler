/*
 * Log.hpp
 *
 *  Created on: 2017年12月6日
 *      Author: ZL
 */

#ifndef UTIL_LOG_HPP_
#define UTIL_LOG_HPP_
#include "global.hpp"
#include "Source.hpp"
#include "../util/Context.hpp"
#include <cstdarg>
class Log {

public:
	static int* logKey;
	Source* source;

	static const int ERROR = 1;
	static const int EXPECTED_1= 1;
	static const int EXPECTED_2= 2;
	static const int EXPECTED_3= 3;
	static Log* instance();
	Log();
	Log(Source* source);
	int getErrNums(){return errorNums;}
	void report(int pos,int kind, string msg);
	void report(int pos,int kind, string msg,string expected);
	void report(int pos,int kind, string msg,string expected,string find);
	virtual ~Log();
private :
	int errorNums ;
};

#endif /* UTIL_LOG_HPP_ */
