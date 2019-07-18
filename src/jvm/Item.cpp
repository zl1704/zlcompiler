/*
 * Item.cpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */
#include "Item.hpp"
/**
 * Item
 */
Item* Item::load(){
	return NULL;
}
void  Item::store(){

}
void  Item::invoke(){}
void  Item::duplicate(){}
void  Item::drop(){}
void Item::stash(int toscode){
	items->stackItem[toscode]->duplicate();
}
/**
 * 期望转换的类型
 * 如：现结果为int(char、 byte 、short都会当做int处理),tagert(也就是pt)为double
 * 	  这里的类型转换都都是可行的，语义分析已经检查了所有转型
 */
Item* Item::coerce(int targetcode){
	if(typecode==targetcode)
		return this;
	else{
		load();
		int typec = Code::truncate(typecode);
		int targetc = Code::truncate(targetcode);
		if(typec!=targetc){
			int offset = targetc>typec?targetc-1:typec;
			//添加一条转换类型指令
			//java字节码设计巧妙，方便计算相似指令
			code->emitop0(ByteCodes::i2l+typec*3+offset);
		}
		//double -> short :double -> int -> short
		if(targetc != targetcode){
			code->emitop0(ByteCodes::int2byte+targetcode-ByteCodes::BYTEcode);
		}
		return items->stackItem[targetcode];
	}
}
Item* Item::coerce(Type* targettype){
	return coerce(targettype->tag);
}
CondItem* Item::mkCond(){
	load();
	return items->makeCondItem(ByteCodes::ifne,NULL,NULL);
}
int Item::width(){
	return 0;
}

/**
 * StackItem
 */

Item* StackItem::load(){}
void  StackItem::store(){}
void  StackItem::invoke(){}
void  StackItem::duplicate(){}
void  StackItem::drop(){}
//Duplicate the top word to place 3.
void StackItem::stash(int toscode){
//	code->emitop0()
}
int StackItem::width(){
	return Code::width(typecode);
}
/**
 *  IndexedItem
 */
Item* IndexedItem::load(){}
void  IndexedItem::store(){}
void  IndexedItem::invoke(){}
void  IndexedItem::duplicate(){}
void  IndexedItem::drop(){}
void IndexedItem::stash(int toscode){}
Item* IndexedItem::coerce(int targetcode){}
Item* IndexedItem::coerce(Type* targettype){}
int IndexedItem::width(){}

/**
 *  SelfItem
 */
Item* SelfItem::load(){}
void  SelfItem::store(){}
void  SelfItem::invoke(){}
void  SelfItem::duplicate(){}
void  SelfItem::drop(){}
void SelfItem::stash(int toscode){}
Item* SelfItem::coerce(int targetcode){}
Item* SelfItem::coerce(Type* targettype){}
int SelfItem::width(){}
/**
 *  LocalItem
 */
Item* LocalItem::load(){}
void  LocalItem::store(){}
void  LocalItem::invoke(){}
void  LocalItem::duplicate(){}
void  LocalItem::drop(){}
void LocalItem::stash(int toscode){}
Item* LocalItem::coerce(int targetcode){}
Item* LocalItem::coerce(Type* targettype){}
int LocalItem::width(){}
/**
 * StaticItem
 */
Item* StaticItem::load(){}
void  StaticItem::store(){}
void  StaticItem::invoke(){}
void  StaticItem::duplicate(){}
void  StaticItem::drop(){}
void StaticItem::stash(int toscode){}
Item* StaticItem::coerce(int targetcode){}
Item* StaticItem::coerce(Type* targettype){}
int StaticItem::width(){}

/**
 * MemberItem
 */
Item* MemberItem::load(){}
void  MemberItem::store(){}
void  MemberItem::invoke(){}
void  MemberItem::duplicate(){}
void  MemberItem::drop(){}
void MemberItem::stash(int toscode){}
Item* MemberItem::coerce(int targetcode){}
Item* MemberItem::coerce(Type* targettype){}
int MemberItem::width(){}
/**
 * ImmediateItem
 */
Item* ImmediateItem::load(){}
void  ImmediateItem::store(){}
void  ImmediateItem::invoke(){}
void  ImmediateItem::duplicate(){}
void  ImmediateItem::drop(){}
void ImmediateItem::stash(int toscode){}
Item* ImmediateItem::coerce(int targetcode){}
Item* ImmediateItem::coerce(Type* targettype){}
int ImmediateItem::width(){}
/**
 * AssinItem
 */
Item* AssinItem::load(){}
void  AssinItem::store(){}
void  AssinItem::invoke(){}
void  AssinItem::duplicate(){}
void  AssinItem::drop(){}
void AssinItem::stash(int toscode){}
Item* AssinItem::coerce(int targetcode){}
Item* AssinItem::coerce(Type* targettype){}
int AssinItem::width(){}
/**
 * CondItem
 */
Item* CondItem::load(){}
void  CondItem::store(){}
void  CondItem::invoke(){}
void  CondItem::duplicate(){}
void  CondItem::drop(){}
void CondItem::stash(int toscode){}
Item* CondItem::coerce(int targetcode){}
Item* CondItem::coerce(Type* targettype){}
int CondItem::width(){}
