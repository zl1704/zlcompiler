/*
 * Gen.cpp
 *
 *  Created on: 2018年1月5日
 *      Author: ZL
 */

#include "Gen.hpp"
int* Gen::genKey = new int;
Gen* Gen::instance() {
	Gen* gen = (Gen*) Context::get(genKey);
	if (gen == NULL) {
		gen = new Gen();
		Context::put(genKey, gen);
	}
	return gen;
}

Gen::Gen() {
	Context::put(genKey, this);
	log = Log::instance();
	syms = Symtab::instance();
	chk = Check::instance();
	vector<Type*> nullVector;
	methodType = new MethodType(nullVector, NULL, syms->methodClass);
	attrEnv = NULL;
	toplevel = NULL;
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
void Gen::genDef(Tree* tree, Env<GenContext*>* env) {
	Env<GenContext*>* preEnv = this->env;
	this->env = env;
	tree->accept(this);
	this->env = preEnv;

}
void Gen::genStat(Tree* tree, Env<GenContext*>* env) {
	if (code->isAlive()) {
		genDef(tree, env);
	} else if (env->info->isSwitch && tree->getTag() == Tree::VARDEF) {
		//switch中定义的变量 ， 要在局部变量表中分配位置，c++中是不支持这种语法的
		code->newLocalVar(((VariableDecl*) tree)->sym);
	}

}
template<class T>
void Gen::genStats(vector<T> trees, Env<GenContext*>* env) {

	for (int i = 0; i < trees.size(); i++)
		genStat(trees[i], env);

}
void Gen::genArgs(vector<Expression*> trees, vector<Type*> ts) {
	for (int i = 0; i < trees.size(); i++)
		genExpr(trees[i], ts[i])->load();
}
/**
 * 生成条件表达项，返回值为true,false
 */
CondItem* Gen::genCond(Tree* tree) {
	Pretty::debug("\nGen::genCond: \t", tree);
	Tree* innerTree = Check::skipParens(tree);
	if (innerTree->getTag() == Tree::CONDEXPR) {
		// (cond)?(1):(2)
		Conditional* ctree = (Conditional*) innerTree;
		CondItem* cond = genCond(ctree->cond);
		//确定真值情况
		if (cond->isTrue()) {
			code->resolve(cond->trueJumps);
			return genCond(ctree->truepart);
		}
		if (cond->isFalse()) {
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
		CondItem* result = items->makeCondItem(second->opcode,
				Chain::merge(trueJumps, second->trueJumps),
				Chain::merge(falseJumps, second->falseJumps));
		return result;
	} else {
		return genExpr(tree, syms->booleanType)->mkCond();
	}

}
/**
 * 为表达式生成代码，判断常量的情况
 */
Item* Gen::genExpr(Tree* tree, Type* pt) {
	Type* prept = this->pt;
	if (tree->type->isConst()) {
		result = items->makeImmediateItem((ConstType*) tree->type);
	} else {
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

	Env<GenContext*>* loopEnv = env->dup(loop, new GenContext());
	int startpc = code->entryPoint();
	if (testFirst) {
		//while ,for
		CondItem* c;
		if (cond != NULL) {
			code->statBegin(cond->pos);
			c = genCond(Check::skipParens(cond));
		} else {
			//没有条件直接goto  无test的for
			c = items->makeCondItem(ByteCodes::goto_, NULL, NULL);
		}
		//false跳转,会生成一个跳转指令
		Chain* loopDone = c->jumpFalse();
		//true label
		code->resolve(c->trueJumps);
		genStat(body, loopEnv);
		//处理continue
		code->resolve(loopEnv->info->cont);
		//step ,for循环会有
		genStats(step, loopEnv);
		//生成跳转指令，跳转到循环start处
		code->resolve(code->branch(ByteCodes::goto_), startpc);
		//回填false链，跳出循环
		code->resolve(loopDone);
	} else {
		//do-while
		//body
		genStat(body, loopEnv);
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
			c = items->makeCondItem(ByteCodes::goto_, NULL, NULL);
		}

		//回填
		code->resolve(c->jumpTrue(), startpc);
		code->resolve(c->falseJumps);
	}
	//处理循环中的break,exit保存在env中
	code->resolve(loopEnv->info->exit);
}

/**
 * 生成code
 *
 */
void Gen::genMethod(MethodDecl* md, Env<GenContext*>* env) {
	if (debug)
		cout << "\nGen::genMethod:\t " << md->name << endl;
	int startpc = initCode(md, env);
	genStat(md->body, env);
	if (code->isAlive()) {
		code->statBegin(md->body->endPos);
		//生成return指令
		if (env->enclMethod == NULL
				|| ((MethodType*) (env->enclMethod->sym->type))->restype->tag
						== TypeTags::VOID) {
			code->emitop0(ByteCodes::return_);
		}
	}
	code->endScopes(0);

}

/**
 * 准备code数据
 * this ， 参数变量，入口pc
 */
int Gen::initCode(MethodDecl* md, Env<GenContext*>* env) {

	md->sym->code = code = new Code(md->sym, pool);
	items = new Items(pool, code);
	//给非静态方法添加this和参数到局部变量表中
	//此方法为非静态方法，构造函数为非静态方法
	if ((md->sym->flags_field & Flags::STATIC) == 0) {

		code->setDefined(
				code->newLocalVar(
						new VarSymbol(Flags::FINAL, Name::_this,
								md->sym->owner->type, md->sym->owner)));
	}
	//添加参数到局部变量表中
	for (int i = 0; i < md->params.size(); i++) {
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
void Gen::genClass(Env<AttrContext*>* env, ClassDecl* cdef) {

	if (util::debug)
		cout << "\n=========Gen Start!  " << endl;

	//defs只剩方法，在write阶段变量从 classsym中符号表中找

	cdef->defs = fillInitDefs(cdef->defs, cdef->sym);
	Env<GenContext*> * localEnv = new Env<GenContext*>(cdef, new GenContext());
	pool = new Pool();
	for (int i = 0; i < cdef->defs.size(); i++) {
		genDef(cdef->defs[i], localEnv);

	}

}

vector<Tree*> Gen::fillInitDefs(vector<Tree*> defs, ClassSymbol* csym) {
	vector<Statement*> initCode;
	vector<Statement*> cinitCode;
	vector<Tree*> methodDefs;

	for (int i = 0; i < defs.size(); i++) {
		Tree* def = defs[i];
		Block* block;
		VariableDecl* vdef;
		VarSymbol* vsym;
		switch (def->getTag()) {
		case Tree::BLOCK:
			block = (Block*) def;
			//静态构造块
			if (block->isStatic()) {
				util::appendList(cinitCode, block->stats);
			} else {
				util::appendList(initCode, block->stats);
			}
			break;

		case Tree::VARDEF:
			vdef = (VariableDecl*) def;
			vsym = vdef->sym;
			if (vdef->init != NULL) {
				//实例变量总是要初始化
				if ((vsym->flags_field & Flags::STATIC) == 0) {
					Statement* init = TreeMaker::makeAssignment(vdef->pos, vsym,
							vdef->init);
					init->type = vsym->type;
					initCode.push_back(init);
//					cout << "init code size :" << initCode.size()<<endl;
				} else if (!vsym->type->isConst()) {
					//非静态编译期常量
					Statement* init = TreeMaker::makeAssignment(vdef->pos, vsym,
							vdef->init);
					init->type = vsym->type;
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
	if (initCode.size() != 0) {
		for (int i = 0; i < methodDefs.size(); i++) {
			fillMethod((MethodDecl*) methodDefs[i], initCode);
		}
	}
	//填充静态初始化代码
	if (cinitCode.size() != 0) {
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
void Gen::fillMethod(MethodDecl* md, vector<Statement*> initCode) {

	if (md->name.compare(Name::init) == 0) {
		vector<Statement*> stats = md->body->stats;
		vector<Statement*> newstats;
		//添加super()调用 ，暂时不支持继承，需要在Enter中完成对构造方法的填充
		//...
		//initcode
		util::appendList(newstats, initCode);
		//构造方法中的代码
		util::appendList(newstats, stats);

		md->body->stats = newstats;

	}

}

int Gen::zero(int tc) {

	switch (tc) {
	case ByteCodes::INTcode:
	case ByteCodes::BYTEcode:
	case ByteCodes::SHORTcode:
	case ByteCodes::CHARcode:
		return ByteCodes::iconst_0;
	case ByteCodes::LONGcode:
		return ByteCodes::lconst_0;
	case ByteCodes::FLOATcode:
		return ByteCodes::fconst_0;
	case ByteCodes::DOUBLEcode:
		return ByteCodes::dconst_0;
	default:
		return -1;
	}
}
//间接项
int Gen::makeRef(Type* type) {
	if (type->tag == TypeTags::CLASS)
		return pool->put(type->tsym, Pool::POOL_SYM);
	return pool->put(type, Pool::POOL_TYPE);

}

void Gen::visitMethodDef(MethodDecl* tree) {
	Env<GenContext*> * localEnv = env->dup(tree);
	localEnv->enclMethod = tree;
	this->pt = ((MethodType*) (tree->sym->type))->restype;
	genMethod(tree, localEnv);
	//delete localEnv;
}
void Gen::visitVarDef(VariableDecl* tree) {
	Pretty::debug("\nGen::visitVarDef:\n", tree, 4);
	VarSymbol* v = tree->sym;
	code->newLocalVar(v);
	//右值不为空，生成赋值语句
	if (tree->init != NULL) {
		genExpr(tree->init, v->type)->load();
		items->makeLocalItem(v)->store();
	}
}

void Gen::visitBlock(Block* tree) {
	int limit = code->nextreg;
	Env<GenContext*>* localEnv = env->dup(tree);
	genStats(tree->stats, localEnv);
	//delete localEnv;

	if (env->tree->getTag() != Tree::METHODDEF) {
		//method的block在genMethod完成
		code->statBegin(tree->endPos);
		code->endScopes(limit);
		code->pendingStatPos = -1;

	}

}
void Gen::visitDoLoop(DoWhileLoop* tree) {
	vector<ExpressionStatement*> step;
	genLoop(tree, tree->body, tree->cond, step, false);
}
void Gen::visitWhileLoop(WhileLoop* tree) {
	Pretty::debug("\nGen::visitWhileLoop:\n", tree, 4);
	vector<ExpressionStatement*> step;
	genLoop(tree, tree->body, tree->cond, step, true);
}
void Gen::visitForLoop(ForLoop* tree) {
	Pretty::debug("\nGen::visitForLoop:\n", tree, 4);
	int limit = code->nextreg;
	genStats(tree->init, env);
	genLoop(tree, tree->body, tree->cond, tree->step, true);
	code->endScopes(limit);
}

void qsort2(int* keys, int* values, int lo, int hi) {
	int i = lo;
	int j = hi;
	int pivot = keys[(i + j) / 2];
	do {
		while (keys[i] < pivot)
			i++;
		while (pivot < keys[j])
			j--;
		if (i <= j) {
			int temp1 = keys[i];
			keys[i] = keys[j];
			keys[j] = temp1;
			int temp2 = values[i];
			values[i] = values[j];
			values[j] = temp2;
			i++;
			j--;
		}
	} while (i <= j);
	if (lo < j)
		qsort2(keys, values, lo, j);
	if (i < hi)
		qsort2(keys, values, i, hi);
}

void Gen::visitSwitch(Switch* tree) {
	Pretty::debug("\nGen::visitSwitch:\n", tree, 4);
	int limit = code->nextreg;
	Item* sel = genExpr(tree->selector, syms->intType);
	if (tree->cases.empty()) {
		//	无case情况
		/**
		 *   switch(sel){
		 *   }
		 */
		sel->load()->drop();
	} else {

		sel->load();
		Env<GenContext*>* switchEnv = new Env<GenContext*>(tree,
				new GenContext());
		switchEnv->info->isSwitch = true;

		int lo = 0x7fffffff;
		int hi = 0x80000000;
		int nlabels = 0;
		int defaultIndex = -1;
		vector<Case*> cases = tree->cases;

		int clen = cases.size();
		int labels[clen];
		int offset[clen];

		for (unsigned i = 0; i < clen; ++i) {
			if (cases[i]->pat != NULL) {
				//case
				int val = util::strToNum<jint>(
						((ConstType*) cases[i]->pat->type)->str);
				labels[i] = val;
				//找最大值最小值
				if (val < lo)
					lo = val;
				if (hi < val)
					hi = val;
				nlabels++;

			} else {
				//default:

				defaultIndex = i;
			}

		}
		//决定使用tableswitch  ,lookupswitch
		long table_space_cost = 4 + ((long) hi - lo + 1); // words
		long table_time_cost = 3; // comparisons
		long lookup_space_cost = 3 + 2 * (long) nlabels;
		long lookup_time_cost = nlabels;
		int opcode = -1;
		if (nlabels > 0
				&& table_space_cost + 3 * table_time_cost
						<= lookup_space_cost + 3 * lookup_time_cost)
			opcode = ByteCodes::tableswitch;
		else
			opcode = ByteCodes::lookupswitch;
		int startpc = code->curPc();
		code->emitop0(opcode);
		code->align(4);
		int tableBase = code->curPc();
		code->emit4(-1); //default  等待回填
		if (opcode == ByteCodes::tableswitch) {
			/**
			 * tableswitch:
			 * 	tableswitch
			 * 	default
			 * 	from to
			 * 	case1
			 * 	case2
			 * 	....
			 *
			 */
			code->emit4(lo);
			code->emit4(hi);
			for (int i = 0; i < clen; ++i) {
				code->emit1(-1);
			}
		} else {

			/**
			 * lookupswitch:
			 * 		lookupswitch
			 * 		default
			 * 		nlabes
			 * 		case1  label1
			 * 		case2  label2
			 *		...		...
			 *
			 */
			code->emit4(nlabels);
			for (unsigned i = 0; i < clen; ++i) {
				code->emit1(-1);
				code->emit1(-1);
			}
		}

		for (int i = 0; i < clen; ++i) {

			int pc = code->entryPoint();
			if (i != defaultIndex) {
				if (opcode == ByteCodes::tableswitch) {
					code->put4(tableBase + 4 * (labels[i] - lo + 3),
							pc - startpc);
					offset[i] = pc;

				} else {
					//待排序
					offset[i] = pc - startpc;
				}
			} else {

				code->put4(tableBase, pc - startpc);

			}
			genStats(cases[i]->stats, switchEnv);
		}
		//回填break跳转,pendjumps会在下个emitop之前处理
		code->resolve(switchEnv->info->exit);

		//设置默认default,switch中没有default
		if (code->get4(tableBase) == -1)
			code->put4(tableBase, code->entryPoint() - startpc);

		//debug

		if (opcode == ByteCodes::tableswitch) {
			//未填充的都未default
			int defaultOffset = code->get4(tableBase);
			for (int i = lo; i <= hi; i++) {
				int t = (int) (tableBase + 4 * (i - lo + 3));
				if (code->get4(t) == -1)
					code->put4(t, defaultOffset);
			}

		} else {
			//排序非default的情况
			if (defaultIndex > 0) {
				for (int i = defaultIndex; i < clen - 1; i++) {
					labels[i] = labels[i + 1];
					offset[i] = offset[i + 1];
				}
			}

			if (nlabels > 0)
				qsort2(labels, offset, 0, nlabels - 1);
			for (int i = 0; i < nlabels; i++) {
				int caseidx = tableBase + 8 * (i + 1);
				code->put4(caseidx, labels[i]);
				code->put4(caseidx + 4, offset[i]);

			}

		}

		if (util::debug) {
			cout << " \tswitch table:{\n";
			int defaultOffset = code->get4(tableBase);
			if (opcode == ByteCodes::tableswitch) {
				for (int i = lo, j = 0; i <= hi; i++) {
					int t = (int) (tableBase + 4 * (i - lo + 3));
					if (i != defaultIndex) {
						printf("%15d: %d\n", i, offset[j]);
						j++;
					} else if (code->get4(t) != -1) {
						printf("%15d: %d\n", i, defaultOffset + startpc);
					}

				}

			} else {
				for (int i = 0; i < nlabels; i++) {
					if (i != defaultIndex) {
						printf("%15d: %d\n", labels[i], offset[i]+startpc);
					}

				}

			}
			printf("%15s: %d\n", "default", defaultOffset + startpc);
			cout << " \t}\n";
		}
	}

	code->endScopes(limit);
}

/**
 * (cond)?(1):(2)
 * 按if-else翻译
 */
void Gen::visitConditional(Conditional* tree) {

	Pretty::debug("\nGen::visitConditional :\t", tree, 4);
	CondItem* cond = genCond(tree->cond);
	Chain* elseChain = cond->jumpFalse();
	//表达式出口
	Chain* thenExit = NULL;
	//为false,无需生成(1)的代码
	if (!cond->isFalse()) {
		code->resolve(cond->trueJumps);
		int startpc = code->curPc();
		genExpr(tree->truepart, pt)->load();
		code->state->forceStackTop(tree->type);
		thenExit = code->branch(ByteCodes::goto_);
	}
	if (elseChain != NULL) {
		code->resolve(elseChain);
		int startpc = code->curPc();
		genExpr(tree->falsepart, pt)->load();
		code->state->forceStackTop(tree->type);
	}
	code->resolve(thenExit);
	result = items->makeStackItem(pt);
}

/**
 * if(cond){
 * 		then
 * 		goto exit;
 * }else{
 *
 *
 *
 * }
 */
void Gen::visitIf(If* tree) {
	Pretty::debug("\nGen::visitIf:\n", tree, 4);
	Chain* thenExit = NULL;
	int limit = code->nextreg;
	CondItem* cond = genCond(tree->cond);
	Chain* elseChain = cond->jumpFalse();
	if (!cond->isFalse()) {
		code->resolve(cond->trueJumps);
		genStat(tree->thenpart, env);
		thenExit = code->branch(ByteCodes::goto_);
	}

	if (elseChain != NULL) {
		code->resolve(elseChain);
		if (tree->elsepart != NULL)
			genStat(tree->elsepart, env);
	}
	code->resolve(thenExit);
	code->endScopes(limit);
}
void Gen::visitExec(ExpressionStatement* tree) {
	Pretty::debug("\nGen::visitExec: \t", tree);
	//后缀转前缀  ，x++;为一条语句才能转，优化
	switch (tree->expr->getTag()) {
	case Tree::POSTINC:
		((Unary*) tree->expr)->setTag(Tree::PREINC);
		break;
	case Tree::POSTDEC:
		((Unary*) tree->expr)->setTag(Tree::PREINC);
		break;

	}
	/*
	 * Assign::drop:执行 lhs.store();
	 */
	genExpr(tree->expr, tree->expr->type)->drop();
}
void Gen::visitBreak(Break* tree) {
	env->info->addExit(code->branch(ByteCodes::goto_));
}
void Gen::visitContinue(Continue* tree) {
	env->info->addCont(code->branch(ByteCodes::goto_));
}
void Gen::visitReturn(Return* tree) {
	//有返回值
	if (tree->expr != NULL) {
		genExpr(tree->expr, tree->expr->type)->load();
		code->emitop0(
				ByteCodes::ireturn
						+ Code::truncate(
								ByteCodes::typecode(tree->expr->type)));
	} else {
		code->emitop0(ByteCodes::return_);
	}
}
void Gen::visitApply(MethodInvocation* tree) {
	Pretty::debug("\nGen::visitApply:\t", tree, 4);
	Item* m = genExpr(tree->meth, methodType);
	genArgs(tree->args,
			((MethodType*) Check::symbol(tree->meth)->type)->argtypes);
	result = m->invoke();
}

/**
 * new A(123);
 */
void Gen::visitNewClass(NewClass* tree) {
	//new 返回对象指针
	code->emitop2(ByteCodes::new_, makeRef(tree->type));
	//复制，作为构造函数参数
	code->emitop0(ByteCodes::dup);
	//生成参数
	genArgs(tree->args, ((MethodType*) tree->constructor->type)->argtypes);

	//调用 构造方法，invokespecial
	items->makeMemberItem(tree->constructor, false)->invoke();
	result = items->makeStackItem(tree->type);

}
void Gen::visitNewArray(NewArray* tree) {
	if (tree->elems.size() != 0) {
		//int a[] = {}

	} else {
		//gen dims
		vector<Expression*> _dims = tree->dims;
		for (int i = 0; i < _dims.size(); i++) {
			genExpr(_dims[i], syms->intType)->load();
		}
		// new
		//去掉一层数组
		int dims = tree->dims.size();
		Type* elemType = Type::elemtype(tree->type);
		int elemcode = Code::arraycode(elemType);
		//A[]
		if (elemcode == 0 || (elemcode == 1 && dims == 1)) {

			code->emitAnewarray(makeRef(elemType), tree->type);
		} else if (elemcode == 1) {
			//多维
			code->emitMultianewarray(dims, makeRef(elemType), tree->type);
		} else {
			//基本类型
			code->emitNewarray(elemcode, tree->type);

		}

	}
	result = items->makeStackItem(tree->type);
}
void Gen::visitParens(Parens* tree) {
	result = genExpr(tree->expr, tree->expr->type);
}
void Gen::visitAssign(Assign* tree) {
	Pretty::debug("\nGen::visitAssign:\t", tree, 4);
	Item* l = genExpr(tree->lhs, tree->lhs->type);
	genExpr(tree->rhs, tree->rhs->type)->load();
	result = items->makeAssinItem(l);
	//由Exec store

}
/**
 * ===>  a = a + b;
 */
void Gen::visitAssignop(AssignOp* tree) {
	OperatorSymbol* opsym = (OperatorSymbol*) tree->opsym;
	Item* l;
	if (opsym->opcode == ByteCodes::string_add) {

	} else {
		l = genExpr(tree->lhs, tree->lhs->type);
		l->load();
		//复制一个与右操作数运算
		//????
		l->duplicate();
		l->coerce(((MethodType*) opsym->type)->argtypes[0])->load();
		//完成二元操作
		completeBinop(tree->lhs, tree->rhs, opsym)->coerce(tree->lhs->type);
	}
	result = items->makeAssinItem(l);
}

/**
 * 主要是 ++x ,x++运算
 */
void Gen::visitUnary(Unary* tree) {
	Pretty::debug("\nGen::visitUnary: \t", tree);

	OperatorSymbol* opsym = (OperatorSymbol*) tree->sym;
	MethodType* mt = (MethodType*) opsym->type;
	if (tree->opcode == Tree::NOT) {
		// ! 逻辑运算
		CondItem* cond = genCond(tree->arg);
		result = cond->negate();
	} else {
		//算数运算
		Item* item = genExpr(tree->arg, mt->argtypes[0]);

		if (tree->getTag() == Tree::NEG) {
			result = item->load();
			code->emitop0(opsym->opcode);
		} else if (tree->getTag() == Tree::COMPL) {
			result = item->load();
			//! 转成  ===>  -1 异或 od
			if (item->typecode == ByteCodes::LONGcode) {
				items->makeImmediateItem(
						ConstType::create(TypeTags::LONG, "-1"))->load();
			} else {
				code->emitop0(ByteCodes::iconst_m1);
			}
			code->emitop0(opsym->opcode);
		} else if (tree->getTag() == Tree::PREINC
				|| tree->getTag() == Tree::PREDEC) {
			item->duplicate();
			//int类型自增  通过iinc指令,直接给局部变量int +1
			if (dynamic_cast<LocalItem*>(item)
					&& (opsym->opcode == ByteCodes::iadd
							|| opsym->opcode == ByteCodes::isub)) {
				((LocalItem*) item)->incr(
						tree->getTag() == Tree::PREINC ? 1 : -1);
				result = item;
				//后面通过load()来使用
			} else {
				//非int
				// load() ,add ,store()
				item->load();
				code->emitop0(zero(item->typecode) + 1);
				code->emitop0(opsym->opcode);

				//char byte short 需要转型
				if (item->typecode != ByteCodes::INTcode
						&& (Code::truncate(item->typecode)))
					code->emitop0(
							ByteCodes::int2byte + item->typecode
									- ByteCodes::BYTEcode);

				//item->store();
				//这里是assign 后续会调用drop()
				result = items->makeAssinItem(item);
			}

		} else if (tree->getTag() == Tree::POSTINC
				|| tree->getTag() == Tree::POSTDEC) {
			item->duplicate();
			//后缀运算 x++, 先将x load()到栈中
			if (dynamic_cast<LocalItem*>(item)
					&& (opsym->opcode == ByteCodes::iadd
							|| opsym->opcode == ByteCodes::isub)) {
				Item* res = item->load();
				((LocalItem*) item)->incr(
						tree->getTag() == Tree::POSTINC ? 1 : -1);
				result = res;
				//后面通过load()来使用
			} else {
				//非int
				// load() ,add ,store()
				Item* res = item->load();
//				code->state->print();
				//数组运算会用，因为是后缀，x = a[i]++ ,先将a[i]复制到数组操作之下，以便数组+1操作完之后将原来a[i]赋值给x
				item->stash(item->typecode);
				code->emitop0(zero(item->typecode) + 1);
				code->emitop0(opsym->opcode);

				//char byte short 需要转型
				if (item->typecode != ByteCodes::INTcode
						&& (Code::truncate(item->typecode)))
					code->emitop0(
							ByteCodes::int2byte + item->typecode
									- ByteCodes::BYTEcode);

				item->store();
				result = res;
			}

		} else if (tree->getTag() == Tree::NULLCHK) {

		} else {

			cout << "Gen::visitUnary :未定义的一元操作" << endl;
		}

	}

}

void Gen::visitBinary(Binary* tree) {
	OperatorSymbol* opsym = ((OperatorSymbol*) tree->opsym);
	/**
	 * string add情况
	 * 常量折叠已经在attr阶段处理
	 * 非字面字符串相加暂时不处理，后续版本更新
	 */
	if (opsym->opcode == ByteCodes::string_add) {

	} else if (tree->type->tag == Tree::AND) {
		//&& 条件
		CondItem* lcond = genCond(tree->lhs);
		if (!lcond->isFalse()) {
			Chain* falseJumps = lcond->jumpFalse();
			code->resolve(lcond->trueJumps);
			CondItem* rcond = genCond(tree->rhs);

			result = items->makeCondItem(rcond->opcode, rcond->trueJumps,
					Chain::merge(falseJumps, rcond->falseJumps));

		} else {
			//常量false情况,短路机制
			result = lcond;
		}
	} else if (tree->type->tag == Tree::OR) {
		//思路同AND
		CondItem* lcond = genCond(tree->lhs);
		if (!lcond->isTrue()) {
			Chain* trueJumps = lcond->jumpTrue();
			code->resolve(lcond->falseJumps);
			CondItem* rcond = genCond(tree->rhs);

			result = items->makeCondItem(rcond->opcode,
					Chain::merge(trueJumps, rcond->trueJumps),
					rcond->falseJumps);

		} else {
			result = lcond;
		}

	} else {
		Item* item = genExpr(tree->lhs,
				((MethodType*) opsym->type)->argtypes.at(0));
		item->load();
		result = completeBinop(tree->lhs, tree->rhs, opsym);
	}

}
Item* Gen::completeBinop(Tree* lhs, Tree* rhs, OperatorSymbol* opsym) {
	MethodType* mt = (MethodType*) opsym->type;
	int opcode = opsym->opcode;
	//int与0比较 直接转换为比较跳转指令(153~158)
	if (ByteCodes::if_icmpeq <= opcode && opcode <= ByteCodes::if_icmple
			&& rhs->type->isConst()) {
		ConstType* ctype = ((ConstType*) rhs->type);
		if (util::strToNum<jint>(ctype->str) == 0) {
			opcode = opcode + (ByteCodes::ifeq - ByteCodes::if_icmpeq);

		}

	} else if (ByteCodes::if_acmpeq <= opcode && opcode <= ByteCodes::if_acmpne
			&& Check::isNull(rhs)) {
		//对象判断null
		opcode = opcode + (ByteCodes::if_acmp_null - ByteCodes::if_acmpeq);

	} else {
		//
		Type* rtype = mt->argtypes[1];
		//把移位数由long换成int
		if (opcode >= ByteCodes::ishll && opcode <= ByteCodes::lushrl) {
			opcode = opcode + (ByteCodes::ishl - ByteCodes::ishll);
			rtype = syms->intType;
		}
		//生成第二操作数
		genExpr(rhs, rtype)->load();
		//在symtab中，有的操作注册了两种指令，高8位（opcode1）|低8位（opcode2），现在取第一个
		if (opcode >= (1 << ByteCodes::preShift)) {

			//用第一个指令，高8位
			code->emitop0(opcode >> ByteCodes::preShift);
			opcode = opcode & 0xFF;
		}

	}

	//如果是比较跳转指令，暂时不输出指令，等待后续回填,if-else 会调用对应jumptrue或jumpfalse
	if ((ByteCodes::ifeq <= opcode && opcode <= ByteCodes::if_acmpne)
			|| opcode == ByteCodes::if_acmp_null
			|| opcode == ByteCodes::if_acmp_nonnull) {
		return items->makeCondItem(opcode, NULL, NULL);
	} else {
		//运算结果在栈顶
		code->emitop0(opcode);
		return items->makeStackItem(mt->restype);

	}

}

void Gen::visitTypeCast(TypeCast* tree) {
}
void Gen::visitTypeTest(InstanceOf* tree) {
}
void Gen::visitIndexed(ArrayAccess* tree) {

	genExpr(tree->indexed, tree->indexed->type)->load();
	genExpr(tree->index, syms->intType)->load();
	result = items->makeIndexItem(tree->type);
}

void Gen::visitSelect(FieldAccess* tree) {
	Symbol* sym = tree->sym;
	//.class
	if (tree->name.compare(Name::_class) == 0) {
		code->emitop2(ByteCodes::ldc2, makeRef(tree->selected->type));
		result = items->makeStackItem(pt);
		return;
	}

	//父类 super调用情况
	//Item *base = makeSuperItem();

	Item* base = genExpr(tree->selected, tree->selected->type);
	//判断常量情况 final
	if (sym->kind == Kinds::VAR && ((VarSymbol*) sym)->type->isConst()) {

		result = items->makeImmediateItem((ConstType*) sym->type);
	} else {
		//静态
		if ((sym->flags_field & Flags::STATIC) != 0) {
			//get/putstatic 不需要对象
			base->drop();
			result = items->makeStaticItem(sym);

		} else {
			base->load();
			if (sym == syms->lengthVar) {
				code->emitop0(ByteCodes::arraylength);
				result = items->makeStackItem(syms->intType);
			} else {
				result = items->makeMemberItem(sym,
						(sym->flags_field & Flags::PRIVATE) == 0);
			}
		}

	}

}

void Gen::visitIdent(Ident* tree) {
	Symbol* sym = tree->sym;
	//this ,super
	if (sym->name == Name::_this || tree->name == Name::super) {
		Item* res =
				tree->name == Name::_this ?
						items->makeThisItem() : items->makeSuperItem();
		//构造方法调用
		if (sym->kind == Kinds::MTH) {
			res->load();
			res = items->makeMemberItem(sym, false);	//special

		}
		result = res;
	} else if (sym->kind == Kinds::VAR && sym->owner->kind == Kinds::MTH) {
		//局部变量
		result = items->makeLocalItem((VarSymbol*) sym);
	} else if ((sym->flags_field & Flags::STATIC) != 0) {
		//静态
		result = items->makeStaticItem(sym);
	} else {
		//成员，父类
		items->makeThisItem()->load();
		/**
		 * 是否要判断变量所在位置？在Attr阶段完成？
		 * private :invokespecial
		 */
		result = items->makeMemberItem(sym,
				(sym->flags_field & Flags::PRIVATE) == 0);

	}

}
void Gen::visitLiteral(Literal* tree) {
	//null
	if (tree->getTag() == TypeTags::BOT) {
		code->emitop0(ByteCodes::aconst_null);
		result = items->makeStackItem(tree->type);
	} else {
		//其他立即数
		result = items->makeImmediateItem((ConstType *) tree->type);
		//genExpr()->load()  加载到栈中

	}
}

/**
 * not used
 */

void Gen::visitCase(Case* tree) {
}
void Gen::visitTree(Tree* tree) {
}

void Gen::visitSkip(Skip* tree) {
}
void Gen::visitTopLevel(CompilationUnit* tree) {
}

void Gen::visitClassDef(ClassDecl* tree) {

}
void Gen::visitTypeIdent(PrimitiveTypeTree* tree) {
}
void Gen::visitTypeArray(ArrayTypeTree* tree) {
}

void Gen::visitModifiers(Modifiers* tree) {
}

/**
 * GenContext
 */

GenContext::GenContext() {

	exit = NULL;
	cont = NULL;
	isSwitch = false;

}

void GenContext::addExit(Chain* c) {
	exit = Chain::merge(c, exit);
}
void GenContext::addCont(Chain* c) {
	cont = Chain::merge(c, cont);
}

