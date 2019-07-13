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


	}

}
Item* Item::coerce(Type* targettype){
	return coerce(targettype->tag);
}
int Item::width(){}

