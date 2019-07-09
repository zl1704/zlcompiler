#ifndef HASHMAP_H
#define HASHMAP_H
#include <stdio.h>
#include <vector>
//#define kHASHSIZE 997001

template<typename KEY, typename VALUE>
class Node {
public:
	KEY getKey() {
		return key;
	}
	VALUE getValue() {
		return value;
	}

	Node() :
			next(NULL), key(), value() {
	}
	Node(int h,const KEY &k, const VALUE &v, Node* _next) :
			hash(h),key(k), value(v), next(_next) {
	}

	~Node() {
		if (next)
			delete next;
	}
	KEY key;
	VALUE value;
	Node *next;
	int hash;
};

template<typename KEY, typename VALUE> class HashMap {
public:
	HashMap() {
		_size = 16;
		table = new Node<KEY, VALUE>*[_size];
		memset(table, 0, _size * sizeof(Node<KEY, VALUE>*));
		nelems = 0;
	}
	int size() {
		return _size;
	}
	bool empty();
	bool put(KEY key, VALUE v) {
		int h = hash(key);
		int i = index(h);
		for (Node<KEY, VALUE>* e = table[i]; e; e = e->next) {
			if (e->hash == h) {
				e->value = v;
				return false;
			}
		}
		makeEntry(h, key, v, i);

	}

	VALUE get(KEY key) {
		int h = hash(key);
		int i = index(h);
		for (Node<KEY, VALUE>* e = table[i]; e; e = e->next) {
			if (e->hash == h) {
				return e->value;
			}
		}
		return NULL;
	}

	void clear();

private:
	unsigned long long hash(KEY key) {
		return (unsigned long long) (key);
	}
	;
	int index(int h) {
		return h & (_size - 1);
	}
	void makeEntry(int h, KEY key, VALUE v, int i) {
		if (nelems >= (0.75 * _size * 2) && NULL != table[i]) {
			dble();
			i = index(h);
		}
		Node<KEY, VALUE>* e= table[i];
		table[i] = new Node<KEY, VALUE>(h,key,v,e);
		nelems++;

	}
	void dble() {
		Node<KEY, VALUE>** old_table = table;
		int oldsize = _size;
		_size = _size * 2;
		table = new Node<KEY, VALUE>*[_size];
		memset(table, 0, _size * sizeof(Node<KEY, VALUE>*));
		//重新计算hash
		Node<KEY, VALUE>* e;
		for (int i = 0; i < oldsize; i++) {
			e = old_table[i];
			while (e) {
				Node<KEY, VALUE>* next = e->next;
				e->hash = hash(e->key);
				int i = index(e->hash);
				e->next = table[i];
				table[i] = e;
				e = next;

			}
		}
		delete[] old_table;

	}
//  std::vector<Node<KEY, VALUE>*> head;
	Node<KEY, VALUE>** table;
	int _size;
	int nelems;
};

#endif
