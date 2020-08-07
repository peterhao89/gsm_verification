#pragma once
#include"CommonUtils.hpp"
using namespace std;
#define LROT32(x,s)  (((x)<<s)|((x)>>(32-s)))
#define RROT32(x,s)  (((x)>>s)|((x)<<(32-s)))
#define LROT64(x,s)  (((x)<<s)|((x)>>(64-s)))
#define RROT64(x,s)  (((x)>>s)|((x)<<(64-s)))
#define STATE_LENGTH 64



#define R1MASK 0x7ffff
#define R2MASK 0x3fffff
#define R3MASK 0x7fffff
#define LROT19(x,s) ((((x)<<s)|((x)>>(19-s)))&R1MASK)
#define LROT22(x,s) ((((x)<<s)|((x)>>(22-s)))&R2MASK)
#define LROT21(x,s) ((((x)<<s)|((x)>>(23-s)))&R3MASK)



class A5_1_S100 {
public:

	A5_1_S100(uint64_t state=0) {
		init(state);
	}

	inline void init(u64 state ) {
		R1 = state & R1MASK;
		R2 = (state >> 19) & R2MASK;
		R3 = (state >> (41)) & R3MASK;
		haveDoneSteps = 0;
		haveDoneMoveMask = 0;
		prefix = 0;
		haveDoneClock = { 0,0 };
	}
	inline u64 getWholeState() {
		return (R1 | (R2 << 19) | (R3 << 41));
	}

	inline u64 getPrefix() {
		return prefix;
	}

	inline u64 getHaveDoneMoveMask() {
		return haveDoneMoveMask;
	}

	inline vector<u64> getHaveDoneClock() {
		return haveDoneClock;
	}

	inline u64 getCurrentZ() {
		return (bit64(R1, 18) ^ bit64(R2, 21) ^ bit64(R3, 22));
	}

	void doOneStep() {
		clockStopGo();
		u64 z = getCurrentZ();
		prefix |= (z << haveDoneSteps);
		++haveDoneSteps;
	}

	//stepNo: 0,1,2,3,...,haveDoneSteps-1
	u64 getOneStepMoveMask(int stepNo) {
		if (haveDoneSteps == 0)
			cerr << "Haven't done any step yet!\n";
		if(stepNo>=haveDoneSteps || stepNo<0)
			cerr << "Illegal step No"<<stepNo<<"\n";
		u64 stepMask = (0x3 & (haveDoneMoveMask >> (2 * (stepNo))));
		return stepMask;
	}

	u64 getOneStepClock(int stepNo) {
		if (haveDoneSteps == 0)
			cerr << "Haven't done any step yet!\n";
		if (stepNo >= haveDoneSteps || stepNo < 0)
			cerr << "Illegal step No" << stepNo << "\n";
		u64 stepClock = bitW64(haveDoneClock, 3 * stepNo)|
			(bitW64(haveDoneClock, 3 * stepNo +1)<<1)|
			(bitW64(haveDoneClock, 3 * stepNo + 2) << 2);
		return stepClock;
	}

	u64 getNextMoveMask() {
		u64 a1, a2, a3;
		a1 = bit64(R1, 8);
		a2 = bit64(R2, 10);
		a3 = bit64(R3, 10);
		u64 mVal = maj(a1, a2, a3);
		u64 msk = 0;
		if (a1 != mVal)msk = 1;
		if (a2 != mVal)msk = 2;
		if (a3 != mVal)msk = 3;
		return msk;
	}

	u64 getNextClock() {
		u64 a1, a2, a3;
		a1 = bit64(R1, 8);
		a2 = bit64(R2, 10);
		a3 = bit64(R3, 10);
		return (a1|(a2<<1)|(a3<<2));
	}


	

	u64 getLastMoveMask() {
		if (haveDoneSteps == 0)
			cerr << "Haven't done any step yet!\n";
		u64 lastMask = (0x3 & (haveDoneMoveMask >> (2 * (haveDoneSteps - 1))));
		return lastMask;
	}

	u64 getLastClock() {
		if (haveDoneSteps == 0)
			cerr << "Haven't done any step yet!\n";
		u64 stepClock = getOneStepClock(haveDoneSteps - 1);
		return stepClock;
	}

	u64 getRiBit(int i, int bitNo) {
		switch (i)
		{
		case 1:
			return bit64(R1, bitNo);
			break;
		case 2:
			return bit64(R2, bitNo);
			break;
		case 3:
			return bit64(R3, bitNo);
			break;
		default:
			return 0;
			break;
		}
	}
	inline u64 getCurrentMaj() {
		u64 a1, a2, a3;
		a1 = bit64(R1, 8);
		a2 = bit64(R2, 10);
		a3 = bit64(R3, 10);
		u64 mVal = maj(a1, a2, a3);
		return mVal;
	}
private:

	inline u64 output() {
		clockStopGo();
		return (bit64(R1,18) ^ bit64(R2,21)^bit64(R3,22));
	}

	

	
	

	inline void clockStopGo() {
		u64 a1, a2, a3;
		a1 = bit64(R1, 8);
		a2 = bit64(R2, 10);
		a3 = bit64(R3, 10);
		vector <u64> clks = { a1,a2,a3 };
		u64 mVal = maj(a1, a2, a3);
		u64 thisMoveMask = 0;
		if (a1 == mVal)updateR1();
		else thisMoveMask = 1;

		if (a2 == mVal)updateR2();
		else thisMoveMask = 2;
		
		if (a3 == mVal)updateR3();
		else thisMoveMask = 3;
		thisMoveMask <<= (2 * haveDoneSteps);
		haveDoneMoveMask |= thisMoveMask;
		for (int i = 0; i < 3; ++i) {
			setBitVal(haveDoneClock[(3 * haveDoneSteps + i) / 64], (3 * haveDoneSteps + i) % 64, clks[i]);
		}
	}




	inline void updateR1() {
		u64 opt = bit64(R1, 18) ^ bit64(R1, 17) ^ bit64(R1, 16) ^ bit64(R1, 13);
		R1 <<= 1;
		R1 &= R1MASK;
		setBitVal(R1, 0, opt);
	}

	inline void updateR2() {
		u64 opt = bit64(R2, 21) ^ bit64(R2, 20);
		R2 <<= 1;
		R2 &= R2MASK;
		setBitVal(R2, 0, opt);
	}

	inline void updateR3() {
		u64 opt = bit64(R3, 22) ^ bit64(R3, 21) ^ bit64(R3, 20) ^ bit64(R3, 7);
		R3 <<= 1;
		R3 &= R3MASK;
		setBitVal(R3, 0, opt);
	}

private:
	u64 R1, R2, R3;
	u64 prefix;
	int haveDoneSteps;
	u64 haveDoneMoveMask;
	vector<u64> haveDoneClock;
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
		u64 x[9] = { bit64(R1,18 + offset),
			bit64(R2,21+ offset),
			bit64(R3,22 + offset),
			bit64(R1,17 + offset),
			bit64(R2,20 + offset),
			bit64(R3,21 + offset),
			bit64(R1,8 + offset),
			bit64(R2,10 + offset),
			bit64(R3,10 + offset) };
		u64 res = x[3] ^ x[4] ^ x[5] ^ (x[0] & x[6]) ^ (x[3] & x[6]) ^ (x[1] & x[7]) ^ (x[4] & x[7]) ^ (x[2] & x[8]) ^ (x[5] & x[8]) ^ (x[0] & x[6] & x[7])
			^ (x[1] & x[6] & x[7]) ^ (x[2] & x[6] & x[7]) ^ (x[3] & x[6] & x[7]) ^ (x[4] & x[6] & x[7]) ^ (x[5] & x[6] & x[7]) ^ (x[0] & x[6] & x[8])
			^ (x[1] & x[6] & x[8]) ^ (x[2] & x[6] & x[8]) ^ (x[3] & x[6] & x[8]) ^ (x[4] & x[6] & x[8]) ^ (x[5] & x[6] & x[8]) ^ (x[0] & x[7] & x[8])
			^ (x[1] & x[7] & x[8]) ^ (x[2] & x[7] & x[8]) ^ (x[3] & x[7] & x[8]) ^ (x[4] & x[7] & x[8]) ^ (x[5] & x[7] & x[8]);
		return res;
	}


private:
	u64 R1, R2, R3;
};
