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
/**
 * NOTE: 补测试用例...
 */
Type* ConstFold::fold2(int opcode, ConstType* left, ConstType* right) {

	switch(opcode){
		case ByteCodes::bool_and:
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)&&strToNum<jint>(right->str)));
		case ByteCodes::bool_or:
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)||strToNum<jint>(right->str)));
		case ByteCodes::iadd:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)+strToNum<jint>(right->str)));
		case ByteCodes::isub:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)+strToNum<jint>(right->str)));
		case ByteCodes::imul:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)*strToNum<jint>(right->str)));
		case ByteCodes::idiv:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)/strToNum<jint>(right->str)));
		case ByteCodes::imod:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)%strToNum<jint>(right->str)));
		case ByteCodes::iand:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)&strToNum<jint>(right->str)));
		case ByteCodes::ior:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)|strToNum<jint>(right->str)));
		case ByteCodes::ixor:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)^strToNum<jint>(right->str)));
		case ByteCodes::ishl:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)<<strToNum<jint>(right->str)));
		case ByteCodes::ishr:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)>>strToNum<jint>(right->str)));
		case ByteCodes::iushr:
			return ConstType::create(TypeTags::INT,toString<jint>(strToNum<jint>(left->str)>>>strToNum<jint>(right->str)));
		case ByteCodes::if_icmpeq://==
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)==strToNum<jint>(right->str)));
		case ByteCodes::if_icmpne://!=
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)!=strToNum<jint>(right->str)));
		case ByteCodes::if_icmplt://<
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)<strToNum<jint>(right->str)));
		case ByteCodes::if_icmpgt://>
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)>strToNum<jint>(right->str)));
		case ByteCodes::if_icmple://<=
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)<=strToNum<jint>(right->str)));
		case ByteCodes::if_icmpge://>=
			return ConstType::create(TypeTags::BOOLEAN,toString<jboolean>(strToNum<jint>(left->str)>=strToNum<jint>(right->str)));
		case ByteCodes::ladd:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)+strToNum<jlong>(right->str)));
		case ByteCodes::lsub:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)-strToNum<jlong>(right->str)));
		case ByteCodes::lmul:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)*strToNum<jlong>(right->str)));
		case ByteCodes::ldiv:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)/strToNum<jlong>(right->str)));
		case ByteCodes::lmod:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)%strToNum<jlong>(right->str)));
		case ByteCodes::land:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)&strToNum<jlong>(right->str)));
		case ByteCodes::lor:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)|strToNum<jlong>(right->str)));
		case ByteCodes::lxor:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)^strToNum<jlong>(right->str)));
		case ByteCodes::lshl:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)<<strToNum<jlong>(right->str)));
		case ByteCodes::lshr:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)>>strToNum<jlong>(right->str)));
		case ByteCodes::lushr:
			return ConstType::create(TypeTags::LONG,toString<jlong>(strToNum<jlong>(left->str)>>>strToNum<jlong>(right->str)));
//		case ByteCodes::lcmp:
		case ByteCodes::fadd:
			return ConstType::create(TypeTags::FLOAT,toString<jdouble>(strToNum<jfloat>(left->str)+strToNum<jfloat>(right->str)));
		case ByteCodes::fsub:
			return ConstType::create(TypeTags::FLOAT,toString<jdouble>(strToNum<jfloat>(left->str)-strToNum<jfloat>(right->str)));
		case ByteCodes::fmul:
			return ConstType::create(TypeTags::FLOAT,toString<jdouble>(strToNum<jfloat>(left->str)*strToNum<jfloat>(right->str)));
		case ByteCodes::fdiv:
			return ConstType::create(TypeTags::FLOAT,toString<jdouble>(strToNum<jfloat>(left->str)/strToNum<jfloat>(right->str)));
		case ByteCodes::fmod:
			return ConstType::create(TypeTags::FLOAT,toString<jdouble>(strToNum<jfloat>(left->str)%strToNum<jfloat>(right->str)));
//		case ByteCodes::fcmpg:
//		case ByteCodes::fcmpl:
		case ByteCodes::dadd:
			return ConstType::create(TypeTags::DOUBLE,toString<jdouble>(strToNum<jdouble>(left->str)+strToNum<jdouble>(right->str)));
		case ByteCodes::dsub:
			return ConstType::create(TypeTags::DOUBLE,toString<jdouble>(strToNum<jdouble>(left->str)-strToNum<jdouble>(right->str)));
		case ByteCodes::dmul:
			return ConstType::create(TypeTags::DOUBLE,toString<jdouble>(strToNum<jdouble>(left->str)*strToNum<jdouble>(right->str)));
		case ByteCodes::ddiv:
			return ConstType::create(TypeTags::DOUBLE,toString<jdouble>(strToNum<jdouble>(left->str)/strToNum<jdouble>(right->str)));
		case ByteCodes::dmod:
			return ConstType::create(TypeTags::DOUBLE,toString<jdouble>(strToNum<jdouble>(left->str)%strToNum<jdouble>(right->str)));
		default:
			return NULL;


	}


	return NULL;
}
//Type转换
Type* ConstFold::coerce(Type* et, Type* tt) {


	return et;
}
