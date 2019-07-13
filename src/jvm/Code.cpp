/*
 * Code.cpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */
#include"Code.hpp"
/**
 * Code
 */

int Code::truncate(int tc){
	switch(tc){
	case ByteCodes::BYTEcode:
	case ByteCodes::SHORTcode:
	case ByteCodes::CHARcode:
		return ByteCodes::INTcode;
	default:
		return tc;
	}
}

int Code::witdh(int tc){
	switch(tc){
	case ByteCodes::LONGcode:
	case ByteCodes::DOUBLEcode:
		return 2;
	case ByteCodes::VOIDcode:
		return 0;
	default:
		return 1;
	}
}


void Code::checkCode(){
	if(cp==size){
		jbyte** new_code = new jbyte[size*2];
		memset(new_code,NULL,size*2*sizeof(jbyte));
		delete[] code;
		code = new_code;
	}
}










/**
 *  Chain
 */
Chain::Chain(int pc,Chain* next){
	this->pc = pc;
	this->next = next;

}

Chain* Chain::merge(Chain* c1,Chain* c2){
	if(c1==NULL) return c2;
	if(c2==NULL) return c1;
	Chain* new_chain = NULL;
	if(c1->pc<c2->pc)
		new_chain = new Chain(c2->pc,merge(c1,c2->next));
	new_chain =  new Chain(c1->pc,merge(c1->next,c2));
	delete c1;
	delete c2;
	return new_chain;
}
