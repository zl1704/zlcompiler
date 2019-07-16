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
Item* Item::load(){}
void  Item::store(){}
void  Item::invoke(){}
void  Item::duplicate(){}
void  Item::drop(){}
void Item::stash(int toscode){}
Item* Item::coerce(int targetcode){
	if(typecode==targetcode)
		return this;
	else{
		load();
		int typec = Code::truncate(typecode);
		int targetc = Code::truncate(targetcode);
		if(typec!=targetc){
			int offset = targetc>typec?targetc-1:typec;

		}

	}

}
Item* Item::coerce(Type* targettype){
	return coerce(targettype->tag);
}
int Item::width(){}

/**
 * StackItem
 */

Item* StackItem::load(){}
void  StackItem::store(){}
void  StackItem::invoke(){}
void  StackItem::duplicate(){}
void  StackItem::drop(){}
void StackItem::stash(int toscode){}
Item* StackItem::coerce(int targetcode){}
Item* StackItem::coerce(Type* targettype){}
int StackItem::width(){}
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
