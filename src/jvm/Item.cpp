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
Item*  Item::invoke(){
	return NULL;
}
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

Item* StackItem::load(){
	return this;
}
//void  StackItem::store(){}
//void  StackItem::invoke(){}
void StackItem::duplicate() {
	if (width() == 2) {
		code->emitop0(ByteCodes::dup2);
	} else {
		code->emitop0(ByteCodes::dup);

	}
}
void StackItem::drop() {
	if (width() == 2) {
		code->emitop0(ByteCodes::pop2);
	} else {
		code->emitop0(ByteCodes::pop);

	}
}
/*
 * dup_x1,x2 dup2_x1,x2
 * 根据toscode决定使用dup_x ，dup2_x
 */
void StackItem::stash(int toscode) {
	if (width() == 2) {
		code->emitop0(ByteCodes::dup2_x2 + 3 * (Code::width(toscode) - 1));
	} else {
		code->emitop0(ByteCodes::dup2_x1 + 3 * (Code::width(toscode) - 1));

	}
}
int StackItem::width(){
	return Code::width(typecode);
}
/**
 *  IndexedItem
 *  数组访问
 */
Item* IndexedItem::load(){
	code->emitop0(ByteCodes::iaload + typecode);
	return items->stackItem[typecode];
}
void  IndexedItem::store(){
	code->emitop0(ByteCodes::istore + typecode);
}
void  IndexedItem::duplicate(){
	code->emitop0(ByteCodes::dup2);
}
void  IndexedItem::drop(){
	code->emitop0(ByteCodes::pop2);
}
void IndexedItem::stash(int toscode){
	 code->emitop0(ByteCodes::dup_x2 + 3 * (Code::width(toscode) - 1));
}
//Item* IndexedItem::coerce(int targetcode){}
//Item* IndexedItem::coerce(Type* targettype){}
int IndexedItem::width(){
	return 2;
}

/**
 *  SelfItem
 */
Item* SelfItem::load(){
	code->emitop0(ByteCodes::aload_0);
	return items->stackItem[typecode];
}
//void  SelfItem::store(){}
//void  SelfItem::invoke(){}
//void  SelfItem::duplicate(){}
//void  SelfItem::drop(){}
//void SelfItem::stash(int toscode){}
//Item* SelfItem::coerce(int targetcode){}
//Item* SelfItem::coerce(Type* targettype){}
//int SelfItem::width(){}

/**
 *  LocalItem
 */
Item* LocalItem::load(){

	if(reg<=3)
		code->emitop0(ByteCodes::iload_0+Code::truncate(typecode)*4+reg);
	else
		code->emitop1w(ByteCodes::iload+Code::truncate(typecode),reg);
	return items->stackItem[typecode];
}
void  LocalItem::store(){
	if(reg<=3)
		code->emitop0(ByteCodes::istore_0+Code::truncate(typecode)*4+reg);
	else
		code->emitop1w(ByteCodes::istore+Code::truncate(typecode),reg);
}
void LocalItem::incr(int x){
	/*
	 * op | od1(index) | od2(incr number)
	 * 1B |   1B	   |  2B
	 */
	if(typecode==ByteCodes::INTcode && x>=-32768 &&x<=32767){
		code->emitop1w(ByteCodes::iinc,reg,x);
	}else{
		//通过add,sub指令实现
		load();
		if(x>=0){
			items->makeImmediateItem(ConstType::create(TypeTags::INT,util::toString(x)))->load();
			code->emitop0(ByteCodes::iadd);
		}else{
			items->makeImmediateItem(ConstType::create(TypeTags::INT,util::toString(-x)))->load();

		}
		items->makeStackItem(code->syms->intType)->coerce(typecode);
	}


}

/**
 * StaticItem
 */
Item* StaticItem::load(){
	code->emitop2(ByteCodes::getstatic,code->pool->put(member));
	return items->stackItem[typecode];
}
void  StaticItem::store(){
	code->emitop2(ByteCodes::putstatic,code->pool->put(member));
}
Item*  StaticItem::invoke(){
	//方法调用
	MethodType* mt = (MethodType*) member->type;
	int rescode = ByteCodes::typecode(mt->restype);
	code->emitInvokestatic(code->pool->put(member),mt);
	return items->stackItem[rescode];
}

/**
 * MemberItem
 */
Item* MemberItem::load(){
	code->emitop2(ByteCodes::getfield, code->pool->put(member));
	return items->stackItem[typecode];
}
void MemberItem::store() {
	code->emitop2(ByteCodes::putfield, code->pool->put(member));
}
Item* MemberItem::invoke() {
	//方法调用
	MethodType* mt = (MethodType*) member->type;
	int rescode = ByteCodes::typecode(mt->restype);
	//没有接口和继承，全部special调用
	code->emitInvokespecial(code->pool->put(member), mt);
	return items->stackItem[rescode];
}
void  MemberItem::duplicate(){
	items->stackItem[ByteCodes::OBJECTcode]->duplicate();
}
void  MemberItem::drop(){
	items->stackItem[ByteCodes::OBJECTcode]->drop();
}
void MemberItem::stash(int toscode){
	items->stackItem[ByteCodes::OBJECTcode]->stash(toscode);
}
int MemberItem::width(){
	return 1;
}
/**
 * ImmediateItem
 */
Item* ImmediateItem::load(){}
void  ImmediateItem::store(){}
Item*  ImmediateItem::invoke(){}
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
Item*  AssinItem::invoke(){}
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
Item*  CondItem::invoke(){}
void  CondItem::duplicate(){}
void  CondItem::drop(){}
void CondItem::stash(int toscode){}
Item* CondItem::coerce(int targetcode){}
Item* CondItem::coerce(Type* targettype){}
int CondItem::width(){}
