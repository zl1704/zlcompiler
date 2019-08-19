/*
 * KeyWords.h
 *
 *  Created on: 2017年12月4日
 *      Author: ZL
 */

#ifndef PARSER_KEYWORDS_HPP_
#define PARSER_KEYWORDS_HPP_

#include "../util/global.hpp"
#include "../parser/Token.hpp"
class KeyWords{
public :
	KeyWords();
	int key(string key){
		return keywords[key];
	}
private:
	map<string,int> keywords;
	void initKeyWords();

};



#endif /* PARSER_KEYWORDS_HPP_ */
