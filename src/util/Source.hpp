/*
 * CharStream.hpp
 *
 *  Created on: 2017年12月2日
 *      Author: ZL
 */

#ifndef UTIL_SOURCE_HPP_
#define UTIL_SOURCE_HPP_
#include <stdlib.h>
#include "global.hpp"

/**
 * 源文件
 */

class Source{
public:
	Source(){};
	Source(string fileName);
	string getFileName(){return fileName;}
	vector<char> getSrcBuf(){return *buf;}
	string getLineStr(int line);
	int getLine(int pos){
		if(pos >=buf->size())
			return rowbuf[buf->size()-1];
		return rowbuf[pos];
	}
	int getCol(int pos){return colbuf[buf->size()<=pos?buf->size()-1:pos]; };
private :
	string fileName;
	vector<char>* buf; //源文件字符流
	int* rowbuf; //行号表
	int* colbuf;

};


//文件字符流
class CharStream {
public :
	CharStream(string fileName);
	void readFile(string fileName);

	vector<char> buf; //字符流
	vector<int> rowBuffer;//行号表
	vector<int> colBuffer;
	ifstream file;



};





#endif /* UTIL_SOURCE_HPP_ */
