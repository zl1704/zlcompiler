/*
 * global.hpp
 *
 *  Created on: 2017年12月4日
 *      Author: ZL
 */

#ifndef UTIL_GLOBAL_HPP_
#define UTIL_GLOBAL_HPP_
#include <stdio.h>
#include <string.h>
#include <vector>
#include<set>
#include <map>
#include <functional>
#include <iostream>
#include <sstream>    //使用stringstream需要引入这个头文件
#include <fstream>


using namespace std;
namespace util {
static bool debug = false;


//ostringstream对象用来进行格式化的输出，常用于将各种类型转换为string类型
//ostringstream只支持<<操作符
template<typename T> string toString(const T& t) {
	ostringstream oss;  //创建一个格式化输出流
	oss << t;             //把值传递如流中
	return oss.str();
}

template<typename T> T strToNum(string str) {
	T t;
	stringstream  oss;  //创建一个格式化输出流
	oss << str;             //把值传递如流中
	oss >>t;
	return t;
}

/**
 * 类型转换
 */
//template<typename T,typename R>
//void convert(T from,R to){
//	 ostringstream oss;  //创建一个格式化输出流
//	 oss << from;             //把值传递如流中
//	 return oss.;
//
//}
template<class T, class R>
void appendList(vector<T>& from, vector<R> to) {
	int size = to.size();
	for (int i = 0; i < size; i++) {
		from.push_back(to.at(i));
	}
}
;
//
template<typename T>
vector<T> ListOf(T t) {
	vector<T> vec;
	vec.push_back(t);
	return vec;
}





}

#endif /* UTIL_GLOBAL_HPP_ */
