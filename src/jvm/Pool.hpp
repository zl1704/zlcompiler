/*
 * Pool.hpp
 *
 *  Created on: 2019年7月9日
 *      Author: zz
 */

#ifndef JVM_POOL_HPP_
#define JVM_POOL_HPP_
#include "../util/HashMap.h"


class Code;
/**
 * 常量池
 */
class Pool{
public:
	static int POOL_SYM = 1;
	static int POOL_TYPE = POOL_SYM+1;
	friend class Code;
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
	int put(void* value,int type = POOL_SYM){
		int index= map->get(value);
		if(index==NULL){
			index = pp;
			map->put(value,index);
			typeMap->put(value,type);
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
	int getType(void* o){
		int type = typeMap->get(o);
		return type == NULL?-1:type;
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
	//为了快速获取常量池中对象
	HashMap<void*,int>* map;
	HashMap<void*,int>* typeMap;//存放类型

	void dble(){
		void** new_pool = new void*[size*2];
		memcpy(new_pool,pool,size*sizeof(void*));
		delete[] pool;
		pool = new_pool;

	}



};



#endif /* JVM_POOL_HPP_ */
