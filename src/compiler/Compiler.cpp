/*
 * Compiler.cpp
 *
 *  Created on: 2017年12月4日
 *      Author: ZL
 */

#include "Compiler.hpp"
#include "Env.hpp"
int* Compiler::compilerKey = new int ;
Compiler::Compiler(){

	Pretty::instance();
	log = Log::instance();
	enter = Enter::instance();
	attr = Attr::instance();
	gen = Gen::instance();
}
Compiler* Compiler::instance(){
	Compiler* compiler = (Compiler*)Context::get(compilerKey);
	if(compiler ==NULL){
		compiler = new Compiler();
		Context::put(compilerKey,compiler);
	}
	return compiler;
}


void Compiler::compile(string fileName){

	Source*  source =new Source(fileName);
	log->source = source;
	Token::init();
	lexer = new Lexer(log,source);
	lexer->parse();
	parser = new Parser(lexer,log);
	if(log->getErrNums()>0)
				return ;

	CompilationUnit* unit = parser->parse();
	vector<CompilationUnit* > units;
	units.push_back(unit);
	if(log->getErrNums()>0)
			return ;
	enter->main(units);

	//第二阶段
	compile2();



}
/**
 * 第二阶段  属性标注 ，代码生成
 */
void Compiler::compile2(){
	if(log->getErrNums()>0)
		return ;
	vector<Env<AttrContext*>*> todos = enter->todos;
	for(int i =0;i<todos.size();i++)
		generate(attr->attribute(todos.at(i)));



}

void Compiler::generate(Env<AttrContext* >* env){
	Pretty* pretty =Pretty::instance();
	ClassDecl* cdef = env->enclClass;
	pretty->visitClassDef(cdef);
	gen->genClass(env,cdef);



}
