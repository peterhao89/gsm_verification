#pragma once
#include<vector>
#include<set>
#include<algorithm>
#include <immintrin.h>
//#include<zmmintrin.h>
#include<map>
#include<fstream>
#include<iostream>
using namespace std;
#define LROT32(x,s)  (((x)<<s)|((x)>>(32-s)))
#define RROT32(x,s)  (((x)>>s)|((x)<<(32-s)))
#define LROT64(x,s)  (((x)<<s)|((x)>>(64-s)))
#define RROT64(x,s)  (((x)>>s)|((x)<<(64-s)))
#define STATE_LENGTH 64

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
class A5_1_S100 {
public:

	A5_1_S100(uint64_t state=0) {
		update(state);
	}

	inline void update(u64 state ) {
		R1 = state & R1MASK;
		R2 = (state >> 19) & R2MASK;
		R3 = (state >> (41)) & R3MASK;
	}
	inline u64 getWholeState() {
		return (R1 | (R2 << 19) | (R3 << 41));
	}

	inline u64 output() {
		clockStopGo();
		return (bit(R1,18) ^ bit(R2,21)^bit(R3,22));
	}

	u64 R1,R2,R3;

	inline u64 getCurrentMaj() {
		u64 a1, a2, a3;
		a1 = bit(R1, 8);
		a2 = bit(R2, 10);
		a3 = bit(R3, 10);
		u64 mVal = maj(a1, a2, a3);
		return getCurrentMaj();
	}
	inline u64 getCurrentZ() {
		return (bit(R1, 18) ^ bit(R2, 21) ^ bit(R3, 22));
	}

	inline u64 maj(u64 a, u64 b, u64 c) {
		return ((a&b)^(b&c)^(a&c));
	}

	inline void clockStopGo() {
		u64 a1, a2, a3;
		a1 = bit(R1, 8);
		a2 = bit(R2, 10);
		a3 = bit(R3, 10);
		u64 mVal = maj(a1, a2, a3);
		if (a1 == mVal)updateR1();
		if (a2 == mVal)updateR2();
		if (a3 == mVal)updateR3();
	}

	inline void updateR1() {
		u64 opt = bit(R1, 18) ^ bit(R1, 17) ^ bit(R1, 16) ^ bit(R1, 13);
		R1 <<= 1;
		R1 &= R1MASK;
		setBitVal(R1, 0, opt);
	}

	inline void updateR2() {
		u64 opt = bit(R2, 21) ^ bit(R2, 20);
		R2 <<= 1;
		R2 &= R2MASK;
		setBitVal(R2, 0, opt);
	}

	inline void updateR3() {
		u64 opt = bit(R3, 22) ^ bit(R3, 21) ^ bit(R3, 20) ^ bit(R3, 7);
		R3 <<= 1;
		R3 &= R3MASK;
		setBitVal(R3, 0, opt);
	}

private:
};


class KnownBitsDeduction {
public:
	u64 prefix;
	int prefixLength;
	int totalSteps;
	KnownBitsDeduction(u64 prfx = 0, int prfxL = 0, int stps = 1) {
		totalSteps = stps;
		prefix = prfx;
		prefixLength = prfxL;
	}

private:
	map<u64, set<u64>> collector;
};

class Deductor {
	u64 prefix;
	u64 initState;
	int haveDoneSteps;
	u64 knownBitMask;
	
	Deductor(u64 stat, int stp, u64 msk, u64 pfx) {
		prefix = pfx;
		initState = stat;
		haveDoneSteps = stp;
		knownBitMask = msk;
		bitPositions= vector<vector<int>>(3, vector<int>());
		for (int i = 0; i < 64; ++i) {
			if (i < 19)bitPositions[0].push_back(i);
			else if (i >= 19 && i < (19 + 22))bitPositions[1].push_back(i);
			else bitPositions[2].push_back(i);
		}
		knownBitPositions = getKnownBits();
		checkRunner=A5_1_S100(initState);
		bool checkKnownSteps = true;
		for (int i = 0; i < haveDoneSteps; ++i) {
			checkKnownSteps= doKnownStep();
		}
	}
	bool doKnownStep() {
		bool ok = true;
		if (knownBitPositions.find(bitPositions[0][8]) == knownBitPositions.end()
			|| knownBitPositions.find(bitPositions[1][10]) == knownBitPositions.end()
			|| knownBitPositions.find(bitPositions[2][10]) == knownBitPositions.end()
		) {
			cout << "There is unknown bits in stop_go positions!\n";
			ok = false;
		}
		u64 majVal = checkRunner.getCurrentMaj();
		u64 a1 = bit(checkRunner.R1, 8);
		u64 a2 = bit(checkRunner.R2, 10);
		u64 a3 = bit(checkRunner.R3, 10);
		if (a1 == majVal) {
			checkRunner.updateR1();
			rotateR1();
			if (knownBitPositions.find(bitPositions[0][18]) == knownBitPositions.end()) {
				cout << "R1 position is unknown!\n";
				ok = false;
			}
		}
		if (a2 == majVal) {
			checkRunner.updateR2();
			rotateR2();
			if (knownBitPositions.find(bitPositions[1][21]) == knownBitPositions.end()) {
				cout << "R2 position is unknown!\n";
				ok = false;
			}
		}
		if (a3 == majVal) {
			checkRunner.updateR3();
			rotateR3();
			if (knownBitPositions.find(bitPositions[2][22]) == knownBitPositions.end()) {
				cout << "R3 position is unknown!\n";
				ok = false;
			}
		}
		return ok;
	}

	void doUnknownStep() {
		u64 newMask = knownBitMask;
		vector<int> counterBits;
		if (knownBitPositions.find(bitPositions[0][8]) == knownBitPositions.end()) {
			counterBits.push_back(bitPositions[0][8]);
			setBitVal(newMask, bitPositions[0][8], 1);
			knownBitPositions.insert(bitPositions[0][8]);
		}
		if (knownBitPositions.find(bitPositions[1][10]) == knownBitPositions.end()) {
			counterBits.push_back(bitPositions[1][10]);
			setBitVal(newMask, bitPositions[1][10], 1);
			knownBitPositions.insert(bitPositions[1][10]);
		}
		if (knownBitPositions.find(bitPositions[2][10]) == knownBitPositions.end()) {
			counterBits.push_back(bitPositions[2][10]);
			setBitVal(newMask, bitPositions[2][10],1);
			knownBitPositions.insert(bitPositions[2][10]);
		}
		u64 total = 1;
		total <<= counterBits.size();
		for (u64 count = 0; count < total; ++count) {
			u64 tmpInitState = initState;
			for (int btNo = 0; btNo < counterBits.size(); ++btNo) {
				setBitVal(tmpInitState, counterBits[btNo], bit(count, btNo));
			}
		}
		
		
	}



private:
	set<int> knownBitPositions;
	vector<vector<int>> bitPositions;
	A5_1_S100 checkRunner;
	void rotateR1() {
		for (int i = 18; i >0; --i) {
			bitPositions[0][i] = bitPositions[0][i - 1];
		}
		bitPositions[0][0] = -1;
	}
	void rotateR2() {
		for (int i = 22; i > 0; --i) {
			bitPositions[1][i] = bitPositions[1][i - 1];
		}
		bitPositions[1][0] = -1;
	}
	void rotateR3() {
		for (int i = 23; i > 0; --i) {
			bitPositions[2][i] = bitPositions[2][i - 1];
		}
		bitPositions[2][0] = -1;
	}




	set<int> getKnownBits() {
		set<int> knownBits;
		for (int i = 0; i < STATE_LENGTH; ++i) {
			if(bit(knownBitMask,i)==1)knownBits.insert(i);
		}
		return knownBits;
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
