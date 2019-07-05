/*
 * ConstFold.cpp
 *
 *  Created on: 2019年3月21日
 *      Author: zz
 */
#include "ConstFold.hpp"


int* ConstFold::ctkey = new int;
ConstFold* ConstFold::instance() {
	ConstFold* cfolder = (ConstFold*) Context::get(ctkey);
	if (cfolder == NULL){
		cfolder = new ConstFold();
	}
	return cfolder;

}
ConstFold::ConstFold(){
	Context::put(ctkey,this);

}

//折叠一元操作
Type* ConstFold::fold1(int opcode, ConstType* t) {
	switch(opcode){
	case ByteCodes::nop:
		return t;
	case ByteCodes::ineg://-
		return ConstType::create(TypeTags::INT,toString<jint>(-strToNum<jint>(t->str)));
	case ByteCodes::ixor://~
		return ConstType::create(TypeTags::INT,toString<jint>(~strToNum<jint>(t->str)));
	case ByteCodes::bool_not://!
		return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(!strToNum<jboolean>(t->str)));
	case ByteCodes::ifeq://==0
		return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jboolean>(t->str)==0));
	case ByteCodes::lneg:
		return ConstType::create(TypeTags::INT,toString<jlong>(-strToNum<jlong>(t->str)));
	case ByteCodes::lxor:
		return ConstType::create(TypeTags::INT,toString<jlong>(~strToNum<jlong>(t->str)));
	case ByteCodes::fneg:
		return ConstType::create(TypeTags::INT,toString<jfloat>(-strToNum<jfloat>(t->str)));;
	case ByteCodes::dneg:;
		return ConstType::create(TypeTags::INT,toString<jdouble>(-strToNum<jdouble>(t->str)));;
	default:
		return NULL;
	}
	return t;
}
//折叠二元操作
Type* ConstFold::fold2(int opcode, ConstType* left, ConstType* right) {

	switch(opcode){
		case ByteCodes::bool_and:
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)&&strToNum<jint>(left->str)));
		case ByteCodes::bool_or:
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)||strToNum<jint>(left->str)));
		case ByteCodes::iadd:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)+strToNum<jint>(left->str)));
		case ByteCodes::isub:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)+strToNum<jint>(left->str)));
		case ByteCodes::imul:
		case ByteCodes::idiv:
		case ByteCodes::imod:
		case ByteCodes::iand:
		case ByteCodes::ior:
		case ByteCodes::ixor:
		case ByteCodes::ishl:
		case ByteCodes::ishr:
		case ByteCodes::iushr:
		case ByteCodes::if_icmpeq:
		case ByteCodes::if_icmpne:
		case ByteCodes::if_icmplt:
		case ByteCodes::if_icmpgt:
		case ByteCodes::if_icmple:
		case ByteCodes::if_icmpge:

		default:
			return NULL;




	}


	return left;
}
//Type转换
Type* ConstFold::coerce(Type* et, Type* tt) {


	return et;
}
