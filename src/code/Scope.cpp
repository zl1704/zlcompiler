/*
 * Scope.cpp
 *
 *  Created on: 2017年12月30日
 *      Author: ZL
 */

#include "Scope.hpp"
Entry* Scope::sentinel = new Entry(NULL, NULL, NULL, NULL);
Scope::Scope(Symbol* owner) {
	table = new Entry*[INITIAL_SIZE];
	memset(table, 0, INITIAL_SIZE * sizeof(Entry*));
	nelems = 0;
	size = INITIAL_SIZE;
	this->owner = owner;
	next = NULL;
	shared = 0;
}

Scope::Scope(Scope* next, Symbol* owner, Entry** table, int size) :
		next(next), owner(owner), table(table), size(size) {
	nelems = 0;
	shared = 0;
}

Scope::Scope(Scope* next, Symbol* owner, Entry** table, int nelems, int size) :
		next(next), owner(owner), table(table), size(size), nelems(nelems) {
	shared = 0;
}

Scope* Scope::dup(bool shared) {

	return dup(this->owner,shared);
}
Scope* Scope::dup(Symbol* newOwner,bool shared) {
	Entry** new_table = new Entry*[INITIAL_SIZE];
	memset(new_table, 0, INITIAL_SIZE * sizeof(Entry*));
	if(shared)
		memcpy(table, table, this->size);
	return new Scope(this, newOwner, new_table, 0, INITIAL_SIZE);
}
//共享复制
Scope* Scope::dupShared() {
	Entry** new_table = new Entry*[this->size];
	memset(new_table, 0, this->size * sizeof(Entry*));
	memcpy(table, table, this->size);
	return new Scope(this, owner, new_table, 0, this->size);
}

Scope* Scope::leave() {
	if (table != next->table)
		return next;
}
void Scope::enter(Symbol* sym) {
	enter(sym, this);
}
void Scope::enter(Symbol* sym, Scope* s) {
	enter(sym, s, s);
}
//
void Scope::enter(Symbol* key, Scope* value, Scope * origin) {
//	cout<<"before enter\t";print();
	int h = hash(key->name);
	int i = index(h);

	for (Entry* e = table[i]; e; e = e->next) {
		if (e->hash == h && key->name.compare(e->sym->name) == 0) {
			break;
		}
	}
	makeEntry(h, key, value, i);
	if(util::debug){
		cout << "scope: \t after enter\t";
		print();
		cout << endl;
	}
}
void Scope::makeEntry(int hash, Symbol* sym, Scope* scope, int bucketIndex) {
	if (nelems >= (0.75 * size * 2) && NULL != table[bucketIndex]) {
		dble();
		bucketIndex = index(hash);
	}
	Entry* e = table[bucketIndex];
	table[bucketIndex] = new Entry(hash, sym, scope, e);
	nelems++;
}

/**
 * 扩充 table
 */
void Scope::dble() {
	Entry** old_table = table;
	int oldsize = size;
	size = size * 2;
	table = new Entry*[size];
	memset(table, 0, size * sizeof(Entry*));
//	memcpy(table, old_table, oldsize*sizeof(Entry*));
	//size 扩大 hash也要重新计算，并不只是简单的拷贝
	transfer(old_table, oldsize);

	delete[] old_table;

}
void Scope::transfer(Entry** oldtable, int size) {
	Entry* e;
	for (int i = 0; i < size; i++) {
		e = oldtable[i];
		while (e) {
			Entry* next = e->next;
			e->hash = hash(e->sym->name);
			int i = index(e->hash);
			e->next = table[i];
			table[i] = e;
			e = next;
		}
	}
}

Entry* Scope::findLocal(string name) {
	int h = hash(name);
	Entry* e = table[index(h)];
	return e;
}

Entry* Scope::find(string name) {
	Entry* e = findLocal(name);
	if (e == NULL && this->next != NULL) {
		return this->next->find(name); // recursive lookup
	}
	return e;

}

//使用链式法
Entry* Scope::getEntry(string name) {
	int h = hash(name);
	int index = h & (size - 1);

	return NULL;
}
int Scope::index(int h) {
	return h & (size - 1);
}
//字符串hash
int Scope::hash(string str) {
	int h = 0;
	const char* c = str.data();
	for (; *c; c++) {
		h = 31 * h + *c;
	}
	return abs(h);
}
//debug
void Scope::print() {
	if (next)
		next->print();
	Entry * e = NULL;
	cout << "[";
	for (int i = 0; i < size; i++) {
		e = table[i];
		while (e) {
			cout << e->sym->name;
			e = e->next;
			cout << ",";
		}
	}
	cout << "]";

}

Scope::~Scope() {
	// TODO Auto-generated destructor stub
}

