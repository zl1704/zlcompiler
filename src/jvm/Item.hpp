/*
 * Item.hpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */

#ifndef JVM_ITEM_HPP_
#define JVM_ITEM_HPP_
#include "ByteCodes.hpp"
#include "Code.hpp"

/**
 * 代码生成中的一些东西，例如局部变量，栈中对象，静态变量，成员变量和方法，立即数常量，条件跳转
 */
class Item{

public:
	int typecode;
	Code* code;
	Item(){}
	Item(int typecode,Code* code){
		this->typecode = typecode;
		this->code = code;
	}

	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);

	int width();
};

class StackItem:public Item{
public:
	StackItem(int typecode,Code* code):Item(typecode,code){}
	virtual Item* load();
    virtual void  store();
    virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);
	int width();
};

class IndexedItem:public Item{

public:
	IndexedItem(Type* type,Code* code):Item(ByteCodes::typecode(type),code){}
	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);
	int width();
};

class SelfItem:public Item{
public:
	bool isSuper;
	SelfItem(bool isSpuer,Code* code):Item(ByteCodes::OBJECTcode,code){
		this->isSuper = isSuper;
	}
	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);
	int width();

};

class LocalItem:public Item{
public:
	int reg;
	Type* type;
	LocalItem(Type* type,int reg,Code* code):Item(ByteCodes::typecode(type),code){
		this->type = type;
		this->reg = reg;
	}
	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);
	int width();

};
/**
 * 静态变量 或 方法
 */
class StaticItem:public Item{

public:
	Symbol* member;
	StaticItem(Symbol* member,Code* code):Item(ByteCodes::typecode(member->type),code){
		this->member = member;
	}
	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);
	int width();
};
/**
 * 实例变量或方法
 */
class MemberItem :public Item{
public:
	Symbol* member;
	MemberItem(Symbol* member,Code* code):Item(ByteCodes::typecode(member->type),code){
		this->member = member;
	}
	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);
	int width();
};

class ImmediateItem :public Item{
public:
	Literal * literal;
	ImmediateItem(Type* type,Code* code):Item(ByteCodes::typecode(type),code){
		this->literal = literal;
	}
	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);
	int width();
};

class AssinItem:public Item{
public:
	//左值
	Item* lhs;
	AssinItem(Item* lhs,Code* code):Item(lhs->typecode,code){
		this->lhs = lhs;
	}
	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);
	int width();
};

class CondItem:public Item{
public:
	Tree * tree;
	int opcode;
	Chain* trueJumps;
	Chain* falseJumps;
	CondItem(int opcode,Chain* tureJumps,Chain* falseJumps,Code* code){
		this->typecode = ByteCodes::BYTEcode;
		this->trueJumps = trueJumps;
		this->opcode = opcode;
	}
	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual void stash(int toscode);
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);
	int width();
};


class Items{
public:
	Pool* pool;
	Code* code;
	Items(Pool* pool,Code* code){
		this->pool = pool;
		this->code =code;
		voidItem = new Item(ByteCodes::VOIDcode,code);
		thisItem = new SelfItem(false,code);
		superItem = new SelfItem(true,code);
		for (int i = 0; i < ByteCodes::VOIDcode; i++) stackItem[i] = new StackItem(i,code);
		stackItem[ByteCodes::VOIDcode] = voidItem;

	}

	//make Item

	Item* makeStackItem(Type* type){
		return stackItem[ByteCodes::typecode(type)];
	}

	Item* makeLocalItem(VarSymbol* v){
		return new LocalItem(v->type,v->adr,code);
	}
	Item* makeStaticItem(Symbol* member){
		return new StaticItem(member,code);
	}

	Item* makeMemberItem(Symbol* s){
		return new MemberItem(s,code);
	}

	Item* makeImmediateItem(Type* type){
		return new ImmediateItem(type,code);
	}

	Item* makeAssinItem(Item* lhs){
		return new AssinItem(lhs,code);
	}

	Item* makeCondItem(int opcode,Chain* trueJumps,Chain* falseJumps){
		return new CondItem(opcode,trueJumps,falseJumps,code);
	}

private :
	Item* voidItem;
	Item* thisItem;
	Item* superItem;
	Item* stackItem[ByteCodes::TypeCodeCount];



};


#endif /* JVM_ITEM_HPP_ */
