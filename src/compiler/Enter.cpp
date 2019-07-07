/*
 * Enter.cpp
 *
 *  Created on: 2017年12月31日
 *      Author: ZL
 */

#include "Enter.hpp"
int* Enter::enterKey = new int;
Enter* Enter::instance() {
	Enter* enter = (Enter*) Context::get(enterKey);
	if (enter == NULL) {
		enter = new Enter();
	}
	return enter;
}

Enter::Enter() {
	Context::put(enterKey, this);
	log = Log::instance();
	syms = Symtab::instance();
	chk = Check::instance();
	attr = Attr::instance();
	vector<Tree*> nulllist;
	predefClassDef = TreeMaker::makeClassDecl(0,
			TreeMaker::Mods(0, Flags::PUBLIC), syms->predefClass->name,
			nulllist);
	predefClassDef->sym = syms->predefClass;
	first = true;
}

Enter::~Enter() {
}
void Enter::print(Env<AttrContext*>* env) {
	if (env) {
		cout << "Env[ this = ";
		env->info->scope->print();
		cout << "out = ";
		cout << " ]";
	} else
		cout << "null";
}

//入口
void Enter::main(vector<CompilationUnit*> trees) {

	complete(trees, NULL);
}

void Enter::complete(vector<CompilationUnit*> trees, ClassSymbol* c) {
	vector<ClassSymbol*> prevUncompleted = uncompleted;
	vector<Tree*> trees1;
	util::appendList(trees1, trees);
	classEnter(trees1, NULL);

}
Scope* Enter::enterScope(Env<AttrContext*>* env) {
	return (env->tree->getTag() == Tree::CLASSDEF) ?
			((ClassDecl*) env->tree)->sym->members_field : env->info->scope;
}

Env<AttrContext*>* Enter::topLevelEnv(CompilationUnit* tree) {
	Env<AttrContext*>* localEnv = new Env<AttrContext*>(tree,
			new AttrContext());
	localEnv->toplevel = tree;
	localEnv->enclClass = predefClassDef;
	localEnv->info->scope = syms->rootPackage->members_field;
	return localEnv;
}
/**
 * 返回一个class env
 */
Env<AttrContext*>* Enter::classEnv(ClassDecl* tree, Env<AttrContext*>* env) {

	Env<AttrContext*>* localEnv = env->dup(tree,
			env->info->dup(env->info->scope->dup(tree->sym)));
	localEnv->enclClass = tree;
	localEnv->outer = env;
	localEnv->info->isSelfCall = false;
	return localEnv;
}

/**
 * method env
 */
Env<AttrContext*>* Enter::methodEnv(MethodDecl* tree, Env<AttrContext*>* env) {
	Env<AttrContext*>* localEnv = env->dup(tree,
			env->info->dup(env->info->scope->dup(tree->sym)));
	localEnv->enclMethod = tree;
	localEnv->info->scope->owner = tree->sym;
	if ((tree->mods->flags & Flags::STATIC) != 0)
		localEnv->info->staticLevel++;
	return localEnv;

}

/**
 * 变量初始化环境
 */
Env<AttrContext*>* Enter::initEnv(VariableDecl* tree, Env<AttrContext*>* env) {
	Env<AttrContext*>* localEnv = env->dupto(
			new Env<AttrContext*>(tree, env->info->dup()));
	if (tree->sym->owner->kind == Kinds::TYP) {
		localEnv->info->scope = env->info->scope->dup();
		localEnv->info->scope->owner = tree->sym;
	}
	return localEnv;

}

/**
 * 方法签名  返回类型 ,参数
 */
Type* Enter::signature(Tree* restype, vector<VariableDecl*> params,
		Env<AttrContext*> * localEnv) {
	vector<Type*> args;
	for (int i = 0; i < params.size(); i++) {
		memberEnter(params.at(i), localEnv);
		args.push_back(params.at(i)->vartype->type);
	}

	Type* res =
			restype == NULL ?
					syms->voidType : attr->attribType(restype, localEnv);
	return new MethodType(args, res, syms->methodClass);
}

/**
 * enter  class
 */

vector<Type*> Enter::classEnter(vector<Tree*> trees, Env<AttrContext*>* env) {
	vector<Type*> ts;
	for (int i = 0; i < trees.size(); i++) {
		Type* t = classEnter(trees.at(i), env);
		if (t != NULL)
			ts.push_back(t);
	}
	return ts;

}
Type* Enter::classEnter(Tree* tree, Env<AttrContext*>* env) {
	Env<AttrContext*>* prevEnv = this->env;
	this->env = env;
	tree->accept(this);
	this->env = prevEnv;
	return result;
}

/**
 *   enter member
 */
void Enter::memberEnter(vector<Tree*> trees, Env<AttrContext*>* env) {
	for (int i = 0; i < trees.size(); i++)
		memberEnter(trees.at(i), env);

}
void Enter::memberEnter(Tree* tree, Env<AttrContext*>* env) {
	Env<AttrContext*>* prevEnv = this->env;
	this->env = env;
	tree->accept(this);
	this->env = prevEnv;

}

bool Enter::hashConstructor(vector<Tree*> trees) {
	Tree* tree = NULL;
	for (int i = 0; i < trees.size(); i++) {
		if ((tree = trees.at(i))->getTag() == Tree::METHODDEF) {
			return ((MethodDecl*) tree)->name == Name::init;
		}
	}
	return false;
}

MethodDecl* Enter::DefaultConstructor() {
	long flags = Flags::PUBLIC;
	vector<VariableDecl*> params;
	vector<Statement*> stats;

	MethodDecl* method = TreeMaker::makeMethodDecl(0, TreeMaker::Mods(0, flags),
			TreeMaker::makeTypeIdent(0, TypeTags::VOID), Name::init, params,
			TreeMaker::makeBlock(0, 0, stats));
	return method;
}

/**
 * visitor implemention
 */

//这里完成package导入 ，此版本暂时不支持，跳过
void Enter::visitTopLevel(CompilationUnit* tree) {
	first = true;
	//默认都为预设好的内置符号
	tree->package = syms->rootPackage;
//	tree->package->complete();
	Env<AttrContext*>* topEnv = topLevelEnv(tree);
	topEnv->toplevel = tree;
	//第一次 只是导入class
	classEnter(tree->defs, topEnv);
	first = false;
	//第二次 导入类成员
	classEnter(tree->defs, topEnv);
	result = NULL;
}

void Enter::visitClassDef(ClassDecl* tree) {
	if (first) {
		Scope* enclScope = enterScope(env);
		ClassSymbol* c = new ClassSymbol(Flags::PUBLIC, tree->name,
				syms->rootPackage);

		//检查是否有重复
		if (!chk->checkUniqueClassName(tree->pos, tree->name, enclScope)) {
			result = NULL;
			return;
		}

		enclScope->enter(c);
		Env<AttrContext*>* localEnv = classEnv(tree, env);

		typeEnvs.insert(pair<TypeSymbol*, Env<AttrContext*>*>(c, localEnv));
		todos.push_back(localEnv);
		localEnv->tree = tree;
		localEnv->enclClass = tree;
		localEnv->toplevel = env->toplevel;
		c->members_field = enclScope->dup(c);
		c->flags_field = tree->mods->flags;
		tree->sym = c;
		/**
		 * 填充this
		 */

		VarSymbol* thisSym = new VarSymbol(Flags::FINAL | Flags::HASINIT, Name::_this, c->type, c);
		thisSym->pos = 0;
		c->members_field->enter(thisSym);
		//这里导入有问题 需要等所有class 导入之后才能  导入成员属性
		//	classEnter(tree->defs, localEnv);
		result = c->type;
	} else {
		Env<AttrContext*>* localEnv = typeEnvs.find(tree->sym)->second;
		if (!hashConstructor(tree->defs)) {
			MethodDecl* t = DefaultConstructor();
			MethodSymbol* m = new MethodSymbol(tree->mods->flags, Name::init,
			NULL, localEnv->info->scope->owner);
			m->type = signature(t->restype, t->params, localEnv);
			t->sym = m;
			tree->defs.insert(tree->defs.begin(), t);
		}

		//为每个构造方法第一行添加super()
		//...

		//
		for (int i = 0; i < tree->defs.size(); i++)
			memberEnter(tree->defs.at(i), localEnv);
		result = NULL;
	}
}
void Enter::visitMethodDef(MethodDecl* tree) {
	Scope* enclScope = enterScope(env);
	if(util::debug){
		cout << "enter:visitMethod:";
		enclScope->print();
		cout << endl;
	}
	MethodSymbol* m = new MethodSymbol(tree->mods->flags, tree->name, NULL,
			enclScope->owner);
	tree->sym = m;
	Env<AttrContext*>* localEnv = methodEnv(tree, env);

	//方法的 类型 ，包括 返回类型，参数类型。。(泛型...)
	m->type = signature(tree->restype, tree->params, localEnv);

	if (chk->checkUnique(tree->pos, m, enclScope)) {
		enclScope->enter(m);
	}

	result = m->type;
}
void Enter::visitVarDef(VariableDecl* tree) {
	Env<AttrContext*>* localEnv = env;
	Scope* enclScope = enterScope(env);
	attr->attribType(tree->vartype, localEnv);
	long flags = env->enclMethod ? Flags::STATIC : tree->mods->flags;
	VarSymbol* v = new VarSymbol(flags, tree->name, tree->vartype->type,
			enclScope->owner);
	v->flags_field = tree->mods->flags;
	tree->sym = v;
	if (chk->checkUnique(tree->pos, v, enclScope)) {
		enclScope->enter(v);
	}
	//debug
//	enclScope->print();
	result = tree->type;
}
void Enter::visitSkip(Skip* tree) {
	result = NULL;
}
void Enter::visitBlock(Block* tree) {
	result = NULL;
}
void Enter::visitDoLoop(DoWhileLoop* tree) {
	result = NULL;
}
void Enter::visitWhileLoop(WhileLoop* tree) {
	result = NULL;
}
void Enter::visitForLoop(ForLoop* tree) {
	result = NULL;
}

void Enter::visitSwitch(Switch* tree) {
	result = NULL;
}
void Enter::visitCase(Case* tree) {
	result = NULL;
}

void Enter::visitConditional(Conditional* tree) {
	result = NULL;
}
void Enter::visitIf(If* tree) {
	result = NULL;
}
void Enter::visitExec(ExpressionStatement* tree) {
	result = NULL;
}
void Enter::visitBreak(Break* tree) {
	result = NULL;
}
void Enter::visitContinue(Continue* tree) {
	result = NULL;
}
void Enter::visitReturn(Return* tree) {
	result = NULL;
}
void Enter::visitApply(MethodInvocation* tree) {
	result = NULL;
}
void Enter::visitNewClass(NewClass* tree) {
	result = NULL;
}
void Enter::visitNewArray(NewArray* tree) {
	result = NULL;
}
void Enter::visitParens(Parens* tree) {
	result = NULL;
}
void Enter::visitAssign(Assign* tree) {
	result = NULL;
}
void Enter::visitAssignop(AssignOp* tree) {
	result = NULL;
}
void Enter::visitUnary(Unary* tree) {
	result = NULL;
}
void Enter::visitBinary(Binary* tree) {
	result = NULL;
}
void Enter::visitTypeCast(TypeCast* tree) {
	result = NULL;
}

void Enter::visitTypeTest(InstanceOf* tree) {
	result = NULL;
}
void Enter::visitIndexed(ArrayAccess* tree) {
	result = NULL;
}
void Enter::visitSelect(FieldAccess* tree) {
	result = NULL;
}
void Enter::visitIdent(Ident* tree) {
	result = NULL;
}
void Enter::visitLiteral(Literal* tree) {
	result = NULL;
}
void Enter::visitTypeIdent(PrimitiveTypeTree* tree) {
	result = NULL;
}
void Enter::visitTypeArray(ArrayTypeTree* tree) {
	result = NULL;
}
void Enter::visitModifiers(Modifiers* tree) {
	result = NULL;
}
void Enter::visitTree(Tree* tree) {
	result = NULL;
}
