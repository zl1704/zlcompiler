/*
 * Code.hpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */

#ifndef JVM_CODE_HPP_
#define JVM_CODE_HPP_
#include "../util/global.hpp"
#include "../util/includeGlobal.hpp"
#include "Pool.hpp"
#include "../util/Bits.hpp"
#define MAXPC 65535
class Code;
class Chain;
/**
 * 局部变量
 */
class LocalVar{
public:
	LocalVar(){
		this->vsym = NULL;
		this->reg = -1;
		this->code = NULL;
		start_pc = 65535;
		length = 65535;
	}
	LocalVar(VarSymbol* v):vsym(v){
		this->reg = -1;
		this->code = NULL;
		start_pc = 65535;
		length = 65535;
	}
	LocalVar* dup(){
		return new LocalVar(vsym);
	}



	//用于debug变量活跃范围
	int start_pc;
	int length;
private :
	VarSymbol* vsym;
	char reg;
	Code* code;
};

/**
 * 机器状态
 * 栈中数据
 */
class State{
public:
	//存放数据寄存器集合
	Bits* defined;
	//栈中类型
	Type** stack;
	Code* code;
	int s_size;
	int stacksize;

	int* locks;
	int l_size;
	int nlocks;
	State(Code* code){
		defined = new Bits();
		stack = new Type*[16];
		memset(stack,NULL,16*4);
		s_size = 16;
		stacksize = 0;
		locks = new int[16];
		l_size = 0;
		nlocks = 0;
		this->code =code;

	}
	State(){
		defined = NULL;
		stack = NULL;
		s_size = 0;
		stacksize = 0;

		code = NULL;
		locks = NULL;
		l_size = 0;
		nlocks = 0;

	}


	~State(){
		delete defined;
		delete stack;
		delete locks;

	}
	State* dup();
	void lock(int reg);
	void unlock(int reg);
	void push(Type* t);
	Type* pop1();
	Type* peek();
	Type* pop2();
	void pop(int n);
	void pop(Type* t);
	//把栈顶转为父类t ...
	void forceStackTop(Type* t);
	State* join(State* other);

};



/**
 * 方法代码
 */
class Code{
public:
	jbyte* code;
	int size ;
	int cp;
	LocalVar** lvs;
	int lvsize;
	MethodSymbol* msym;
	Symtab* syms;
	Pool* pool;
	State* state;
	bool alive;
	int max_locals;
	int max_stack;
	int nextreg;

	Chain* pendingJumps;
	bool fixedPc ;
	Code(MethodSymbol* sym,Pool* pool){
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
		syms = Symtab::instance();
		state = new State(this);
		pendingJumps = NULL;
		fixedPc = false;
	}
	void checkCode();
	//byte short  char int ---> int
	static int truncate(int tc);
	int curPc();
	void resolvePending();
	void resolve(Chain* c);
	//回填跳转链
	void resolve(Chain* c,int target);
	//对象长度
	static int width(int tc);
	static int width(Type* type);
	void addLocalVar(VarSymbol* v);
	int newLocalVar(VarSymbol* v);
	int newLocalVar(Type* type);
	void setDefined(Bits* newDefined);
	void setDefined(int adr);
	void setUnDefined(int adr);
	void endScope(int adr);


};

/**
 * 跳转链
 */
class Chain{
public:
	int pc;
	Chain *next;
	Chain(int pc,Chain* next);
	//合并两个跳转链
	static Chain* merge(Chain* c1,Chain* c2);

};



#endif /* JVM_CODE_HPP_ */
