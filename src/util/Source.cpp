/*
 * CharStream.cpp
 *
 *  Created on: 2017年12月2日
 *      Author: ZL
 */

#include "Source.hpp"
Source::Source(string fileName) :
		fileName(fileName) {
	CharStream cs(fileName);
	buf = new vector<char>();
	this->buf->assign(cs.buf.begin(), cs.buf.end());
	this->rowbuf = new int[cs.rowBuffer.size()];
	this->colbuf = new int[cs.colBuffer.size()];
	memcpy(this->rowbuf, cs.rowBuffer.data(), 4*cs.rowBuffer.size());
	memcpy(this->colbuf, cs.colBuffer.data(), 4*cs.colBuffer.size());
}

string Source::getLineStr(int line) {
	int i = 0;
	int row = 1;
	string str = "";
	char c;
	while (i < buf->size()) {
		if (row < line) {
			c = (*buf)[i];
			while (c != '\n' && c != '\r\n' && c != '\r' && i <= buf->size()) {
				i++;
				c = (*buf)[i];
			}
			row++;
			i++;
			continue;
		} else {
			c = (*buf)[i];
			while (c != '\n' && c != '\r\n' && c != '\r' && i <= buf->size()) {
				str = str + util::toString(c);
				i++;
				c = (*buf)[i];
			}

			break;
		}

	}
	return str;
}

CharStream::CharStream(string fileName) {
	readFile(fileName);
}

void CharStream::readFile(string fileName) {
	char temp;

	//std::cout << "Opening file...\n";
	cout << fileName << endl;
	file.open(fileName.data());
	if (file.fail()) {
		string errorMsg = "Failed to read input file\n";
		cerr << errorMsg;
		exit(1);
		return ;
	}

	//std::cout << "Reading file into memory...\n";
	// read the file into memory
	int row = 1;
	int col = 0;
	int filesize = 0;
	while (file.is_open() && !file.eof() && file.good()) {

 		temp = file.get();
		if (temp == '\n' || temp == '\r\t' || temp == '\r') {
			row++;
			buf.push_back('\n');
			filesize++;
			//std::cout << temp;
			colBuffer.push_back(col);
			col = 0;

		}else {
			buf.push_back(temp);
			filesize++;
			colBuffer.push_back(col);
			col++;
		}
		rowBuffer.push_back(row);

	}
	 buf[filesize-1] = 0;
	//std::cout << "Finished reading file into memory...\n";
//	std::cout << "row: " << rowBuffer[25] << endl;
//	std::cout << "col: " << colBuffer[0] << endl;
//	std::cout << "data: " << buf.data()<< endl;
//	std::cout << "size: " <<  buf.size()<< endl;
}

