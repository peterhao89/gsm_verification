#pragma once
#include<vector>
#include<set>
#include<algorithm>
#include <immintrin.h>
//#include<zmmintrin.h>
#include<map>
#include<fstream>
#include<iostream>
#include<bitset>
#include<string>

#define bit64(x,n)   (((x)>>(n))&1)
using namespace std;

typedef uint64_t u64;
typedef bitset<128> MaskAndClock;

struct cmpBitset128 {
	bool operator()(const MaskAndClock& a, const MaskAndClock& b) const {
		for (int i = 0; i < 128; i++) {
			if (a[i] < b[i])
				return true;
			else if (a[i] > b[i])
				return false;
		}
		return false;
	}
};

typedef set<MaskAndClock, cmpBitset128> MaskClockSet;



u64 rand_64()//产生64位随机数
{
	static u64 Z[2] = { 0x375201345e7fa379, 0xcde9fe134e8af6b1 ^ (u64(rand()) << 32) ^ rand() };//(rand() << 24) + (rand() << 12) + rand() 0xcde9fb8a
	u64 temp = Z[1] ^ (Z[1] << 63) ^ (Z[0] >> 1);
	Z[0] = Z[1];
	Z[1] = temp;
	return Z[0];
}


inline void setBitVal(u64& reg, int bitNo, u64 val) {
	u64 msk = 1;
	msk <<= bitNo;
	if (val != 0) {
		reg |= msk;
	}
	else {
		msk = ~msk;
		reg &= msk;
	}
}


inline void flipBitVal(u64& reg, int bitNo) {
	u64 msk = 1;
	msk <<= bitNo;
	reg ^= msk;
}

inline u64 maj(u64 a, u64 b, u64 c) {
	return ((a & b) ^ (b & c) ^ (a & c));
}

//Mask the known bit positions and clock are the clockbit values
class MaskClockParcer {
public:
	MaskClockParcer(MaskAndClock mm) {
		mask = 0;
		clock = 0;
		for (int i = 0; i < 64; ++i) {
			setBitVal(clock, i, mm[i]);
			setBitVal(mask, i, mm[64+i]);
		}

	}

	MaskAndClock getMaskAndClock() {
		MaskAndClock mm;
		for (int i = 0; i < 64; ++i) {
			if (bit64(mask, i) == 0)mm[64 + i] = 0;
			else mm[64 + i] = 1;
			if (bit64(clock, i) == 0)mm[i] = 0;
			else mm[i] = 1;
		}
		return mm;
	}


	u64 mask, clock;
};
