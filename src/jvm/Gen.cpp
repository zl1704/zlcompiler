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
/**
 * 生成条件表达项，返回值为true,false
 */
CondItem* Gen::genCond(Tree* tree){

	Tree* innerTree = Check::skipParens(tree);
	if(tree->getTag() == Tree::CONDEXPR){
		// (cond)?(1):(2)
		Conditional* ctree = (Conditional*) tree;
		CondItem* cond = genCond(ctree->cond);
		//确定真值情况
		if(cond->isTrue()){
			code->resolve(cond->trueJumps);
			return genCond(ctree->truepart);
		}
		if(cond->isFalse()){
			code->resolve(cond->falseJumps);
			return genCond(ctree->falsepart);
		}
		//按if-else翻译
		//生成false跳转
		Chain* secondJumps = cond->jumpFalse();
		//合并trueJumps
		code->resolve(cond->trueJumps);
		CondItem* first = genCond(ctree->truepart);
		//(1)中 false跳转
		Chain* falseJumps = first->jumpFalse();
		code->resolve(first->trueJumps);
		//true跳转，跳到(2)之后
		Chain* trueJumps = code->branch(ByteCodes::goto_);
		//false跳转
		code->resolve(secondJumps);
		//(2)语句
		CondItem* second = genCond(ctree->falsepart);
		//合并(1) (2)中true ，false出口
		CondItem* result = items->makeCondItem(second->opcode,Chain::merge(trueJumps,second->trueJumps),Chain::merge(falseJumps,second->falseJumps));
		return result;
	}else{
		return genExpr(tree,syms->booleanType)->mkCond();
	}

}
/**
 * 为表达式生成代码，判断常量的情况
 */
Item* Gen::genExpr(Tree* tree ,Type* pt){
	Type* prept = this->pt;
	if(tree->type->isConst()){
		result = items->makeImmediateItem((ConstType*)tree->type);
	}else{
		this->pt = pt;
		tree->accept(this);
	}
	this->pt = prept;
	return result->coerce(pt);

}
/**
 * 三种循环方式
 *  for: let,cond,body,step
 *  while:cond,body
 *  do-while:body,cond
 */


void Gen::genLoop(Statement* loop, Statement* body, Expression* cond,
		vector<ExpressionStatement*> step, bool testFirst) {

	Env<GenContext* >* loopEnv = env->dup(loop,new GenContext());
	int startpc = code->entryPoint();
	if(testFirst){
		//while ,for
		CondItem* c;
		if(cond != NULL){
			code ->statBegin(cond->pos);
			c = genCond(Check::skipParens(cond));
		}else{
			//没有条件直接goto  无test的for
			c = items->makeCondItem(ByteCodes::goto_,NULL,NULL);
		}
		//false跳转,会生成一个跳转指令
		Chain* loopDone = c->jumpFalse();
		//true label
		code->resolve(c->trueJumps);
		genStat(body,loopEnv);
		//处理continue
		code->resolve(loopEnv->info->cont);
		//step ,for循环会有
		genStats(step,loopEnv);
		//生成跳转指令，跳转到循环start处
		code->resolve(code->branch(ByteCodes::goto_),startpc);
		//回填false链，跳出循环
		code->resolve(loopDone);
	}else{
		//do-while
		//body
		genStat(body,loopEnv);
		//continue
		code->resolve(loopEnv->info->cont);
		//step ,for循环会有,这里可以不用写
		genStats(step, loopEnv);
		CondItem* c;
		if (cond != NULL) {
			code->statBegin(cond->pos);
			c = genCond(Check::skipParens(cond));
		} else {
			//没有条件直接goto  无test的for
			c = items->makeCondItem(ByteCodes::goto_,NULL,NULL);
		}

		//回填
		code->resolve(c->jumpTrue(),startpc);
		code->resolve(c->falseJumps);
	}
	//处理循环中的break,exit保存在env中
	code->resolve(loopEnv->info->exit);
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
		if(env->enclMethod==NULL||((MethodType*)(env->enclMethod->sym->type))->restype->tag==TypeTags::VOID){
			code->emitop0(ByteCodes::return_);
		}
	}
	code->endScopes(0);

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
	int startpc = code->entryPoint();
	return startpc;

}

/**
 *	方法代码生成前准备工作
 *	1.(静态)成员变量初始化和(静态)构造块中代码添加到(静态)构造方法中
 *	2. 进入各方法生成代码
 */
void Gen::genClass(Env<AttrContext*>* env, ClassDecl* cdef){

	if(util::debug)
		cout << "=========Gen Start!  "<< endl;

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
	VarSymbol* v= tree->sym;
	code->newLocalVar(v);
	//右值不为空，生成赋值语句
	if(tree->init!=NULL){
		genExpr(tree->init,v->type)->load();
		items->makeLocalItem(v)->store();
	}
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
	vector<ExpressionStatement*> step;
	genLoop(tree,tree->body,tree->cond,step,false);
}
void Gen::visitWhileLoop(WhileLoop* tree) {
	vector<ExpressionStatement*> step;
	genLoop(tree,tree->body,tree->cond,step,true);
}
void Gen::visitForLoop(ForLoop* tree) {
	int limit = code->nextreg;
	genStats(tree->init,env);
	genLoop(tree,tree->body,tree->cond,tree->step,true);
	code->endScopes(limit);
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
	env->info->addExit(code->branch(ByteCodes::goto_));
}
void Gen::visitContinue(Continue* tree) {
	env->info->addCont(code->branch(ByteCodes::goto_));
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
	OperatorSymbol* opsym = ((OperatorSymbol*)tree->opsym);
	/**
	 * string add情况
	 * 常量折叠已经在attr阶段处理
	 * 非字面字符串相加暂时不处理，后续版本更新
	 */
	if(opsym->opcode == ByteCodes::string_add){


	}else if(tree->type->tag == Tree::AND){
		//&& 条件
		CondItem* lcond = genCond(tree->lhs);
		if(!lcond->isFalse()){
			Chain* falseJumps = lcond->jumpFalse();
			code->resolve(lcond->trueJumps);
			CondItem* rcond = genCond(tree->rhs);

			result = items->makeCondItem(rcond->opcode,rcond->trueJumps,Chain::merge(falseJumps,rcond->falseJumps));


		}else{
			//常量false情况,短路机制
			result = lcond;
		}
	}else if(tree->type->tag == Tree::OR){
		//思路同AND
		CondItem* lcond = genCond(tree->lhs);
		if(!lcond->isTrue()){
			Chain* trueJumps = lcond->jumpTrue();
			code->resolve(lcond->falseJumps);
			CondItem* rcond = genCond(tree->rhs);

			result = items->makeCondItem(rcond->opcode,Chain::merge(trueJumps,rcond->trueJumps),rcond->falseJumps);

		}else{
			result = lcond;
		}

	}else{
		Item* item = genExpr(tree->lhs,((MethodType*)opsym->type)->argtypes.at(0));
		item->load();
		result = completeBinop(tree->lhs,tree->rhs,opsym);
	}


}
Item*  Gen::completeBinop(Tree* lhs,Tree* rhs,OperatorSymbol* opsym){
	MethodType* mt = (MethodType*)opsym->type;
	int opcode = opsym->opcode;
	//int与0比较 直接转换为比较跳转指令(153~158)
	if(ByteCodes::if_icmpeq<=opcode && opcode<=ByteCodes::if_icmple&&
			rhs->type->isConst()){
		ConstType* ctype = ((ConstType*)rhs->type);
		if(util::strToNum<jint>(ctype->str)==0){
			opcode = opcode +(ByteCodes::ifeq-ByteCodes::if_icmpeq);

		}

	}else if(ByteCodes::if_acmpeq<=opcode && opcode<=ByteCodes::if_acmpne&&Check::isNull(rhs)){
		//对象判断null
		opcode = opcode +(ByteCodes::if_acmp_null-ByteCodes::if_acmpeq);

	}else{
		//
		Type* rtype = mt->argtypes[1];
		//把移位数由long换成int
		if(opcode>=ByteCodes::ishll && opcode <=ByteCodes::lushrl){
			opcode = opcode +(ByteCodes::ishl-ByteCodes::ishll);
			rtype = syms->intType;
		}
		//生成第二操作数
		genExpr(rhs,rtype)->load();
		//在symtab中，有的操作注册了两种指令，高8位（opcode1）|低8位（opcode2），现在取第一个
		if(opcode>=(1<<ByteCodes::preShift)){

			//用第一个指令，高8位
			code->emitop0(opcode >>ByteCodes::preShift);
			opcode = opcode & 0xFF;
		}
		//如果是比较跳转指令，暂时不输出指令，等待后续回填,if-else 会调用对应jumptrue或jumpfalse
		if ((ByteCodes::ifeq <= opcode && opcode <= ByteCodes::if_acmpne)
				|| opcode == ByteCodes::if_acmp_null
				|| opcode == ByteCodes::if_acmp_nonnull) {
			return items->makeCondItem(opcode,NULL,NULL);
		}else{
			code->emitop0(opcode);
			return items->makeStackItem(mt->restype);

		}


	}



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





