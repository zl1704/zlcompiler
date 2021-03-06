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
class Items;
/**
 * 局部变量
 */
class LocalVar{
public:
	friend class Code;
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
	//debug
	void print();
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
	Items* items;
	bool alive;
	int max_locals;
	int max_stack;
	int nextreg;
	int pendingStatPos;
	Chain* pendingJumps;
	bool fixedPc ;

	Code(MethodSymbol* sym,Pool* pool);
	void checkCode();
	//byte short  char int ---> int
	static int truncate(int tc);
	static int negate(int opcode);
	int curPc();
	int entryPoint();
	void resolvePending();
	//回填跳转链
	void resolve(Chain* c,int target);
	//point to current code pointer.
	void resolve(Chain* c);

	Chain* branch(int opcode);
	//对象长度
	static int width(int tc);
	static int width(Type* type);
	static int width(vector<Type*> types);
	void addLocalVar(VarSymbol* v);
	int newLocalVar(VarSymbol* v);
	int newLocalVar(Type* type);
	void setDefined(Bits* newDefined);
	void setDefined(int adr);
	void setUnDefined(int adr);
	void endScope(int adr);
	void endScopes(int adr);

	inline bool isAlive(){
		return alive || pendingJumps!=NULL;
	}
	inline void markAlive(){
		alive = true;
	}
	inline void markDead(){
		alive = false;
	}

	//附加行号表信息
	void addLineNumber(int startPc,int lineNumber);
	void statBegin(int pos);
	void markStatBegin();


	//Emit code
	void emitInvokedynamic(int meth,MethodType* mtype);
	void emitInvokespecial(int meth,MethodType* mtype);
	void emitInvokestatic(int meth,MethodType* mtype);
	void emitInvokevirtual(int meth, MethodType* mtype);
	void emitNewarray(int od, Type* type);
	void emitAnewarray(int od, Type* type);
	void emitMultianewarray(int ndims, int type, Type* arrayType);
	static int arraycode(Type* type);


	int emitJump(int op);
	void emitop0(int op);
	void emitop1(int op,int od);
	void emitop1w(int op,int od);
	void emitop1w(int op,int od1,int od2);
	void emitop2(int op,int od);
	void emitop4(int op,int od);

	void emit1(int op);
	void emit2(int op);
	void emit4(int op);
	void emitop(int op);

	//get put

	int get1(int pc);
	int get2(int pc);
	int get4(int pc);

	void put1(int pc,int od);
	void put2(int pc,int od);
	void put4(int pc,int od);
	void align(int incr);



private:
	vector<vector<int> > lineInfo;
	Source* source;
	Type* typeForPool(void* cd);
};

/**
 * 跳转链
 */
class Chain{
public:
	int pc;
	Chain *next;
	State* state;
	Chain(int pc,Chain* next,State* state);
	//合并两个跳转链
	static Chain* merge(Chain* c1,Chain* c2);

};



#endif /* JVM_CODE_HPP_ */
