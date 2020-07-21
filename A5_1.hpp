#pragma once
#include<vector>
#include<set>
#include<algorithm>
#include <immintrin.h>
#include<zmmintrin.h>
#include<fstream>
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

class A5_1_S100 {
public:

	A5_1_S100(uint64_t state=0) {
		R1 = state & R1MASK;
		R2 = (state >> 19) & R2MASK;
		R3 = (state >> (41)) & R3MASK;
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
		setBitVal(R1, 0, opt);
	}

	void updateR2() {
		u64 opt = bit(R2, 21) ^ bit(R2, 20);
		R2 <<= 1;
		setBitVal(R2, 0, opt);
	}

	void updateR3() {
		u64 opt = bit(R3, 22) ^ bit(R3, 21) ^ bit(R3, 20) ^ bit(R3, 7);
		R3 <<= 1;
		setBitVal(R3, 0, opt);
	}

	void setBitVal(u64& reg, int bitNo, u64 val) {
		u64 msk = 1;
		msk <<= bitNo;
		if (val == 0) {
			reg |= msk;
		}
		else {
			msk= ~msk;
			reg &= msk;
		}
	}
};