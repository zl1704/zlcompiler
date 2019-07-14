/*
 * Item.cpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */
#include "Item.hpp"

Item* Item::load(){}
void  Item::store(){}
void  Item::invoke(){}
void  Item::duplicate(){}
void  Item::drop(){}
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

