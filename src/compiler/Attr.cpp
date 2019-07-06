/*
 * Attr.cpp
 *
 *  Created on: 2018年1月2日
 *      Author: ZL
 */

#include "Attr.hpp"
int* Attr::attrKey = new int;

Attr* Attr::instance() {
	Attr* attr = (Attr*) Context::get(attrKey);
	if (attr == NULL){
		attr = new Attr();
	}
	return attr;

}

Attr::Attr() {
	Context::put(attrKey, this);
	syms = Symtab::instance();
	enter = Enter::instance();
	chk = Check::instance();
	log = Log::instance();
	cfolder = ConstFold::instance();
}

Attr::~Attr() {
}

Env<AttrContext*>* Attr::attribute(Env<AttrContext*>* env) {
	attribute(env->enclClass->sym);
	return env;
}
void Attr::attribute(ClassSymbol* c) {
	//需要支持继承 在这里提前attr 父类

	Env<AttrContext*> * env = enter->typeEnvs.find(c)->second;

	attriClassBody(env, c);
}
void Attr::attriClassBody(Env<AttrContext*>* env, ClassSymbol* c) {
	ClassDecl* tree = (ClassDecl*) env->tree;
	tree->type = c->type;
	env->info->scope = c->members_field;
	//标注class 成员属性
	for (int i = 0; i < tree->defs.size(); i++) {
		attribStat(tree->defs.at(i), env);
	}

}

Type* Attr::attribExpr(Expression* tree, Env<AttrContext*>* env, Type* pt) {
	return attribTree(tree, env, Kinds::VAL, pt);

}

vector<Type*> Attr::attribExprs(vector<Expression*> trees, Env<AttrContext*>* env,
		Type* pt) {
	vector<Type*> ts;
	for (int i = 0; i < trees.size(); i++)
		ts.push_back(attribExpr(trees.at(i), env, pt));
}
vector<Type*> Attr::attribArgs(vector<Expression*> trees,
		Env<AttrContext*>* env) {
	vector<Type*> types;
	for (int i = 0; i < trees.size(); i++) {
		types.push_back(
				chk->checkNonVoid(trees[i]->pos,
						attribTree(trees[i], env, Kinds::VAL, Type::noType)));
	}
	return types;
}

template<class T>
Type* Attr::attribStats(vector<T> trees, Env<AttrContext*>* env) {
	for (int i = 0; i < trees.size(); i++)
		attribStat(trees.at(i), env);
}
Type* Attr::attribStat(Tree* tree, Env<AttrContext*>* env) {

	return attribTree(tree, env, Kinds::NIL, Type::noType);

}

Type* Attr::attribType(Tree* tree, Env<AttrContext*>* env) {

	return attribType(tree, env, Type::noType);
}
Type* Attr::attribType(Tree* tree, Env<AttrContext*>* env, Type* pt) {
	return attribTree(tree, env, Kinds::TYP, pt);
}
Type* Attr::attribTree(Tree* tree, Env<AttrContext*>* env, int pkind,
		Type* pt) {

	Env<AttrContext*>* prevEnv = this->env;
	int prevkind = this->pkind;
	Type* ptype = this->pt;
	this->env = env;
	this->pt = pt;
	this->pkind = pkind;
	tree->accept(this);
	this->env = prevEnv;
	this->pt = ptype;
	this->pkind = prevkind;
	return result;

}

/**
 * 检查类型
 */
Type* Attr::check(Tree* tree, Type* owntype, Type* pt, int ownkind, int pkind,
		string errKey) {
	if (owntype && owntype->tag != TypeTags::ERROR
			&& pt->tag != TypeTags::METHOD) {
		//note
		if ((ownkind & ~pkind) == 0)
			owntype = chk->checkType(tree->pos,owntype,pt,errKey);
		else{
			log->report(tree->pos,Log::ERROR,errKey);

		}

	}
	tree->type = owntype;
	return owntype;
}
/**
 * 根据名称  参数类型 ， 所在范围 寻找方法
 */
Symbol* Attr::findMethod(int pos, Scope* s, string name,
		vector<Type*> argtypes,string className) {
	if (util::debug) {
		cout << "find method: \t" + name;
		s->print();
		cout << endl;
	}
	Scope* ps = s;
	//尝试查找参数完全匹配的方法
	while (s) {
		for (Entry* e = s->findLocal(name); e; e = e->next) {
			if (e->sym->kind == Kinds::MTH && e->sym->name == name) {
				MethodType* mt = ((MethodType*) e->sym->type);
				if (mt->argtypes.size() == argtypes.size()) {
					bool f = true;
					for (int i = 0; i < argtypes.size(); i++) {
						if (mt->argtypes[i]->tag != argtypes[i]->tag) {
							f = false;
							break;
						}
					}
					if (f) {
						return e->sym;
					}
				}

			}

		}
		s = s->next;
	}
	s = ps;
	//查找参数兼容的方法
	while (s) {
			for (Entry* e = s->findLocal(name); e; e = e->next) {
				if (e->sym->kind == Kinds::MTH && e->sym->name == name) {
					MethodType* mt = ((MethodType*) e->sym->type);
					if (mt->argtypes.size() == argtypes.size()) {
						bool f = true;
						for (int i = 0; i < argtypes.size(); i++) {
							if (chk->isAssignable( argtypes[i],mt->argtypes[i])) {
								f = false;
								break;
							}
						}
						if (f) {
							return e->sym;
						}
					}

				}

			}
			s = s->next;
		}

	string err ="";
	if(name == Name::init)
		err = "没有找到构造方法:" + className + "(";
	else
		err = "没有找到方法:" + name + "(";
	for (int i = 0; i < argtypes.size(); i++) {
		err += Type::typeName(argtypes[i]);
		if (i != argtypes.size() - 1)
			err += ",";
	}
	err += ")";
	log->report(pos, Log::ERROR, err);

	return syms->errSymbol;
}
Symbol* Attr::findField(int pos, Scope* s, string name,Type* t) {
	while (s) {
		for (Entry* e = s->findLocal(name); e; e = e->next) {
			if (e->sym->kind == Kinds::VAR && e->sym->name == name) {
				return e->sym;
			}
		}
		log->report(pos, Log::ERROR,Type::typeName(t) +"类中没有找到成员属性 " + name);
		return syms->errSymbol;
	}
}
/**
 * ident   type or var
 */
Symbol* Attr::findIdent(int pos, Env<AttrContext*>* env, string name,
		int kind) {
	Scope* s = env->info->scope;
	while (s) {
		for (Entry* e = s->findLocal(name); e; e = e->next) {
			if (e->sym->name == name && (e->sym->kind & kind) != 0)
				return e->sym;
		}
		s = s->next;
	}
	s = syms->rootPackage->members_field;
	while (s) {
		for (Entry* e = s->findLocal(name); e; e = e->next) {
			if (e->sym->name == name && (e->sym->kind & kind) != 0)
				return e->sym;
		}
		s = s->next;
	}
	log->report(pos, Log::ERROR, "没有找到符号  " + name);
	return syms->errSymbol;
}
Symbol* Attr::findType(Env<AttrContext*>* env, string name) {
	for (Env<AttrContext*>* env1 = env; env1; env1 = env->outer) {
		for (Entry* e = env1->info->scope->findLocal(name); e; e = e->next) {
			if (e->sym->kind == Kinds::TYP)
				return e->sym;
		}

	}
	return syms->errSymbol;
}
//Symbol* Attr::findVar(Env<AttrContext*>* env, string name) {
//
//}
/**
 * 标注 二元操作符类型，决定表达式类型
 */
Symbol* Attr::resolveBinaryOperator(int pos, Env<AttrContext*>* env, int optag,
		Type* left, Type* right) {
	if(util::debug){
		cout<< "寻找二元操作: " + Type::typeName(left) + "  "
						+ TreeInfo::operatorName(optag) + "   "
						+ Type::typeName(right) << endl;
	}
	Scope* s = syms->predefClass->members();
	//先找完全匹配
	for (Entry* e = s->find(TreeInfo::operatorName(optag)); e; e = e->next) {
		if (e->sym->name != TreeInfo::operatorName(optag))
			continue;
		OperatorSymbol* opsym = (OperatorSymbol*) e->sym;
//		cout<<"找到操作: ";
//		cout<<(opsym->opcode)<<endl;
		MethodType* mt = (MethodType*) opsym->type;
		if (mt->argtypes.size() == 2) {
			if (mt->argtypes.at(0)->tag == left->tag
					&& (mt->argtypes.at(1)->tag == right->tag)) {
				cout
						<< "找到二元操作: " + Type::typeName(left) + "  "
								+ TreeInfo::operatorName(optag) + "   "
								+ Type::typeName(right);
				cout
						<< "\t结果类型: "
								+ Type::typeName(
										((MethodType*) opsym->type)->restype->tag)
						<< endl;
				return opsym;
			}
		}
	}

	//参数兼容
	for (Entry* e = s->find(TreeInfo::operatorName(optag)); e; e = e->next) {
			if (e->sym->name != TreeInfo::operatorName(optag))
				continue;
			OperatorSymbol* opsym = (OperatorSymbol*) e->sym;
	//		cout<<"找到操作: ";
	//		cout<<(opsym->opcode)<<endl;
			MethodType* mt = (MethodType*) opsym->type;
			if (mt->argtypes.size() == 2) {
				//note!!! 判断需修改 ，最佳匹配
				if (chk->isAssignable(left,mt->argtypes.at(0))
						&& chk->isAssignable(right,mt->argtypes.at(1))) {
					cout
							<< "找到二元操作: " + Type::typeName(left) + "  "
									+ TreeInfo::operatorName(optag) + "   "
									+ Type::typeName(right);
					cout
							<< "\t结果类型: "
									+ Type::typeName(
											((MethodType*) opsym->type)->restype->tag)
							<< endl;
					return opsym;
				}
			}
		}
	string err = "没有找到匹配的运算    " + Type::typeName(left) + "  "
			+ TreeInfo::operatorName(optag) + " " + Type::typeName(right);
	log->report(pos, Log::ERROR, err);
	return syms->errSymbol;

}
/**
 * 标注一元操作
 */
Symbol* Attr::resolveUnaryOperator(int pos, int optag, Env<AttrContext*>* env,
		Type* argtype) {
	if(util::debug){
		cout
				<< "寻找一元操作: " + Type::typeName(argtype) + "  "
						+ TreeInfo::operatorName(optag) << endl;
	}
	Scope* s = syms->predefClass->members();
	for (Entry* e = s->findLocal(TreeInfo::operatorName(optag)); e; e =
			e->next) {
		if (e->sym->name != TreeInfo::operatorName(optag))
			continue;
		OperatorSymbol* opsym = (OperatorSymbol*) e->sym;
//			cout<<"找到操作: ";
//			cout<<(opsym->opcode)<<endl;
		MethodType* mt = (MethodType*) opsym->type;
		if (mt->argtypes.size() == 1) {
			if (mt->argtypes.at(0)->tag == argtype->tag) {
				if(util::debug){
					cout
							<< "找到一元操作: " + Type::typeName(argtype) + "  "
									+ TreeInfo::operatorName(optag);
					cout
							<< "\t结果类型: "
									+ Type::typeName(
											((MethodType*) opsym->type)->restype->tag)
						<< endl;
				}
				return opsym;
			}
		}
	}
	string err = "没有找到匹配的运算    " + Type::typeName(argtype) + "  "
			+ TreeInfo::operatorName(optag) + "   ";
	log->report(pos, Log::ERROR, err);
	return syms->errSymbol;

}

Symbol* Attr::selectSym(FieldAccess* tree, Symbol* sitesym, Type* site,
		Type* pt, int pkind) {
	if (site->tag == TypeTags::CLASS ||site->tag ==TypeTags::ARRAY) {
		string name = tree->name;
		int pos = tree->pos;
		Symbol* s;
		if (pt->tag == TypeTags::METHOD && name != Name::_this) {
			s = findMethod(pos,
					((ClassSymbol*) site->tsym)->members_field, name,
					((MethodType*) pt)->argtypes);
			if (s->type->tag == TypeTags::METHOD
					&& (env->enclClass->sym != site->tsym)) {
				long long flags = ((MethodSymbol*) s)->flags_field;
				if ((flags & Flags::PUBLIC) == 0)
					log->report(tree->pos, Log::ERROR, "无法访问非public方法");

			}
		} else if (name == Name::_this) {
			//构造方法调用
			s = findMethod(pos, env->info->scope, Name::init,
					((MethodType*) pt)->argtypes,((ClassSymbol*) site->tsym)->name);
//			if (s->type->tag == TypeTags::METHOD
//					&& (env->enclClass->sym != site->tsym)) {
//				long long flags = ((MethodSymbol*) s)->flags_field;
//				//这里肯定是自身调用，无需判断 public
////				if ((flags & Flags::PUBLIC) == 0)
////					log->report(tree->pos, Log::ERROR, "无法访问非public方法");
//
//			}
		} else {
			s = findField(pos,
					((ClassSymbol*) site->tsym)->members_field, name,site);
			if (s->kind == Kinds::VAR && (env->enclClass->sym != site->tsym)) {
				long long flags = ((VarSymbol*) s)->flags_field;
				if ((flags & Flags::PUBLIC) == 0)
					log->report(tree->pos, Log::ERROR, "无法访问非public属性");
			}
		}


		return s;
	} else {
		log->report(tree->pos, Log::ERROR, "类型不是class 对象,无法access成员");
		return syms->errSymbol;
	}

}

void Attr::visitTopLevel(CompilationUnit* tree) {

}
void Attr::visitClassDef(ClassDecl* tree) {

}
void Attr::visitMethodDef(MethodDecl* tree) {
	MethodSymbol* m = tree->sym;
	//方法的 env
	Env<AttrContext*>* localEnv = enter->methodEnv(tree, env);
	//已添加 跳过
	for (int i = 0; i < tree->params.size(); i++) {
		//添加方法参数 属性在enter阶段已经标注过，这里直接添加到scope中
		attribStat(tree->params.at(i), localEnv);
	}
	if(util::debug){
		cout << "method :" + tree->name << " enclscope: ";
		localEnv->info->scope->print();
		cout << endl;
	}

	//body
	attribStat(tree->body, localEnv);
	tree->type = m->type;
	result = tree->type;
}
void Attr::visitVarDef(VariableDecl* tree) {
	if (env->info->scope->owner->kind == Kinds::MTH) {
		if (tree->sym != NULL) {
			//解析过直接添加到scope
			env->info->scope->enter(tree->sym);
		} else {
			enter->memberEnter(tree, env);
		}
		tree->sym->flags_field  = env->enclMethod ? Flags::STATIC :tree->mods->flags;;
	}

	VarSymbol* v = tree->sym;
	//init
	if (tree->init) {
		Env<AttrContext*>* initEnv = enter->initEnv(tree, env);
		initEnv->info->enclVar = v;
		attribExpr(tree->init, initEnv, v->type);
		delete initEnv;
	}

	result = tree->type = v->type;
}
void Attr::visitSkip(Skip* tree) {
	result = NULL;
}
void Attr::visitBlock(Block* tree) {
	if (env->info->scope->owner->kind == Kinds::TYP) {
		//类初始化  或静态初始化块
		Env<AttrContext*>* localEnv = env->dup(tree,
				env->info->dup(env->info->scope->dup()));
		localEnv->info->scope->owner = new MethodSymbol(
				tree->flags | Flags::BLOCK, Name::empty, NULL,
				env->info->scope->owner);
//		if ((tree->flags & Flags::STATIC) != 0)
//			localEnv->info->staticLevel++;
		attribStats(tree->stats, localEnv);
	} else {
		Env<AttrContext*>* localEnv = env->dup(tree,
				env->info->dup(env->info->scope->dup()));
		attribStats(tree->stats, localEnv);
	}
	result = NULL;

}
void Attr::visitDoLoop(DoWhileLoop* tree) {
	attribStat(tree->body, env->dup(tree));
	attribExpr(tree->cond, env, syms->booleanType);
	result = NULL;
}
void Attr::visitWhileLoop(WhileLoop* tree) {
	attribExpr(tree->cond, env, syms->booleanType);
	attribStat(tree->body, env->dup(tree));
	result = NULL;
}
void Attr::visitForLoop(ForLoop* tree) {
	Env<AttrContext*>* loopEnv = env->dup(env->tree,
			env->info->dup(env->info->scope->dup()));
	attribStats(tree->init, loopEnv);
//	cout<<"for env:";
//	loopEnv->info->scope->print();
//	cout<<endl;
	if (tree->cond != NULL)
		attribExpr(tree->cond, loopEnv, syms->booleanType);
	loopEnv->tree = tree; // before, we were not in loop!
	attribStats(tree->step, loopEnv);
	attribStat(tree->body, loopEnv);
	result = NULL;
}

void Attr::visitSwitch(Switch* tree) {
	Type* seltype = attribExpr(tree->selector, env, Type::noType);
	Env<AttrContext*>* switchEnv = env->dup(tree,
			env->info->dup(env->info->scope->dup()));
	if (chk->sameType(seltype, syms->stringType)) {
		log->report(tree->pos, Log::ERROR, "switch 不支持字符串");
	}
	bool hasDefault = false;
	Case* c = NULL;
	for (int i = 0; tree->cases.size(); i++) {
		c = tree->cases.at(i);

		Env<AttrContext*>* caseEnv = switchEnv->dup(c,
				env->info->dup(switchEnv->info->scope->dup()));
		if (c->pat != NULL) {
			Type* pattype = attribExpr(c->pat, switchEnv, seltype);
		} else if (hasDefault) {
			log->report(tree->pos, Log::ERROR, "重复的default label");

		} else {
			hasDefault = true;
		}
		attribStats(c->stats, caseEnv);
		addVars(c->stats, switchEnv->info->scope);
	}
	result = NULL;
}
void Attr::addVars(vector<Statement*> stats, Scope* switchScope) {
	for (int i = 0; stats.size(); i++) {
		if (stats.at(i)->getTag() == Tree::VARDEF)
			switchScope->enter(((VariableDecl*) stats.at(i))->sym);
	}

}

void Attr::visitCase(Case* tree) {

}
/**
 * 三目运算符
 */
void Attr::visitConditional(Conditional* tree) {
	attribExpr(tree->cond, env, syms->booleanType);
	attribExpr(tree->truepart, env, Type::noType);
	attribExpr(tree->falsepart, env, Type::noType);
	check(tree, tree->cond->type, syms->booleanType, Kinds::VAL, pkind,
			"类型不兼容");
	result = check(tree, tree->truepart->type, tree->falsepart->type,
			Kinds::VAL, pkind, "三目运算结果类型不一致");
}

void Attr::visitIf(If* tree) {
	env->info->scope->print();
	cout << endl;
	attribExpr(tree->cond, env, syms->booleanType);
	attribStat(tree->thenpart, env);
	if (tree->elsepart != NULL)
		attribStat(tree->elsepart, env);
	chk->checkEmptyIf(tree);
	result = NULL;

}
void Attr::visitExec(ExpressionStatement* tree) {

	Env<AttrContext*>* localEnv = env->dup(tree);
	attribExpr(tree->expr, localEnv, Type::noType);
	result = NULL;
}
/**
 * 暂不支持break  label
 * 添加此功能事  需要从当前env 向上找到label
 * findtargetjump
 */
void Attr::visitBreak(Break* tree) {

	tree->target = NULL;
	result = NULL;
}
/**
 * 同break
 */
void Attr::visitContinue(Continue* tree) {
	tree->target = NULL;
	result = NULL;
}

void Attr::visitReturn(Return* tree) {
	MethodSymbol* m = env->enclMethod->sym;
	if (m->type->tag == TypeTags::VOID) {
		//void  return后没有值
		if (tree->expr) {
			log->report(tree->expr->pos, Log::ERROR, "void 方法无返回值");
		}
	} else if (tree->expr != NULL) {
		log->report(tree->expr->pos, Log::ERROR, "缺少返回值");
	} else
		attribExpr(tree->expr, env, ((MethodType*) m->type)->restype);

	result = NULL;
}
void Attr::visitApply(MethodInvocation* tree) {
	Env<AttrContext*>* localEnv = env->dup(tree, env->info->dup());
	string name = Check::treeName(tree->meth);
	vector<Type*> argtypes;
	argtypes = attribArgs(tree->args, localEnv);
	if (name == Name::_this) {
		//this调用

		MethodType* mpt = new MethodType(argtypes, NULL, syms->methodClass);
		Type* mtype = attribExpr(tree->meth, localEnv, mpt);
		if (env->enclMethod->sym->name != Name::init
				&& ((void *) env->enclMethod->body->stats.at(0))
						!= ((void *) tree))
			log->report(tree->pos, Log::ERROR, "构造方法调用只能出现在构造方法第一行");
		result = tree->type = syms->voidType;
	} else {
		//普通方法

		MethodType* mpt = new MethodType(argtypes, NULL, syms->methodClass);
		//a.fun() 先find a.fun ,传mpt找到methodsym
		Type* mtype = attribExpr(tree->meth, localEnv, mpt);

		result = check(tree, ((MethodType*) mtype)->restype, pt, Kinds::VAL,
				pkind, "类型不兼容");
	}

}
void Attr::visitNewClass(NewClass* tree) {

	Expression* clazz = tree->clazz;
	string className = "";
	if(dynamic_cast<Ident* >(clazz))
		className = ((Ident*)clazz)->name;
	Type* clazzType = attribType(clazz, env);
	//准备寻找构造方法
	Scope* s= clazz->type->tsym->members();

	vector<Type*> argtypes = attribArgs(tree->args,env);
	if(clazzType->tag == TypeTags::CLASS){
		//找构造方法
		tree->constructor =  findMethod(tree->pos,s,Name::init,argtypes,className);
		tree->constructorType = tree->constructor->type;
	}

	result = check(tree, clazzType, pt, Kinds::VAL, pkind,"new 类型错误");
}
void Attr::visitNewArray(NewArray* tree) {
	Type* owntype;
	Type* elemtype;
	//通过new 构造
	if(tree->elemtype){
		elemtype = attribType(tree->elemtype, env);
		owntype = elemtype;
		for(int i = 0;i<tree->dims.size();i++){
			attribExpr(tree->dims[i], env, syms->intType);
			owntype = new ArrayType(owntype,syms->arrayClass);
		}
	}else{
		//通过{...}构造
		if (pt->tag == TypeTags::ARRAY) {
			elemtype = ((ArrayType*)pt)->elemtype;
		}else{
			log->report(tree->pos,Log::ERROR,"非法数组初始化");
		}



	}
	if (tree->elems.size()!=0) {
	            attribExprs(tree->elems, env, elemtype);
	            owntype = new ArrayType(elemtype, syms->arrayClass);
	}
	result = check(tree, owntype, pt, Kinds::VAL, pkind,"");
}
void Attr::visitParens(Parens* tree) {
	Type* owntype = attribTree(tree->expr, env, pkind, pt);
	result = check(tree, owntype, pt, pkind, pkind, "类型错误");
}
/**
 *    ！！赋值操作 类型暂时没做兼容性检查
 */
void Attr::visitAssign(Assign* tree) {

	Type* left = attribTree(tree->lhs, env->dup(tree), Kinds::VAR,
			Type::noType);
	Type* right = attribExpr(tree->rhs, env, left);
	result = check(tree, right, pt, Kinds::VAL, pkind, "类型不兼容");
}
/**
 *
 * 类似赋值操作
 */
void Attr::visitAssignop(AssignOp* tree) {
	Type* owntype = attribTree(tree->lhs, env, Kinds::VAR, Type::noType);
	Type* operand = attribExpr(tree->rhs, env, Type::noType);
	Symbol* opsym = tree->opsym = resolveBinaryOperator(tree->pos, env,
			tree->getTag() - Tree::ASGOffset, owntype, operand);
	/**
	 * 这里做兼容性检查  chk
	 */
	result = check(tree, owntype, pt, Kinds::VAL, pkind, "类型不兼容");
}
/**
 * !!!todo:常量折叠
 *  常量折叠：只判断类型是否可以折叠，如果可以判断折叠之后的类型，并在此节点上标记，无需将原来节点删除
 *         在后面生成代码阶段访问到此节点时判断是否是常量。如果是无需继续递归访问子结点
 */
void Attr::visitUnary(Unary* tree) {

	Type* argtype =
			(Tree::PREINC <= tree->getTag() && tree->getTag() >= Tree::POSTDEC) ?
					attribTree(tree->arg, env, Kinds::VAR, Type::noType) :
					chk->checkNonVoid(tree->arg->pos,
							attribExpr(tree->arg, env, Type::noType));
	Symbol* opsym = tree->sym = resolveUnaryOperator(tree->pos, tree->getTag(),
			env, argtype);
	Type* owntype = syms->errType;
	if(opsym->kind==Kinds::MTH){

		owntype = (Tree::PREINC <= tree->getTag() && tree->getTag() >= Tree::POSTDEC) ?tree->arg->type:((MethodType*)opsym->type)->restype;
		int opcode = ((OperatorSymbol*)opsym)->opcode;
		if(argtype->isConst()){
			Type* ct = cfolder->fold1(opcode,(ConstType*)argtype);
			owntype = cfolder->coerce(ct,owntype);

		}

	}
	result = check(tree,owntype,pt,pkind,Kinds::VAL,"类型不兼容");

}

/**
 *
 */
void Attr::visitBinary(Binary* tree) {
//	cout<<"binary env:";
//	env->info->scope->print();
//	cout<<endl;
	Type* left = chk->checkNonVoid(tree->lhs->pos,
			attribExpr(tree->lhs, env, Type::noType));

	Type* right = chk->checkNonVoid(tree->rhs->pos,
			attribExpr(tree->rhs, env, Type::noType));
	Symbol* opsym = tree->opsym = resolveBinaryOperator(tree->pos, env,
			tree->getTag(), left, right);
	Type* owntype = syms->errType ;
	//左右都是常量，进行折叠
	if(opsym->kind==Kinds::MTH){

		owntype=((MethodType*) opsym->type)->restype;
		int opcode = ((OperatorSymbol*)opsym)->opcode;
		if(left->isConst()&&right->isConst()){
			Type* ct = cfolder->fold2(opcode,(ConstType*)left,(ConstType*)right);
			owntype = cfolder->coerce(ct,owntype);

		}


	}
	if (opsym != syms->errSymbol)
		result = check(tree,owntype,pt,Kinds::VAL,pkind,"错误类型") ;
	else
		result = syms->errType;

}
void Attr::visitTypeCast(TypeCast* tree) {

}
void Attr::visitTypeTest(InstanceOf* tree) {
}
void Attr::visitIndexed(ArrayAccess* tree) {
	Type* t;
	//a[]   a的类型
	Type* owntype =  attribExpr(tree->indexed,env,Type::noType);
	//[] 中必须是int
	attribExpr(tree->index,env,syms->intType);
	if(owntype->tag == TypeTags::ARRAY)
		t = ((ArrayType*) owntype)->elemtype;
	else
		log->report(tree->indexed->pos,Log::ERROR,"必须为数组,但找到 "+Type::typeName(owntype));

	  result = check(tree, owntype, pt, Kinds::VAR, pkind,"类型不兼容");
}


void Attr::visitSelect(FieldAccess* tree) {
	int skind = 0;
	if (tree->name == Name::_this) {
		skind = Kinds::TYP;
	} else {
		if ((pkind & Kinds::TYP) != 0)
			skind = skind | Kinds::TYP;
		if ((pkind & (Kinds::VAL | Kinds::MTH)) != 0)
			skind = skind | Kinds::VAL | Kinds::TYP;
	}
	Type* site = attribTree(tree->selected, env, skind, Type::noType);
	Symbol* sitesym = Check::symbol(tree->selected);
	Symbol* sym = selectSym(tree, sitesym, site, pt, pkind);
	tree->sym = sym;
	//静态区 不能访问非静态成员
	if(sym != syms->errSymbol&& (sym->flags_field & Flags::STATIC )==0 ){
		//类名. 不能调用成员方法
		if(dynamic_cast<Ident*>(tree->selected)){
			string name =((Ident*) tree->selected)->name;
			if(name == site->tsym->name)
				log->report(tree->pos, Log::ERROR, "只有对象才能访问非static成员");
		}
	}

	result  = check(tree,sym->type,pt,sym->kind,pkind,"类型不兼容");

}
void Attr::visitIdent(Ident* tree) {

	if (tree->sym == NULL) {
//		ClassSymbol* sym = new ClassSymbol(0, tree->name, syms->rootPackage);
//		sym->type->tsym = sym;
//		tree->sym = sym;
//		result = sym->type;
		//寻找 符号
		if (pt->tag == TypeTags::METHOD) {
			//寻找方法
			tree->sym = findMethod(tree->pos, env->info->scope,
					tree->name == Name::_this ? Name::init : tree->name,
					((MethodType*) pt)->argtypes);
		} else
			tree->sym = findIdent(tree->pos, env, tree->name, pkind);
	}
	bool f = (void*)(tree->sym) != (void*)(syms->errSymbol);
	if(f){
		//当前环境是static区域 ,访问非static成员
		Symbol* sym =  env->info->scope->owner;
		if(sym && (((MethodSymbol*)sym)->flags_field & Flags::STATIC) != 0 &&(tree->sym->flags_field & Flags::STATIC) ==0){
			if((tree->sym->kind&Kinds::MTH )!=0||(tree->sym->kind&Kinds::VAR) !=0)
				log->report(tree->pos,Log::ERROR,"static区不能访问非static成员");

		}
	}
	result = check(tree, tree->sym->type, pt, tree->sym->kind, pkind,
				"类型不兼容");
}
void Attr::visitLiteral(Literal* tree) {

	result =check(tree,litType(tree->typetag,tree->value),pt,Kinds::VAL,pkind,"类型不兼容");
}
Type* Attr::litType(int tag,string strVal) {

	return ConstType::create(tag,strVal);
}
void Attr::visitTypeIdent(PrimitiveTypeTree* tree) {
	tree->type = result = syms->typeOfTag[tree->typetag];
}
void Attr::visitTypeArray(ArrayTypeTree* tree) {
	Type* etype = attribType(tree->elemtype, env);
	Type* type = new ArrayType(etype, syms->arrayClass);
	result = tree->type = type;
}

void Attr::visitModifiers(Modifiers* tree) {

}
void Attr::visitTree(Tree* tree) {
}
