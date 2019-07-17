/*
 * Gen.cpp
 *
 *  Created on: 2018年1月5日
 *      Author: ZL
 */

#include "Gen.hpp"
int* Gen::genKey = new int ;
Gen* Gen::instance(){
	Gen* gen = (Gen*)Context::get(genKey);
	if(gen == NULL){
		gen = new Gen();
		Context::put(genKey,gen);
	}
	return gen;
}


Gen::Gen() {
	 Context::put(genKey, this);
	 log = Log::instance();
	 syms = Symtab::instance();
	 chk = Check::instance();
	 attrEnv = NULL;
	 toplevel = NULL;
	 methodType = NULL;
	 code = NULL;
	 env = NULL;
	 pt = NULL;
	 pool = NULL;
	 items = NULL;
	 result = NULL;
	 nerrs = 0;
	 endPositions = 0;
}

Gen::~Gen() {
}
/**
 * gen method
 */
void Gen::genDef(Tree* tree, Env<GenContext*>* env){
	Env<GenContext*>* preEnv = this->env;
	this->env = env;
	tree->accept(this);
	this->env = preEnv;


}
void Gen::genStat(Tree* tree, Env<GenContext*>* env){
	if(code->isAlive()){
		genDef(tree,env);
	}else if(env->info->isSwitch&&tree->getTag()==Tree::VARDEF){
		//switch中定义的变量 ， 要在局部变量表中分配位置，c++中是不支持这种语法的
		code->newLocalVar(((VariableDecl*)tree)->sym);
	}


}
template<class T>
void Gen::genStats(vector<T> trees, Env<GenContext*>* env){

	for(int i = 0;i<trees.size();i++)
		genStat(trees[i],env);

}
void Gen::genArgs(vector<Expression*> trees, vector<Type*> ts){
	for(int i = 0;i<trees.size();i++)
			genExpr(trees[i],ts[i])->load();
}

CondItem* Gen::genCond(Tree* tree){

	return NULL;
}
/**
 * 为表达式生成代码，判断常量的情况
 */
Item* Gen::genExpr(Tree* tree ,Type* pt){
	Type* prept = this->pt;
	if(tree->type->isConst()){
		result = items->makeImmediateItem(tree->type);
	}else{
		this->pt = pt;
		tree->accept(this);
	}
	this->pt = prept;
	return result->coerce(pt);

}


/**
 * 生成code
 *
 */
void Gen::genMethod(MethodDecl* md,Env<GenContext*>* env){

	int startpc = initCode(md,env);
	genStat(md->body,env);
	if(code->isAlive()){
		code->statBegin(md->body->endPos);
		//生成return指令
	}


}


/**
 * 准备code数据
 * this ， 参数变量，入口pc
 */
int Gen::initCode(MethodDecl* md,Env<GenContext*>* env){

	md->sym->code = code = new Code(md->sym,pool);
	items = new Items(pool,code);
	//给非静态方法添加this和参数到局部变量表中
	//此方法为非静态方法，构造函数为非静态方法
	if(md->sym->flags_field&Flags::STATIC==0){

		code->setDefined(code->newLocalVar(new VarSymbol(Flags::FINAL,Name::_this,md->sym->owner->type,md->sym->owner)));
	}
	//添加参数到局部变量表中
	for(int i = 0;i<md->params.size();i++){
		code->setDefined(code->newLocalVar(md->params[i]->sym));
	}
	code->markAlive();
	//没有附加信息，start从0开始
	int startpc = 0;
	return startpc;

}

/**
 *	方法代码生成前准备工作
 *	1.(静态)成员变量初始化和(静态)构造块中代码添加到(静态)构造方法中
 *	2. 进入各方法生成代码
 */
void Gen::genClass(Env<AttrContext*>* env, ClassDecl* cdef){

	//defs只剩方法，在write阶段变量从 classsym中符号表中找

	cdef->defs = fillInitDefs(cdef->defs,cdef->sym);
	Env<GenContext*> * localEnv = new Env<GenContext*>(cdef,new GenContext());
	pool = new Pool();
	for(int i = 0;i<cdef->defs.size();i++){
		genDef(cdef->defs[i],localEnv);

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
	Env<GenContext*> * localEnv = env->dup(tree);
	localEnv->enclMethod = tree;
	this->pt = ((MethodType*)(tree->sym->type))->restype;
	genMethod(tree,localEnv);
	//delete localEnv;
}
void Gen::visitVarDef(VariableDecl* tree) {
}
void Gen::visitSkip(Skip* tree) {
}
void Gen::visitBlock(Block* tree) {
	int limit = code->nextreg;
	Env<GenContext*>* localEnv = env->dup(tree);
	genStats(tree->stats,localEnv);
	//delete localEnv;

	if(env->tree->getTag()!=Tree::METHODDEF){
		//method的block在genMethod完成
		code->statBegin(tree->endPos);
		code->endScopes(limit);
		code->pendingStatPos = -1;

	}

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


/**
 * GenContext
 */

GenContext::GenContext(){

	exit = NULL;
	cont = NULL;
	isSwitch = false;

}


void GenContext::addExit(Chain* c){
	exit = Chain::merge(c,exit);
}
void GenContext::addCont(Chain* c){
	cont = Chain::merge(c,cont);
}





