/*
 * Item.hpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */

#ifndef JVM_ITEM_HPP_
#define JVM_ITEM_HPP_
#include "ByteCodes.hpp"
/**
 * 代码生成中的一些东西，例如局部变量，栈中对象，静态变量，成员变量和方法，立即数常量，条件跳转
 */
class Item{
public:
	int typecode;
	Item(int typecode){
		this->typecode = typecode;
	}

	virtual Item* load();
	virtual void  store();
	virtual void  invoke();
	virtual void  duplicate();
	virtual void  drop();
	virtual Item* coerce(int targetcode);
	Item* coerce(Type* targettype);

	int width();
};

class StackItem:Item{

};

class IndexedItem:Item{


};

class SelfItem:Item{
public:
	bool isSuper;
	SelfItem(bool isSpuer):Item(ByteCodes::OBJECTcode){
		this->isSuper = isSuper;
	}


};

class LocalItem:Item{


};
class StaticItem:Item{

};

class Member :Item{};

class ImmediateItem :Item{};

class AssinItem: Item{};

class CondItem:Item{};


class Items{
public:
	Pool* pool;
	Code* code;
	Items(Pool* pool,Code* code){
		this->pool = pool;
		this->code =code;
		voidItem = new Item(ByteCodes::VOIDcode);
		thisItem = new SelfItem(false);
		superItem = new SelfItem(true);
		for (int i = 0; i < ByteCodes::VOIDcode; i++) stackItem[i] = new StackItem(i);
		stackItem[ByteCodes::VOIDcode] = voidItem;

	}
private :
	Item* voidItem;
	Item* thisItem;
	Item* superItem;
	Item* stackItem[ByteCodes::TypeCodeCount];



};


#endif /* JVM_ITEM_HPP_ */
