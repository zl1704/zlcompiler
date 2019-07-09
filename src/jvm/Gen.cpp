/*
 * Gen.cpp
 *
 *  Created on: 2018年1月5日
 *      Author: ZL
 */

#include "Gen.hpp"
int* Gen::genKey = new int ;
Gen* Gen::instance(){

}


Gen::Gen() {
	 Context::put(genKey, this);
	 log = Log::instance();
	 syms = Symtab::instance();
	 chk = Check::instance();


}

Gen::~Gen() {
}
/**
 * gen method
 */
void Gen::genDef(Tree* tree, Env<AttrContext*>* env){}
void Gen::genStat(Tree* tree, Env<AttrContext*>* env){}
template<class T>
void Gen::genStats(vector<T> trees, Env<AttrContext*>* env){}
void Gen::genArgs(vector<Expression*> trees, vector<Type*> ts){}
/**
 *	方法代码生成前准备工作
 *	1.(静态)成员变量初始化和(静态)构造块中代码添加到(静态)构造方法中
 *	2. 进入各方法生成代码
 */
void Gen::genClass(Env<AttrContext*>* env, ClassDecl* cdef){

	//defs只剩方法，在write阶段变量从 classsym中符号表中找
	cdef->defs = fillInitDefs(cdef->defs,cdef->sym);

	for(int i = 0;i<cdef->defs.size();i++){
		genDef(cdef->defs[i],env);

	}


}

vector<Tree*> Gen::fillInitDefs(vector<Tree*> defs,ClassSymbol* csym){
	vector<Statement*> initCode;
	vector<Statement*> cinitCode;
	vector<Tree*> methodDefs;

	for(int i=0;i<defs.size();i++){
		Tree* def = defs[i];
		Block* block;
		VariableDecl* vdef;
		VarSymbol* vsym;
		switch(def->getTag()){
		case Tree::BLOCK:
			block = (Block*)def;
			//静态构造块
			if(block->isStatic()){
				util::appendList(cinitCode,block->stats);
			}else{
				util::appendList(initCode,block->stats);
			}
			break;

		case Tree::VARDEF:
			vdef = (VariableDecl*) def;
			vsym = vdef->sym;
			if(vdef->init!=NULL){
				//实例变量总是要初始化
				if((vsym->flags_field&Flags::STATIC)==0){
					Statement* init = TreeMaker::makeAssignment(vdef->pos,vsym,vdef->init);
					initCode.push_back(init);
//					cout << "init code size :" << initCode.size()<<endl;
				}else if(!vsym->type->isConst()){
					//非静态编译期常量
					Statement* init = TreeMaker::makeAssignment(vdef->pos, vsym,
							vdef->init);
					cinitCode.push_back(init);

				}
			}
			break;
		case Tree::METHODDEF:
			methodDefs.push_back(def);
			break;
		default:
			//错误处理
			break;

		}


	}
	//把initcode添加到 构造方法中
	if(initCode.size()!=0){
		for(int i =0;i<methodDefs.size();i++){
			fillMethod((MethodDecl*)methodDefs[i],initCode);
		}
//		for(Tree* t:methodDefs){
//			fillMethod((MethodDecl*)t,initCode);
//		}
	}
	//填充静态初始化代码
	if(cinitCode.size()!=0){
		vector<Type*> args;
		vector<VariableDecl*> params;
		MethodSymbol* ms = new MethodSymbol(Flags::STATIC, Name::cinit,
				new MethodType(args, syms->voidType, syms->methodClass), csym);
		csym->members_field->enter(ms);
		Block* block = TreeMaker::makeBlock(0, 0, cinitCode);
		int pos = block->pos;
		methodDefs.push_back(
				TreeMaker::makeMethodDecl(pos,
						new Modifiers(pos, ms->flags_field),
						TreeMaker::makeTypeIdent(pos, TypeTags::VOID),
						Name::cinit, params, block));


	}


	return methodDefs;
}

/**
 * 构造方法代码布局：
 * --父类构造方法调用 super();
 * 当前类变量初始化代码
 * 构造方法代码
 *
 */
void Gen::fillMethod(MethodDecl* md ,vector<Statement*> initCode){

	if(md->name.compare(Name::init)==0){
		vector<Statement*> stats  = md->body->stats;
		vector<Statement*> newstats ;
		//添加super()调用 ，暂时不支持继承，需要在Enter中完成对构造方法的填充
		//...
		//initcode
		util::appendList(newstats,initCode);
		//构造方法中的代码
		util::appendList(newstats,stats);

		md->body->stats = newstats;


	}

}




void Gen::visitTopLevel(CompilationUnit* tree) {
}



void Gen::visitClassDef(ClassDecl* tree) {
}
void Gen::visitMethodDef(MethodDecl* tree) {
}
void Gen::visitVarDef(VariableDecl* tree) {
}
void Gen::visitSkip(Skip* tree) {
}
void Gen::visitBlock(Block* tree) {
}
void Gen::visitDoLoop(DoWhileLoop* tree) {
}
void Gen::visitWhileLoop(WhileLoop* tree) {
}
void Gen::visitForLoop(ForLoop* tree) {
}

void Gen::visitSwitch(Switch* tree) {
}
void Gen::visitCase(Case* tree) {
}

void Gen::visitConditional(Conditional* tree) {
}
void Gen::visitIf(If* tree) {
}
void Gen::visitExec(ExpressionStatement* tree) {
}
void Gen::visitBreak(Break* tree) {
}
void Gen::visitContinue(Continue* tree) {
}
void Gen::visitReturn(Return* tree) {
}
void Gen::visitApply(MethodInvocation* tree) {
}
void Gen::visitNewClass(NewClass* tree) {
}
void Gen::visitNewArray(NewArray* tree) {
}
void Gen::visitParens(Parens* tree) {
}
void Gen::visitAssign(Assign* tree) {
}
void Gen::visitAssignop(AssignOp* tree) {
}
void Gen::visitUnary(Unary* tree) {
}
void Gen::visitBinary(Binary* tree) {
}
void Gen::visitTypeCast(TypeCast* tree) {
}
void Gen::visitTypeTest(InstanceOf* tree) {
}
void Gen::visitIndexed(ArrayAccess* tree) {
}
void Gen::visitSelect(FieldAccess* tree) {
}
void Gen::visitIdent(Ident* tree) {
}
void Gen::visitLiteral(Literal* tree) {
}
void Gen::visitTypeIdent(PrimitiveTypeTree* tree) {
}
void Gen::visitTypeArray(ArrayTypeTree* tree) {
}
void Gen::visitModifiers(Modifiers* tree) {
}
void Gen::visitTree(Tree* tree) {
}
