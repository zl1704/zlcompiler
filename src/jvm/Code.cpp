/*
 * Code.cpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */
#include"Code.hpp"

/**
 * State
 */

State* State::dup() {
	State* nstate = new State();
	nstate->defined = defined->dup();
	Type** nstack = new Type*[s_size];
	memcpy(nstack, stack, s_size);
	nstate->stack = nstack;
	nstate->s_size = s_size;
	nstate->stacksize = stacksize;
	int* new_locks = new int[l_size];
	memcpy(new_locks, locks, l_size * sizeof(int));
	nstate->locks = new_locks;
	nstate->l_size = l_size;
	nstate->nlocks = nlocks;
	return nstate;

}
void State::lock(int reg) {
	if (l_size == nlocks) {
		int new_size = l_size << 1;
		int* new_locks = new int[new_size];
		memset(new_locks, NULL, new_size * sizeof(int));
		memcpy(new_locks, locks, l_size * sizeof(int));
		l_size = new_size;
		locks = new_locks;
		delete locks;
	}

	locks[nlocks++] = reg;

}
void State::unlock(int reg) {

	nlocks--;
	if (locks[nlocks] != reg) {
		printf("locks reg： %d(locks中) != %d(目标)\n", locks[nlocks], reg);
	}
	locks[nlocks] = -1;
}
void State::push(Type* t) {
	switch (t->tag) {
	case TypeTags::VOID:
		return;
	case TypeTags::BYTE:
	case TypeTags::BOOLEAN:
	case TypeTags::CHAR:
	case TypeTags::SHORT:
		t = code->syms->intType;
		break;
	default:
		break;

	}
	if (stacksize + 2 >= s_size) {
		Type** nstack = new Type*[s_size * 2];
		memset(nstack, NULL, s_size * 2);
		memcpy(nstack, stack, s_size);
		s_size = s_size * 2;
		delete stack;
		stack = nstack;
	}
	stack[stacksize++] = t;
	//宽度为2的类型，多留出一个位置
	switch (Code::width(t)) {
	case 1:
		break;
	case 2:
		stack[stacksize++] = NULL;
		break;
	default:
		break;
	}
	if (stacksize > code->max_stack)
		code->max_stack = stacksize;
}
Type* State::pop1() {
	stacksize--;
	Type* result = stack[stacksize];
	stack[stacksize] = NULL;
	return result;
}
Type* State::peek() {
	return stack[stacksize - 1];
}
Type* State::pop2() {
	stacksize = stacksize - 2;
	Type* result = stack[stacksize];
	stack[stacksize] = NULL;
	return result;
}
void State::pop(int n) {
	while (n > 0) {
		stack[--stacksize] = NULL;
		n--;
	}
}
void State::pop(Type* t) {
	pop(Code::width(t));
}
//把栈顶转为父类t
void State::forceStackTop(Type* t) {
	if (!code->alive)
		return;
	switch (t->tag) {
	case TypeTags::ARRAY:
	case TypeTags::CLASS:
		stack[stacksize - Code::width(t)] = t;
		break;
	default:
		break;
	}

}
State* State::join(State* other){

	return this;
}


/**
 * Code
 */
Code::Code(MethodSymbol* sym,Pool* pool){
		msym = sym;
		this->pool = pool;
		code = new jbyte[64];
		size = 64;
		lvs = new LocalVar*[20];
		lvsize = 20;
		alive = false;
		max_locals = 0;
		max_stack = 0;
		nextreg = 0;
		cp = 1;
		pendingStatPos = -1;
		syms = Symtab::instance();
		state = new State(this);
		pendingJumps = NULL;
		fixedPc = false;
		source = Source::instance();
}



int Code::truncate(int tc) {
	switch (tc) {
	case ByteCodes::BYTEcode:
	case ByteCodes::SHORTcode:
	case ByteCodes::CHARcode:
		return ByteCodes::INTcode;
	default:
		return tc;
	}
}

int Code::width(int tc) {
	switch (tc) {
	case ByteCodes::LONGcode:
	case ByteCodes::DOUBLEcode:
		return 2;
	case ByteCodes::VOIDcode:
		return 0;
	default:
		return 1;
	}
}
int Code::width(Type* type) {
	return type == NULL ? 0 : width(ByteCodes::typecode(type));
}

void Code::checkCode() {
	if (cp == size) {
		jbyte* new_code = new jbyte[size * 2];
		memset(new_code, NULL, size * 2 * sizeof(jbyte));
		memcpy(new_code,code,size*sizeof(jbyte));
		delete code;
		code = new_code;
		size = size *2;
	}
}
int Code::curPc() {
	if (pendingJumps != NULL)
		resolvePending();
	//fixedPc = true;
	return cp;
}
void Code::resolvePending() {
	Chain* c = pendingJumps;
	pendingJumps = NULL;
	resolve(c, cp);

}
//回填
void Code::resolve(Chain* c, int target) {


}

/**
 *
 */
void Code::addLocalVar(VarSymbol* v) {

	if (v->adr + 1 >= lvsize) {
		int new_size = lvsize * 2;
		LocalVar** new_lvs = new LocalVar*[new_size];
		memset(new_lvs, NULL, new_size * sizeof(LocalVar*));
		memcpy(new_lvs, lvs, lvsize * sizeof(LocalVar*));
		delete lvs;
		lvsize = new_size;
		lvs = new_lvs;
	}

	if (pendingJumps != NULL)
		resolvePending();
	lvs[v->adr] = new LocalVar(v);
	//这里为什么要exclude???
	state->defined->include(v->adr);
}
int Code::newLocalVar(VarSymbol* v) {
	int reg = v->adr = newLocalVar(v);
	addLocalVar(v);
	return reg;
}
int Code::newLocalVar(Type* type) {
	int reg = nextreg;
	int width = Code::width(type);
	nextreg = reg + width;
	if (nextreg > max_locals)
		max_locals = nextreg;
	return reg;
}
void Code::setDefined(Bits* newDefined) {
}
void Code::setDefined(int adr) {
	LocalVar* v = lvs[adr];
	if (v == NULL) {
		state->defined->exclude(adr);
	} else {
		state->defined->include(adr);
		//
		if (cp < MAXPC)
			v->start_pc = cp;
	}

}
void Code::setUnDefined(int adr) {
	state->defined->exclude(adr);
	if (adr < lvsize && lvs[adr] != NULL && lvs[adr]->start_pc != MAXPC) {
		LocalVar* v = lvs[adr];
		int length = curPc() - v->start_pc;
		if(length>0&&length<MAXPC){
			v->length = length;
			//lvs[adr] = NULL; //可以推迟endScope中置NULL
			//lvs[adr] = v->dup();
		}else{
			v->length = MAXPC;
		}
	}
}

void Code::endScope(int adr) {

	LocalVar* v = lvs[adr];
	if(v!=NULL){
		lvs[adr] = NULL;
		if(v->start_pc!=MAXPC){
			int length = curPc()-v->start_pc;
			v->length = length;
		}
	}
	state->defined->exclude(adr);

}

void Code::endScopes(int adr){
	int preNextReg = nextreg;
	nextreg = adr;
	for(int i = adr;i<preNextReg;i++)
		endScope(i);

}

void Code::addLineNumber(int startPc,int lineNumber){

	//lineNumber行已添加的无须再添加
	if(lineInfo.empty()||lineInfo.back()[1]!=lineNumber){
		lineInfo.push_back(util::ListOf(startPc,lineNumber));
	}
}
void Code::statBegin(int pos){
	if(pos!=-1){
		pendingStatPos = pos;
	}

}
void Code::markStatBegin(){

	if(alive){
		int line = source->getLine(pendingStatPos);
		addLineNumber(cp,line);
	}
	pendingStatPos = -1;
}

/**
 * Emit code
 */

void Code::emitInvokedynamic(int meth, Type* mtype){

}
void Code::emitInvokespecial(int meth, Type* mtype){

}
void Code::emitInvokestatic(int meth, Type* mtype){

}
void Code::emitInvokevirtual(int meth, Type mtype){

}
void Code::emitJump(int op){}
//输出一个无操作数的操作码
void Code::emitop0(int op){
	emitop(op);
	if (!alive) return;
	//TOS，记录栈顶类型
	switch(op){
	case ByteCodes::aaload:{
		//对象数组
		state->pop(1);//index
		Type* t= state->pop1();
		Type* stackType = t->tag == TypeTags::BOT ?syms->objectType : Type::elemtype(t);
		state->push(stackType);

	}
	break;
	case ByteCodes::goto_:
		markDead();
		break;
	case ByteCodes::nop:
	case ByteCodes::ineg:
	case ByteCodes::lneg:
	case ByteCodes::fneg:
	case ByteCodes::dneg:
		break;
	case ByteCodes::aconst_null:
		state->push(syms->botType);
		break;
	case ByteCodes::iconst_m1:
	case ByteCodes::iconst_0:
	case ByteCodes::iconst_1:
	case ByteCodes::iconst_2:
	case ByteCodes::iconst_3:
	case ByteCodes::iconst_4:
	case ByteCodes::iconst_5:
	case ByteCodes::iload_0:
	case ByteCodes::iload_1:
	case ByteCodes::iload_2:
	case ByteCodes::iload_3:
		state.push(syms->intType);
		break;
	case ByteCodes::lconst_0:
	case ByteCodes::lconst_1:
	case ByteCodes::lload_0:
	case ByteCodes::lload_1:
	case ByteCodes::lload_2:
	case ByteCodes::lload_3:
		state.push(syms->longType);
				break;
	case ByteCodes::fconst_0:
	case ByteCodes::fconst_1:
	case ByteCodes::fconst_2:
	case ByteCodes::fload_0:
	case ByteCodes::fload_1:
	case ByteCodes::fload_2:
	case ByteCodes::fload_3:
		state.push(syms->floatType);
		break;
	case ByteCodes::dconst_0:
	case ByteCodes::dconst_1:
	case ByteCodes::dload_0:
	case ByteCodes::dload_1:
	case ByteCodes::dload_2:
	case ByteCodes::dload_3:
		state.push(syms->longType);
		break;
	case ByteCodes::aload_0:
		state.push(lvs[0]->vsym->type);
		break;
	case ByteCodes::aload_1:
		state.push(lvs[1]->vsym->type);
		break;
	case ByteCodes::aload_2:
		state.push(lvs[2]->vsym->type);
		break;
	case ByteCodes::aload_3:
		state.push(lvs[3]->vsym->type);
		break;
	case ByteCodes::iaload:
	case ByteCodes::baload:
	case ByteCodes::caload:
	case ByteCodes::saload:
		state->pop(2);
		state->push(syms->intType);
		break;
	case ByteCodes::laload:
		state->pop(2);
		state->push(syms->longType);
		break;
	case ByteCodes::faload:
		state->pop(2);
		state->push(syms->floatType);
		break;
	case ByteCodes::daload:
		state->pop(2);
		state->push(syms->doubleType);
		break;
	}

}
void Code::emitop1(int op, int od){}
void Code::emitop1w(int op, int od){}
void Code::emitop1w(int op, int  od1, int od2){}
void Code::emitop2(int op, int od){}
void Code::emitop4(int op, int od){}

void Code::emit1(int op){
	if(!alive)
		return;
	checkCode();
	code[cp++] = (jbyte)op;

}
void Code::emit2(int od){
	emit1(od >> 8);
	emit1(od);
}
void Code::emit4(int od){
	emit1(od >> 24);
	emit1(od >> 16);
	emit1(od >> 8);
	emit1(od);
}
//输出一个操作码，可能有操作数，后续会生成
void Code::emitop(int op){
	if (pendingJumps != NULL) resolvePending();
	if(alive){
		if(pendingStatPos!=-1)
			markStatBegin();
		emit1(op);
	}
}

//get put
//返回pc处 code，无符号整数，清除高位信息
int Code::get1(int pc){
	return code[pc] & 0xFF;
}
int Code::get2(int pc){
	return (get1(pc)<< 8 ) |get1(pc+1);
}
int Code::get4(int pc) {
	return (get1(pc) << 24) | get1(pc + 2) << 16 | get1(pc + 3) << 8
			| get1(pc + 4);
}

void Code::put1(int pc, int od) {
	code[pc] = (jbyte) od;
}
void Code::put2(int pc, int od) {
	put1(pc, od >> 8);
	put1(pc + 1, od);
}
void Code::put4(int pc, int od) {
	put1(pc, od >> 24);
	put1(pc + 1, od >> 16);
	put1(pc + 2, od >> 8);
	put1(pc + 3, od);
}
//字节对齐
void Code::align(int incr){
	if (alive)
	   while (cp % incr != 0) emitop0(ByteCodes::nop);
}



/**
 *  Chain
 */
Chain::Chain(int pc, Chain* next) {
	this->pc = pc;
	this->next = next;

}

Chain* Chain::merge(Chain* c1, Chain* c2) {
	if (c1 == NULL)
		return c2;
	if (c2 == NULL)
		return c1;
	Chain* new_chain = NULL;
	if (c1->pc < c2->pc)
		new_chain = new Chain(c2->pc, merge(c1, c2->next));
	new_chain = new Chain(c1->pc, merge(c1->next, c2));
	delete c1;
	delete c2;
	return new_chain;
}
