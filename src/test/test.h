/*
 * test.h
 *
 *  Created on: 2019年7月8日
 *      Author: zz
 */

#ifndef TEST_TEST_H_
#define TEST_TEST_H_
#include "../util/HashMap.h"
#include "../util/Bits.hpp"
#include <iostream>
using namespace std;
void testmap(){
	HashMap<Compiler*,int >* map = new HashMap <Compiler*,int >();
	Compiler* cp[40];
	for(int i = 0;i<40;i++){

		cp[i] = new Compiler();
		map->put(cp[i],i);
	}
	for(int i =0;i<40;i++){

		cout << map->get(cp[i]) <<endl;
	}
//	Compiler* comp1 = new Compiler();
//	Compiler* comp2 = new Compiler();
//	Compiler* comp3 = new Compiler();
//
//	map->put(comp1,1);
//	map->put(comp2,2);
//	map->put(comp3,3);
//	cout << map->get(comp1)<< endl;
//	cout << map->get(comp2)<< endl;
//	cout << map->get(comp3)<< endl;
//	map->put(comp3,2);
//	cout << map->get(comp3)<< endl;
}
void testBits(){

	Bits* bits = new Bits();
	if(!bits->alive)
		cout << "false"<<endl;
//	cout << bits->isMember(5)<<endl;
//	bits->include(5);
//	cout << bits->isMember(5)<<endl;
//	cout << bits->isMember(55)<<endl;
//	bits->include(55);
//	cout << bits->isMember(55)<<endl;
//	bits->exclude(55);
//	cout << bits->isMember(55)<<endl;

}
#endif /* TEST_TEST_H_ */
