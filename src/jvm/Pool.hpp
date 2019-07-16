/*
 * Pool.hpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */

#ifndef JVM_POOL_HPP_
#define JVM_POOL_HPP_
#include "../util/HashMap.h"
/**
 * 常量池
 */
class Pool{
public:
	Pool(){
		pp= 1;
		size = 16;
		pool = new void*[size];
		memset(pool,NULL,size*sizeof(void*));
		map = new HashMap<void*,int>();

	}
	int numEntries(){
		return pp;
	}
	int put(void* value){
		int index= map->get(value);
		if(index==NULL){
			index = pp;
			map->put(value,index);
			if(pp==size)dble();
			pool[pp++]= value;
			// Long 和Double 情况要多空出一个位置
			/*
			 * if(value)
			 * {
			 * if(pp==size)dble();
				pool[pp++]= NULL;
			 *
			 * }
			 */

		}
		return index;
	}
	int get(void* o){
		int index = map->get(o);
		return index ==NULL?-1:index;
	}

private :
	//下一个存放位置指针
	int pp;
	//常量池大小
	int size;
	void** pool;
	//为了快速常量池中对象
	HashMap<void*,int>* map;

	void dble(){
		void** new_pool = new void*[size*2];
		memcpy(new_pool,pool,size*sizeof(void*));
		delete[] pool;
		pool = new_pool;

	}



};



#endif /* JVM_POOL_HPP_ */
