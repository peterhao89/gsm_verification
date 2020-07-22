#pragma once
#include<vector>
#include<set>
#include<algorithm>
#include <immintrin.h>
//#include<zmmintrin.h>
#include<fstream>
#include<iostream>
using namespace std;
#define LROT32(x,s)  (((x)<<s)|((x)>>(32-s)))
#define RROT32(x,s)  (((x)>>s)|((x)<<(32-s)))
#define LROT64(x,s)  (((x)<<s)|((x)>>(64-s)))
#define RROT64(x,s)  (((x)>>s)|((x)<<(64-s)))


#define bit(x,n)   (((x)>>(n))&1)


#define R1MASK 0x7ffff
#define R2MASK 0x3fffff
#define R3MASK 0x7fffff
#define LROT19(x,s) ((((x)<<s)|((x)>>(19-s)))&R1MASK)
#define LROT22(x,s) ((((x)<<s)|((x)>>(22-s)))&R2MASK)
#define LROT21(x,s) ((((x)<<s)|((x)>>(23-s)))&R3MASK)

typedef uint64_t u64;

u64 rand_64()//产生64位随机数
{
	static u64 Z[2] = { 0x375201345e7fa379, 0xcde9fe134e8af6b1 ^ (u64(rand()) << 32) ^ rand() };//(rand() << 24) + (rand() << 12) + rand() 0xcde9fb8a
	u64 temp = Z[1] ^ (Z[1] << 63) ^ (Z[0] >> 1);
	Z[0] = Z[1];
	Z[1] = temp;
	return Z[0];
}

void setBitVal(u64& reg, int bitNo, u64 val) {
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
class A5_1_S100 {
public:

	A5_1_S100(uint64_t state=0) {
		update(state);
	}

	void update(u64 state ) {
		R1 = state & R1MASK;
		R2 = (state >> 19) & R2MASK;
		R3 = (state >> (41)) & R3MASK;
	}
	u64 getWholeState() {
		return (R1 | (R2 << 19) | (R3 << 41));
	}

	u64 output() {
		clockStopGo();
		return (bit(R1,18) ^ bit(R2,21)^bit(R3,22));
	}

private:
	u64 R1,R2,R3;

	u64 maj(u64 a, u64 b, u64 c) {
		return ((a&b)^(b&c)^(a&c));
	}

	void clockStopGo() {
		u64 a1, a2, a3;
		a1 = bit(R1, 8);
		a2 = bit(R2, 10);
		a3 = bit(R3, 10);
		u64 mVal = maj(a1, a2, a3);
		if (a1 == mVal)updateR1();
		if (a2 == mVal)updateR2();
		if (a3 == mVal)updateR3();
	}

	void updateR1() {
		u64 opt = bit(R1, 18) ^ bit(R1, 17) ^ bit(R1, 16) ^ bit(R1, 13);
		R1 <<= 1;
		R1 &= R1MASK;
		setBitVal(R1, 0, opt);
	}

	void updateR2() {
		u64 opt = bit(R2, 21) ^ bit(R2, 20);
		R2 <<= 1;
		R2 &= R2MASK;
		setBitVal(R2, 0, opt);
	}

	void updateR3() {
		u64 opt = bit(R3, 22) ^ bit(R3, 21) ^ bit(R3, 20) ^ bit(R3, 7);
		R3 <<= 1;
		R3 &= R3MASK;
		setBitVal(R3, 0, opt);
	}

};


class A5_1_S100ANF {
public:

	A5_1_S100ANF(uint64_t state = 0) {
		update(state);
	}
	void update(u64 state ) {
		R1 = state & R1MASK;
		R2 = (state >> 19) & R2MASK;
		R3 = (state >> (41)) & R3MASK;
	}
	u64 output(int offset) {
		u64 x[9] = { bit(R1,18 + offset),
			bit(R2,21+ offset),
			bit(R3,22 + offset),
			bit(R1,17 + offset),
			bit(R2,20 + offset),
			bit(R3,21 + offset),
			bit(R1,8 + offset),
			bit(R2,10 + offset),
			bit(R3,10 + offset) };
		u64 res = x[3] ^ x[4] ^ x[5] ^ (x[0] & x[6]) ^ (x[3] & x[6]) ^ (x[1] & x[7]) ^ (x[4] & x[7]) ^ (x[2] & x[8]) ^ (x[5] & x[8]) ^ (x[0] & x[6] & x[7])
			^ (x[1] & x[6] & x[7]) ^ (x[2] & x[6] & x[7]) ^ (x[3] & x[6] & x[7]) ^ (x[4] & x[6] & x[7]) ^ (x[5] & x[6] & x[7]) ^ (x[0] & x[6] & x[8])
			^ (x[1] & x[6] & x[8]) ^ (x[2] & x[6] & x[8]) ^ (x[3] & x[6] & x[8]) ^ (x[4] & x[6] & x[8]) ^ (x[5] & x[6] & x[8]) ^ (x[0] & x[7] & x[8])
			^ (x[1] & x[7] & x[8]) ^ (x[2] & x[7] & x[8]) ^ (x[3] & x[7] & x[8]) ^ (x[4] & x[7] & x[8]) ^ (x[5] & x[7] & x[8]);
		return res;
	}


private:
	u64 R1, R2, R3;
};
