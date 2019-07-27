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
State* State::join(State* other) {

	return this;
}

/**
 * Code
 */
Code::Code(MethodSymbol* sym, Pool* pool) {
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
	items = NULL;
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
//取反操作
int Code::negate(int opcode) {

	if (opcode == ByteCodes::if_acmp_null)
		return ByteCodes::if_acmp_nonnull;
	else if (opcode == ByteCodes::if_acmp_nonnull)
		return ByteCodes::if_acmp_null;
	else
		return ((opcode + 1) ^ 1) - 1;
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
		memcpy(new_code, code, size * sizeof(jbyte));
		delete code;
		code = new_code;
		size = size * 2;
	}
}
int Code::curPc() {
	if (pendingJumps != NULL)
		resolvePending();
	//fixedPc = true;
	return cp;
}
int Code::entryPoint() {

	int pc = curPc();
	alive = true;
	return pc;
}

void Code::resolvePending() {
	Chain* c = pendingJumps;
	pendingJumps = NULL;
	resolve(c, cp);

}
//回填
//！！！
void Code::resolve(Chain* c, int target) {
	bool changed = false;
	State* newState = state;
	for (; c != NULL; c = c->next) {

		if (target > cp)
			target = cp;
		else if (get1(target) == ByteCodes::goto_) {
			//target是跳转指令，无需跳到这里
			target = target + get2(target + 1);
		}
		//target为goto(chain)下条指令无需跳转，且跳转到target
		if (get1(c->pc) == ByteCodes::goto_ && c->pc + 3 == target
				&& target == cp && !fixedPc) {
			//删除当前指令
			cp = cp - 3;
			target = target - 3;
			if (c->next == NULL) {
				alive = true;
				break;
			}
		} else {
			//goto为相对当前指令pc的offset,实际pc=curPc+offset
			put2(c->pc + 1, target - c->pc);

		}
		fixedPc = true;
		if (target == cp) {
			changed = true;
			//...
			if (alive) {

			} else {

			}

		}

	}

}
void Code::resolve(Chain* c) {
	Chain::merge(c, pendingJumps);
}

Chain* Code::branch(int opcode) {

	Chain* result = NULL;
	if (opcode == ByteCodes::goto_) {
		result = pendingJumps;
		pendingJumps = NULL;
	}
	if (opcode != ByteCodes::dontgoto && isAlive()) {
		result = new Chain(emitJump(opcode), result, state->dup());
		if (opcode == ByteCodes::goto_)
			alive = false;

	}
	return result;
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
	int reg = v->adr = newLocalVar(v->type);
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
		if (length > 0 && length < MAXPC) {
			v->length = length;
			//lvs[adr] = NULL; //可以推迟endScope中置NULL
			//lvs[adr] = v->dup();
		} else {
			v->length = MAXPC;
		}
	}
}

void Code::endScope(int adr) {

	LocalVar* v = lvs[adr];
	if (v != NULL) {
		lvs[adr] = NULL;
		if (v->start_pc != MAXPC) {
			int length = curPc() - v->start_pc;
			v->length = length;
		}
	}
	state->defined->exclude(adr);

}

void Code::endScopes(int adr) {
	int preNextReg = nextreg;
	nextreg = adr;
	for (int i = adr; i < preNextReg; i++)
		endScope(i);

}

void Code::addLineNumber(int startPc, int lineNumber) {

	//lineNumber行已添加的无须再添加
	if (lineInfo.empty() || lineInfo.back()[1] != lineNumber) {
		lineInfo.push_back(util::ListOf(startPc, lineNumber));
	}
}
void Code::statBegin(int pos) {
	if (pos != -1) {
		pendingStatPos = pos;
	}

}
void Code::markStatBegin() {

	if (alive) {
		int line = source->getLine(pendingStatPos);
		addLineNumber(cp, line);
	}
	pendingStatPos = -1;
}

/**
 * Emit code
 */

void Code::emitInvokedynamic(int meth, Type* mtype) {

}
void Code::emitInvokespecial(int meth, Type* mtype) {

}
void Code::emitInvokestatic(int meth, Type* mtype) {

}
void Code::emitInvokevirtual(int meth, Type mtype) {

}
int Code::emitJump(int op) {
	//等待回填
	emitop2(op, 0);
	return cp - 3;
}
/**
 * 输出一个无操作数的操作码,并管理栈中状态(TOS)
 */
void Code::emitop0(int op) {
	emitop(op);
	if (!alive)
		return;
	//TOS，记录栈顶类型
	Type* a, *b, *c, *d;
	switch (op) {
	case ByteCodes::aaload: {
		//对象数组
		state->pop(1);			//index
		Type* t = state->pop1();
		Type* stackType =
				t->tag == TypeTags::BOT ? syms->objectType : Type::elemtype(t);
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
		state->push(syms->intType);
		break;
	case ByteCodes::lconst_0:
	case ByteCodes::lconst_1:
	case ByteCodes::lload_0:
	case ByteCodes::lload_1:
	case ByteCodes::lload_2:
	case ByteCodes::lload_3:
		state->push(syms->longType);
		break;
	case ByteCodes::fconst_0:
	case ByteCodes::fconst_1:
	case ByteCodes::fconst_2:
	case ByteCodes::fload_0:
	case ByteCodes::fload_1:
	case ByteCodes::fload_2:
	case ByteCodes::fload_3:
		state->push(syms->floatType);
		break;
	case ByteCodes::dconst_0:
	case ByteCodes::dconst_1:
	case ByteCodes::dload_0:
	case ByteCodes::dload_1:
	case ByteCodes::dload_2:
	case ByteCodes::dload_3:
		state->push(syms->longType);
		break;
	case ByteCodes::aload_0:
		state->push(lvs[0]->vsym->type);
		break;
	case ByteCodes::aload_1:
		state->push(lvs[1]->vsym->type);
		break;
	case ByteCodes::aload_2:
		state->push(lvs[2]->vsym->type);
		break;
	case ByteCodes::aload_3:
		state->push(lvs[3]->vsym->type);
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
	case ByteCodes::istore_0:
	case ByteCodes::istore_1:
	case ByteCodes::istore_2:
	case ByteCodes::istore_3:
	case ByteCodes::fstore_0:
	case ByteCodes::fstore_1:
	case ByteCodes::fstore_2:
	case ByteCodes::fstore_3:
	case ByteCodes::astore_0:
	case ByteCodes::astore_1:
	case ByteCodes::astore_2:
	case ByteCodes::astore_3:
	case ByteCodes::pop:
	case ByteCodes::lshr:
	case ByteCodes::lshl:
	case ByteCodes::lushr:
		state->pop(1);
		break;
	case ByteCodes::iastore:
	case ByteCodes::fastore:
	case ByteCodes::aastore:
	case ByteCodes::bastore:
	case ByteCodes::castore:
	case ByteCodes::sastore:
		//stack :...,value,index,array
		state->pop(3);
		break;
	case ByteCodes::lastore:
	case ByteCodes::dastore:
		//stack :...,value(two word),index,array
		state->pop(4);
		break;
	case ByteCodes::areturn:
	case ByteCodes::ireturn:
	case ByteCodes::freturn:
		state->pop(1);
		markDead();
		break;
	case ByteCodes::lstore_0:
	case ByteCodes::lstore_1:
	case ByteCodes::lstore_2:
	case ByteCodes::lstore_3:
	case ByteCodes::dstore_0:
	case ByteCodes::dstore_1:
	case ByteCodes::dstore_2:
	case ByteCodes::dstore_3:
	case ByteCodes::pop2:
		state->pop(2);
		break;
	case ByteCodes::lreturn:
	case ByteCodes::dreturn:
		state->pop(2);
		markDead();
		break;
	case ByteCodes::ret:			// with a jsr or jsr_w.
	case ByteCodes::return_:			//from method
		markDead();
		break;
	case ByteCodes::dup:
		state->push(state->peek());
		break;
	case ByteCodes::dup2:
		state->push(state->peek());
		break;
	case ByteCodes::dup_x1:
		//before:  stack: ...,a,b
		b = state->pop1();
		a = state->pop1();
		state->push(b);
		state->push(a);
		state->push(b);
		//after:  stack: ...,b,a,b
		break;
	case ByteCodes::dup_x2:
		//Duplicate the top word to place 4.
		//before:  stack: ...,a,b,c
		c = state->pop1();
		if (state->peek() != NULL) {

			Type* b = state->pop1();
			Type* a = state->pop1();
			state->push(c);
			state->push(b);
			state->push(a);
			state->push(c);
		} else {
			//a,b是一个双字数据
			Type* a = state->pop2();
			state->push(c);
			state->push(a);
			state->push(c);
		}
		//after:  stack: ...,c,a,b,c
		break;
	case ByteCodes::dup2_x1:
		//Duplicate the top two words to places 4 and 5.
		//先判断栈顶是否为双字数据
		//before:  stack: ...,a,b,c,
		if (state->peek() != NULL) {
			Type* c = state->pop1();
			Type* b = state->pop1();
			Type* a = state->pop1();
			state->push(b);
			state->push(c);
			state->push(a);
			state->push(b);
			state->push(c);

		} else {
			Type* b = state->pop2();
			Type* a = state->pop2();
			state->push(b);
			state->push(a);
			state->push(b);
		}
		//after:  stack: ...b,c,a,b,c
		break;
	case ByteCodes::dup2_x2:
		//Duplicate the top two words to places 5 and 6.
		//先判断栈顶是否为双字数据
		//before:  stack: ...,a,b,c,d
		if (state->peek() != NULL) {
			Type* d = state->pop1();
			Type* c = state->pop1();
			if (state->peek() != NULL) {
				Type* b = state->pop1();
				Type* a = state->pop1();
				state->push(c);
				state->push(d);
				state->push(a);
				state->push(b);
				state->push(c);
				state->push(d);
			} else {
				Type* a = state->pop2();
				state->push(c);
				state->push(d);
				state->push(a);
				state->push(c);
				state->push(d);
			}

		} else {
			Type* c = state->pop2();
			if (state->peek() != NULL) {
				Type* b = state->pop1();
				Type* a = state->pop1();
				state->push(c);
				state->push(a);
				state->push(b);
				state->push(c);
			} else {
				Type* a = state->pop2();
				state->push(c);
				state->push(a);
				state->push(c);
			}
		}
		//after:  stack: ...c,d,a,b,c,d
		break;
	case ByteCodes::swap:
		a = state->pop1();
		b = state->pop2();
		state->push(a);
		state->push(b);
		break;
	case ByteCodes::iadd:
	case ByteCodes::isub:
	case ByteCodes::imul:
	case ByteCodes::idiv:
	case ByteCodes::imod:
	case ByteCodes::ishl:
	case ByteCodes::ishr:
	case ByteCodes::iushr:
	case ByteCodes::iand:
	case ByteCodes::ior:
	case ByteCodes::ixor:
		state->pop(1);
		break;
	case ByteCodes::ladd:
	case ByteCodes::lsub:
	case ByteCodes::lmul:
	case ByteCodes::ldiv:
	case ByteCodes::lmod:
	case ByteCodes::land:
	case ByteCodes::lor:
	case ByteCodes::lxor:
		state->pop(2);
		break;
	case ByteCodes::fadd:
	case ByteCodes::fsub:
	case ByteCodes::fmul:
	case ByteCodes::fdiv:
	case ByteCodes::fmod:
		state->pop(1);
		break;
	case ByteCodes::dadd:
	case ByteCodes::dsub:
	case ByteCodes::dmul:
	case ByteCodes::ddiv:
	case ByteCodes::dmod:
		state->pop(2);
		break;
	case ByteCodes::i2l:
		state->pop(1);
		state->push(syms->longType);
		break;
	case ByteCodes::i2f:
		state->pop(1);
		state->push(syms->floatType);
		break;
	case ByteCodes::i2d:
		state->pop(1);
		state->push(syms->doubleType);
		break;
	case ByteCodes::l2i:
		state->pop(2);
		state->push(syms->intType);
		break;
	case ByteCodes::l2f:
		state->pop(2);
		state->push(syms->floatType);
		break;
	case ByteCodes::l2d:
		state->pop(2);
		state->push(syms->doubleType);
		break;
	case ByteCodes::f2i:
		state->pop(1);
		state->push(syms->intType);
		break;
	case ByteCodes::f2l:
		state->pop(1);
		state->push(syms->longType);
		break;
	case ByteCodes::f2d:
		state->pop(1);
		state->push(syms->doubleType);
		break;
	case ByteCodes::d2i:
		state->pop(2);
		state->push(syms->intType);
		break;
	case ByteCodes::d2l:
		state->pop(2);
		state->push(syms->longType);
		break;
	case ByteCodes::d2f:
		state->pop(2);
		state->push(syms->floatType);
		break;
	case ByteCodes::tableswitch:
	case ByteCodes::lookupswitch:
		state->pop(1);
		break;
	case ByteCodes::int2byte:
	case ByteCodes::int2char:
	case ByteCodes::int2short:
		break;
	case ByteCodes::lcmp:
		state->pop(4);
		state->push(syms->intType);
		break;
	case ByteCodes::fcmpl:
	case ByteCodes::fcmpg:
		state->pop(2);
		state->push(syms->intType);
		break;
	case ByteCodes::dcmpl:
	case ByteCodes::dcmpg:
		state->pop(4);
		state->push(syms->intType);
		break;
	case ByteCodes::wide:		//must be handled by the caller
		return;
	case ByteCodes::monitorenter:
	case ByteCodes::monitorexit:
		state->pop(1);
		break;
	default:
		cout << "emitop0 error" << endl;
	}

}
//! 不知道是否可行，回头再测试
Type* Code::typeForPool(void* cd) {

	if (static_cast<ConstType*>(cd)) {
		ConstType* ct = static_cast<ConstType*>(cd);
		if (ct->tag == TypeTags::INT)
			return syms->intType;
		if (ct->tag == TypeTags::FLOAT)
			return syms->floatType;
		if (ct->tag == TypeTags::STRING)
			return syms->stringType;
		if (ct->tag == TypeTags::DOUBLE)
			return syms->doubleType;

	} else if (static_cast<ClassSymbol*>(cd) || static_cast<ArrayType*>(cd)) {
		return syms->classType;
	}
	cout << "error:  typeForPool ";
	return NULL;

}
/**
 * 一个字节操作数
 * bipush ldc1
 */
void Code::emitop1(int op, int od) {
	emitop(op);
	if (!alive)
		return;
	emit1(od);
	switch (op) {
	case ByteCodes::bipush:
		state->push(syms->intType);
		break;
	case ByteCodes::ldc1:
		state->push(typeForPool(pool->pool[od]));
		break;
	default:
		cout << ByteCodes::getCodeStr(op) << "不应该出现在 emitop1" << endl;

	}
}
/**
 * 一个字节操作数
 * 可带wide，如果od大于一个字节范围，附加wide
 * 可带wide且一个操作数的指令:load store ret
 * 最多两个字节，局部变量表最大65535
 */
void Code::emitop1w(int op, int od) {
	if (od > 0xFF) {
		emitop(ByteCodes::wide);
		emitop(op);
		emit2(od);
	} else {
		emitop(op);
		emit1(od);
	}
	if (!alive)
		return;
	switch (op) {
	case ByteCodes::iload:
		state->push(syms->intType);
		break;
	case ByteCodes::lload:
		state->push(syms->longType);
		break;
	case ByteCodes::fload:
		state->push(syms->floatType);
		break;
	case ByteCodes::dload:
		state->push(syms->doubleType);
		break;
	case ByteCodes::aload:
		state->push(lvs[od]->vsym->type);
		break;
	case ByteCodes::lstore:
	case ByteCodes::dstore:
		state->pop(2);
		break;
	case ByteCodes::istore:
	case ByteCodes::fstore:
	case ByteCodes::astore:
		state->pop(1);
		break;
	case ByteCodes::ret:
		markDead();
		break;
	default:
		cout << ByteCodes::getCodeStr(op) << "  不应该出现在emitop1w" << endl;
	}

}
/**
 * 两个操作数
 * 可带wide
 * od1:局部变量表index
 * od2:第二操作数
 */
void Code::emitop1w(int op, int od1, int od2) {

	if (od1 > 0xFF || od2 < -128 || od2 > 127) {
		emitop(ByteCodes::wide);
		emitop(op);
		emit2(od1);
		emit2(od2);
	} else {
		emitop(op);
		emit1(od1);
		emit1(od2);
	}
	if (!alive)
		return;
	switch (op) {
	case ByteCodes::iinc:
		break;
	default:
		cout << ByteCodes::getCodeStr(op) << "  不应该出现在emitop1w(双操作数)" << endl;
	}
}

void Code::emitop2(int op, int od) {
	emitop(op);
	if (!alive)
		return;
	emit2(od);
	void* o;
	switch (op) {
	case ByteCodes::getstatic:		//od为常量池中index
		state->push(((Symbol*) (pool->pool[od]))->type);
		break;
	case ByteCodes::putstatic:		//od为常量池中index
		state->pop(((Symbol*) (pool->pool[od]))->type);
		break;
	case ByteCodes::new_:
		//new仅分配对象所需空间，紧接着调用构造方法初始化
		state->push(((Symbol*) (pool->pool[od]))->type);
		break;
	case ByteCodes::sipush:
		state->push(syms->intType);
		break;
	case ByteCodes::if_acmp_null:
	case ByteCodes::if_acmp_nonnull:
	case ByteCodes::ifeq:
	case ByteCodes::ifne:
	case ByteCodes::iflt:
	case ByteCodes::ifge:
	case ByteCodes::ifgt:
	case ByteCodes::ifle:
		state->pop(1);
		break;
	case ByteCodes::if_icmpeq:
	case ByteCodes::if_icmpne:
	case ByteCodes::if_icmplt:
	case ByteCodes::if_icmpge:
	case ByteCodes::if_icmpgt:
	case ByteCodes::if_icmple:
	case ByteCodes::if_acmpeq:
	case ByteCodes::if_acmpne:
		state->pop(2);
		break;
	case ByteCodes::goto_:
		markDead();
	case ByteCodes::getfield:
		state->push(((Symbol*) (pool->pool[od]))->type);
		break;
	case ByteCodes::putfield:
		state->pop(((Symbol*) (pool->pool[od]))->type);
		break;
	case ByteCodes::checkcast:
		state->pop(1);
		o = pool->pool[od];
		if (static_cast<Symbol*>(o)) {
			state->push(static_cast<Symbol*>(o)->type);
		} else {
			state->push((Type*) o);
		}
		break;

	case ByteCodes::ldc2w:
	case ByteCodes::ldc2:
		state->pop(typeForPool(pool->pool[od]));
		break;
	case ByteCodes::instanceof_:
		state->pop(1);
		state->push(syms->intType);
		break;
	case ByteCodes::jsr:
		break;
	default:
		cout << ByteCodes::getCodeStr(op) << "  不应该出现在emitop2" << endl;
	}
}
void Code::emitop4(int op, int od) {
	emitop(op);
	if (!alive)
		return;
	emit4(od);
	switch (op) {
	case ByteCodes::goto_w:
		markDead();
		break;
	case ByteCodes::jsr_w:
		break;
	default:
		cout << ByteCodes::getCodeStr(op) << "  不应该出现在emitop4" << endl;
	}
}

void Code::emit1(int op) {
	if (util::debug) {
		cout << "Gen :\t " << ByteCodes::getCodeStr(op) << endl;
	}
	if (!alive)
		return;
	checkCode();
	code[cp++] = (jbyte) op;

}
void Code::emit2(int od) {
	emit1(od >> 8);
	emit1(od);
}
void Code::emit4(int od) {
	emit1(od >> 24);
	emit1(od >> 16);
	emit1(od >> 8);
	emit1(od);
}
//输出一个操作码，可能有操作数，后续会生成
void Code::emitop(int op) {
	if (pendingJumps != NULL)
		resolvePending();
	if (alive) {
		if (pendingStatPos != -1)
			markStatBegin();
		emit1(op);
	}
}

//get put
//返回pc处 code，无符号整数，清除高位信息
int Code::get1(int pc) {
	return code[pc] & 0xFF;
}
int Code::get2(int pc) {
	return (get1(pc) << 8) | get1(pc + 1);
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
void Code::align(int incr) {
	if (alive)
		while (cp % incr != 0)
			emitop0(ByteCodes::nop);
}

/**
 *  Chain
 */
Chain::Chain(int pc, Chain* next, State* state) {
	this->pc = pc;
	this->next = next;
	this->state = state;
}

Chain* Chain::merge(Chain* c1, Chain* c2) {
	if (c1 == NULL)
		return c2;
	if (c2 == NULL)
		return c1;
	Chain* new_chain = NULL;
	if (c1->pc < c2->pc)
		new_chain = new Chain(c2->pc, merge(c1, c2->next), c2->state);
	new_chain = new Chain(c1->pc, merge(c1->next, c2), c1->state);
	delete c1;
	delete c2;
	return new_chain;
}
