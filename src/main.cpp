//============================================================================
// Name        : zlcompiler.cpp
// Author      : zl
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "compiler/Compiler.hpp"
 int main(int argc,char *argv[]){
//    ios_base::sync_with_stdio(false);
//	string fileName("E:/workspace/c++/zlcompiler/src/test/test.zl");
	string fileName("E:/workspace/c++/zlcompiler/src/test/test_syntax.zl");
//	string fileName("E:/workspace/c++/zlcompiler/src/test/test_pretty.zl");
//	if(argc <2){
//		cout<<"没有输入文件";
//		return 0;
//	}
	//7.5_start test
	Compiler* complier =  Compiler::instance();
	complier->compile(fileName);


	return 0;
}
