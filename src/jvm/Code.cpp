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

/**
 * Code
 */

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
		delete code;
		code = new_code;
	}
}
int Code::curPc() {
	if (pendingJumps != NULL)
		resolvePending();
	fixedPc = true;
	return cp;
}
void Code::resolvePending() {
	Chain* c = pendingJumps;
	pendingJumps = NULL;
	resolve(c, cp);

}
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
	}
}

void Code::endScope(int adr) {
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
