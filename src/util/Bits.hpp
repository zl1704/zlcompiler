/*
 * Bits.hpp
 *
 *  Created on: 2019年7月15日
 *      Author: zl
 */

#ifndef UTIL_BITS_HPP_
#define UTIL_BITS_HPP_
#include "global.hpp"
/**
 * 位容器
 * 节省空间
 */
class Bits {
public:
	bool alive;
	Bits() {
		alive = false;
		bits = new int[1];
		memset(bits, 0, sizeof(int));
		bitslen = 1;
	}
	Bits(int* bits,int len){
		this->bits  = bits;
		this->bitslen = len;
	}
	~Bits() {
		delete bits;
	}

	void include(int x) {
		if (x < 0)
			return;
		sizeTo((((unsigned) x) >> wordshift) + 1);
		bits[((unsigned) x) >> wordshift] = bits[((unsigned) x) >> wordshift]
				| (1 << (x & wordmask));
	}

	void exclude(int x) {
		if (x < 0)
			return;
		sizeTo((((unsigned) x) >> wordshift) + 1);
		bits[((unsigned) x) >> wordshift] = bits[((unsigned) x) >> wordshift]
				& ~(1 << (x & wordmask));
	}

	/**
	 * include [start,end)
	 */
	void includeRange(int start, int end) {
		sizeTo((((unsigned) end) >> wordshift) + 1);
		for (int x = start; x < end; x++)
			bits[((unsigned) x) >> wordshift] =
					bits[((unsigned) x) >> wordshift] | (1 << (x & wordmask));
	}

	bool isMember(int x) {
		return 0 <= x && x < (bitslen << wordshift)
				&& (bits[((unsigned) x) >> wordshift] & (1 << (x & wordmask)))
						!= 0;
	}
	Bits* andSet(Bits* xs) {
		sizeTo(xs->bitslen);
		for (int i = 0; i < xs->bitslen; i++)
			bits[i] = bits[i] & xs->bits[i];
		return this;
	}
	Bits* orSet(Bits* xs) {
		sizeTo(xs->bitslen);
		for (int i = 0; i < xs->bitslen; i++)
			bits[i] = bits[i] | xs->bits[i];
		return this;
	}

	//exclude
	Bits* diffSet(Bits* xs) {
		sizeTo(xs->bitslen);
		for (int i = 0; i < xs->bitslen; i++)
			bits[i] = bits[i] & ~(xs->bits[i]);
		return this;
	}
	Bits* xorSet(Bits* xs) {
		sizeTo(xs->bitslen);
		for (int i = 0; i < xs->bitslen; i++)
			bits[i] = bits[i] ^ (xs->bits[i]);
		return this;
	}
	int nextBit(int x) {
		int windex = ((unsigned) x) >> wordshift;
		if (windex >= bitslen)
			return -1;
		int word = bits[windex] & ~((1 << (x & wordmask)) - 1);
		while (true) {
			if (word != 0)
				return (windex << wordshift) + trailingZeroBits(word);
			windex++;
			if (windex >= bitslen)
				return -1;
			word = bits[windex];
		}
	}

	Bits* dup(){

		int* newbits = new int[bitslen];
		memset(newbits,0,bitslen*sizeof(int));
		memcpy(newbits,bits,bitslen*sizeof(int));
		return new Bits(newbits,bitslen);
	}

	/**
	 * 统计0的个数
	 */
	int trailingZeroBits(int x) {
		if (x == 0)
			return 32;
		int n = 1;
		if ((x & 0xffff) == 0) {
			n += 16;
			x = ((unsigned) x) >> 16;
		}
		if ((x & 0x00ff) == 0) {
			n += 8;
			x = ((unsigned) x) >> 8;
		}
		if ((x & 0x000f) == 0) {
			n += 4;
			x = ((unsigned) x) >> 4;
		}
		if ((x & 0x0003) == 0) {
			n += 2;
			x = ((unsigned) x) >> 2;
		}
		return n - (x & 1);
	}
private:
	int* bits;
	int bitslen;
	static const int wordlen = 32;
	static const int wordshift = 5;
	static const int wordmask = wordlen - 1;

	void sizeTo(int len) {
		if (bitslen < len) {
			int* new_bits = new int[len];
			memset(new_bits, 0, len*sizeof(int));
			memcpy(new_bits, bits, bitslen*sizeof(int));
			bits = new_bits;
			delete new_bits;
			bitslen = len;

		}
	}

};

#endif /* UTIL_BITS_HPP_ */
