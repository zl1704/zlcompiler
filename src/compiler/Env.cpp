/*
 * Env.cpp
 *
 *  Created on: 2018年1月1日
 *      Author: ZL
 */

#include "Env.hpp"



AttrContext* AttrContext::dup(Scope* scope) {
	AttrContext* info = new AttrContext();
	info->scope = scope;
	info->staticLevel = staticLevel;
	info->isSelfCall = isSelfCall;
	info->varArgs = varArgs;
	info->tvars = tvars;
	info->enclVar = enclVar;
	return info;
}
AttrContext* AttrContext::dup() {
	return dup(scope);
}
