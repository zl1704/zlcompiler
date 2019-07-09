/*
 * Item.hpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */

#ifndef JVM_ITEM_HPP_
#define JVM_ITEM_HPP_
/**
 * 代码生成中的一些东西，例如局部变量，栈中对象，静态变量，成员变量和方法，立即数常量，条件跳转
 */
class Item{

};

class StackItem:Item{

};

class IndexedItem:Item{


};

class SelfItem:Item{

};

class LocalItem:Item{


};
class StaticItem:Item{

};

class Member :Item{};

class ImmediateItem :Item{};

class AssinItem: Item{};

class CondItem:Item{};
#endif /* JVM_ITEM_HPP_ */
