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
	return coerce(ByteCodes::typecode(targettype));
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

		code->emitop0(ByteCodes::dup_x2 + 3 * (Code::width(toscode) - 1));
	} else {
		code->emitop0(ByteCodes::dup_x1 + 3 * (Code::width(toscode) - 1));

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
	code->emitop0(ByteCodes::iastore + typecode);
}
void  IndexedItem::duplicate(){
	code->emitop0(ByteCodes::dup2);
}
void  IndexedItem::drop(){
	code->emitop0(ByteCodes::pop2);
}
//数组存储前调用，将数据复制到arr index下
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
	if(_virtual)
		code->emitInvokevirtual(code->pool->put(member), mt);
	else
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
//load from constant pool
void ImmediateItem::ldc(){
	int index = code->pool->put(ctype,Pool::POOL_TYPE);
	if(typecode == ByteCodes::LONGcode || typecode == ByteCodes::DOUBLEcode){
		//2 byte ,数据占两个位置
		code->emitop2(ByteCodes::ldc2w,index);
	}else if(index<255){
		code->emitop1(ByteCodes::ldc1,index);
	}else{
		//索引超过255的，
		code->emitop2(ByteCodes::ldc2,index);
	}

}
bool ImmediateItem::isPosZero(jfloat x){
	 return x == 0.0f && 1.0f / x > 0.0f;
}
bool ImmediateItem::isPosZero(jdouble x){
	return x == 0.0d && 1.0d / x > 0.0d;
}
CondItem* ImmediateItem::mkCond() {
	int ival = util::strToNum<int>(ctype->str);
	if (ival != 0) {
		return items->makeCondItem(ByteCodes::goto_, NULL, NULL);

	} else {
		return items->makeCondItem(ByteCodes::dontgoto, NULL, NULL);

	}
}
Item* ImmediateItem::load(){
	jint ival;
	jlong lval;
	jfloat fval;
	jdouble dval;
	switch(typecode){
	case ByteCodes::BYTEcode:
	case ByteCodes::CHARcode:
	case ByteCodes::SHORTcode:
	case ByteCodes::INTcode:
		ival = util::strToNum<jint>(ctype->str);
		if (ival >= -1 && ival <= 5)
			code->emitop0(ByteCodes::iconst_0 + ival);
		else if (-128 <= ival && ival <= 127)
			code->emitop1(ByteCodes::bipush, ival);
		else if (-32768 <= ival && ival <= 32767)
			code->emitop1w(ByteCodes::sipush, ival);
		else
			ldc();  //从常量池中获取
		break;
	case ByteCodes::LONGcode:
		lval = util::strToNum<jlong>(ctype->str);
		if (lval == 0 || lval == 1)
			code->emitop0(ByteCodes::lconst_0 + lval);
		else
			ldc();
		break;
	case ByteCodes::FLOATcode:
		fval = util::strToNum<jfloat>(ctype->str);
		if (isPosZero(fval) || fval == 1.0 || fval == 2.0)
			code->emitop0(ByteCodes::fconst_0 + (int) fval);
		else
			ldc();
		break;
	case ByteCodes::DOUBLEcode:
		dval = util::strToNum<jdouble>(ctype->str);
		if (isPosZero(dval) || dval == 1.0)
			code->emitop0(ByteCodes::dconst_0 + (int) dval);
		else
			ldc();
		break;
	case ByteCodes::OBJECTcode:
		ldc();
		break;
	}
	return items->stackItem[typecode];
}
Item* ImmediateItem::coerce(int targetcode) {
	if (typecode == targetcode) {
		return this;
	}

	switch (targetcode) {
	case ByteCodes::INTcode:
		if (Code::truncate(typecode) == ByteCodes::INTcode)
			return this;
		else
			return items->makeImmediateItem(
					ConstType::create(TypeTags::INT, ctype->str));
	case ByteCodes::LONGcode:
		return items->makeImmediateItem(
				ConstType::create(TypeTags::LONG, ctype->str));
	case ByteCodes::DOUBLEcode:
		return items->makeImmediateItem(
				ConstType::create(TypeTags::DOUBLE, ctype->str));
	case ByteCodes::FLOATcode:
		return items->makeImmediateItem(
				ConstType::create(TypeTags::FLOAT, ctype->str));
	case ByteCodes::CHARcode:
		return items->makeImmediateItem(
				ConstType::create(TypeTags::CHAR, ctype->str));
	case ByteCodes::SHORTcode:
		return items->makeImmediateItem(
				ConstType::create(TypeTags::SHORT, ctype->str));
	case ByteCodes::BYTEcode:
		return items->makeImmediateItem(
				ConstType::create(TypeTags::BYTE, ctype->str));
	default:
		return Item::coerce(targetcode);

	}

}
/**
 * AssinItem
 */
Item* AssinItem::load(){

	lhs->stash(typecode);
	lhs->store();
	return items->stackItem[typecode];

}
void  AssinItem::duplicate(){
	load()->duplicate();
}
void  AssinItem::drop(){
	lhs->store();
}
void AssinItem::stash(int toscode){
	cout << "错误调用！！！" <<endl;
}
int AssinItem::width(){
	return lhs->width() + Code::width(typecode);
}
/**
 * CondItem
 */
//这里暂时不太明白
Item* CondItem::load(){

	Chain* trueChain = NULL;
	Chain* falseChain = jumpFalse();
	if(!isFalse()){
		code->resolve(trueJumps);
		code->emitop0(ByteCodes::iconst_1);//?
		trueChain = code->branch(ByteCodes::goto_);
	}
	if(falseChain!=NULL){
		code->resolve(falseChain);
		code->emitop0(ByteCodes::iconst_0);
	}
	code->resolve(trueChain);
	return items->stackItem[typecode];
}
void  CondItem::drop(){
	load()->drop();
}
void CondItem::stash(int toscode){}
void  CondItem::duplicate(){
	load()->duplicate();
}
CondItem* CondItem::mkCond(){
	return this;
}
CondItem* CondItem::negate(){

	CondItem* c = items->makeCondItem(Code::negate(opcode),falseJumps,trueJumps);
	c->tree = tree;
	return c;
}
/**
 * 当前输出一个跳转指令，并与false链合并
 */
Chain* CondItem::jumpFalse(){

//	if(tree == NULL)
		return Chain::merge(falseJumps,code->branch(Code::negate(opcode)));
//	int startpc = code->curPc();
//	Chain* c = Chain::merge(falseJumps,code->branch(opcode));
//	return c;
}
Chain* CondItem::jumpTrue(){
	if (tree == NULL)
		return Chain::merge(trueJumps, code->branch(opcode));
	int startpc = code->curPc();
	Chain* c = Chain::merge(trueJumps, code->branch(opcode));
	return c;
}


bool CondItem::isTrue(){
	return falseJumps== NULL && opcode == ByteCodes::goto_;
}
bool CondItem::isFalse(){
	return trueJumps== NULL && opcode == ByteCodes::dontgoto;
}
