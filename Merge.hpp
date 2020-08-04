#pragma once
#include"A51Impl.hpp"

const vector<int> BITS_RELATED_Z0 = { 8,17,18, 29,39,40, 51,62,63 };


const vector<int> BITS_RELATED_Z0Z1 = { 7,8,16,17,18,  28,29,38,39,40,  50,51,61,62,63 };
const u64 MASK_Z0Z1 = 0xe00c01c030070180;
const u64 MASK_Z0 = 0xc008018020060100;
const u64 MASK64R1 = 0x7ffff;
const u64 MASK64R2 = 0x1fffff80000;
const u64 MASK64R3 = 0xfffffe0000000000;
struct IsdProb {

	u64 isd;
	double prob;
	IsdProb(u64 i = 0, double p = 0.0) {
		isd = i;
		prob = p;
	}
};
struct cmpIsdProb {
	bool operator()(const IsdProb& a, const IsdProb& b) const {
		if (a.prob > b.prob)
			return true;
		else if (a.prob == b.prob && a.isd > b.isd)
			return true;
		else
			return false;
	}
};

void getAllDiff(vector<int> elements, int d, vector<u64>& collector) {
	vector<int> index;
	for (int i = 0; i <= d; ++i)index.push_back(i - 1);
	bool flag = true;
	int count = 0;
	int k = d;
	while (index[0] == -1) {
		if (flag) {
			u64 elem = 0;

			for (int i = 1; i <= d; ++i) {
				setBitVal(elem, elements[index[i]], 1);
			}
			collector.push_back(elem);
			count++;
			flag = false;
		}
		index[k]++;
		if (index[k] == elements.size()) {
			index[k--] = 0;
			continue;
		}
		if (k < d) {
			index[++k] = index[k - 1];
			continue;
		}
		if (k == d)flag = true;

	}
}

IsdProb getOneIsdProb(u64 isd, vector<int> rbits, u64 ksd = 0x3, int bitNo = 2) {

	u64 totalXNumber = 1;
	totalXNumber <<= rbits.size();
	long satisfiedNumber = 0;
	for (u64 counter = 0; counter < totalXNumber; ++counter) {
		u64 initState = 0;
		for (int i = 0; i < rbits.size(); ++i) {
			if (bit64(counter, i) == 1)setBitVal(initState, rbits[i], 1);
		}
		A5_1_S100 imp(initState);
		A5_1_S100 imp1(initState ^ isd);
		bool satisfied = true;
		for (int i = 0; i < bitNo; ++i) {
			imp.doOneStep();
			imp1.doOneStep();
			if (bit64(ksd, i) != (imp.getCurrentZ() ^ imp1.getCurrentZ())) {
				satisfied = false;
				break;
			}
		}
		if (satisfied)satisfiedNumber++;
	}
	double prob = double(satisfiedNumber) / totalXNumber;
	IsdProb res = { isd,prob };
	return res;
}
/*
	std::set<IsdProb, cmpIsdProb> ddt;
	for (int i = 0; i < diffLowerThan2.size(); ++i) {
		IsdProb isdProb = getProb(diffLowerThan2[i], relatedBitsZ0Z1, 0x3, 2);
		if(isdProb.prob>0)
			ddt.insert(isdProb);
	}
*/
const vector<IsdProb> DDT0x3 = {
	{0x6000000000000000,0.75},
	{0xc000000000,0.75},
	{0x30000,0.75},
	{0xc000000000000000,0.4375},
	{0x18000000000,0.4375},
	{0x60000,0.4375},
	{0x4004000000000000,0.375},
	{0x8010000000,0.375},
	{0x20080,0.375},
	{0x4000004000000000,0.3125},
	{0x4000000010000000,0.3125},
	{0x4000000000010000,0.3125},
	{0x4000000000000080,0.3125},
	{0x2000008000000000,0.3125},
	{0x2000000000020000,0.3125},
	{0x4008000000000,0.3125},
	{0x4000000020000,0.3125},
	{0x8000010000,0.3125},
	{0x8000000080,0.3125},
	{0x4000020000,0.3125},
	{0x10020000,0.3125},
	{0x4008000000000000,0.28125},
	{0x8020000000,0.28125},
	{0x20100,0.28125},
	{0x8008000000000000,0.25},
	{0x8000000020000000,0.25},
	{0x8000000000000100,0.25},
	{0x4000008000000000,0.25},
	{0x4000000000020000,0.25},
	{0x2008000000000000,0.25},
	{0x2000000020000000,0.25},
	{0x2000000000000100,0.25},
	{0x8010000000000,0.25},
	{0x8004000000000,0.25},
	{0x8000010000000,0.25},
	{0x8000000040000,0.25},
	{0x8000000010000,0.25},
	{0x8000000000080,0.25},
	{0x8000000000000,0.25},
	{0x4000020000000,0.25},
	{0x4000000000100,0.25},
	{0x10020000000,0.25},
	{0x10000000100,0.25},
	{0x8000020000,0.25},
	{0x4020000000,0.25},
	{0x4000000100,0.25},
	{0x20040000,0.25},
	{0x20010000,0.25},
	{0x20000080,0.25},
	{0x20000000,0.25},
	{0x10000100,0.25},
	{0x40100,0.25},
	{0x10100,0.25},
	{0x100,0.25},
	{0xc000000000000,0.234375},
	{0x8000020000000,0.234375},
	{0x8000000000100,0.234375},
	{0x30000000,0.234375},
	{0x20000100,0.234375},
	{0x180,0.234375},
	{0x4000000020000000,0.21875},
	{0x4000000000000100,0.21875},
	{0x8008000000000,0.21875},
	{0x8000000020000,0.21875},
	{0x8000000100,0.21875},
	{0x20020000,0.21875},
	{0x4000000000000000,0.1875},
	{0x8000000000,0.1875},
	{0x20000,0.1875},
	{0x8000010000000000,0.125},
	{0x8000008000000000,0.125},
	{0x8000004000000000,0.125},
	{0x8000000010000000,0.125},
	{0x8000000000040000,0.125},
	{0x8000000000020000,0.125},
	{0x8000000000010000,0.125},
	{0x8000000000000080,0.125},
	{0x4000010000000000,0.125},
	{0x4000000000040000,0.125},
	{0x2000010000000000,0.125},
	{0x2000000000040000,0.125},
	{0x4010000000000,0.125},
	{0x4000000040000,0.125},
	{0x10000040000,0.125},
	{0x10000020000,0.125},
	{0x10000010000,0.125},
	{0x10000000080,0.125},
	{0x8000040000,0.125},
	{0x4000040000,0.125},
	{0x10040000,0.125},
	{0xa000000000000000,0.0625},
	{0x8004000000000000,0.0625},
	{0x8000000000000000,0.0625},
	{0x14000000000,0.0625},
	{0x10010000000,0.0625},
	{0x10000000000,0.0625},
	{0x50000,0.0625},
	{0x40080,0.0625},
	{0x40000,0.0625}
};


u64 getInteralStateByStaticZ0Z1(u64 zPattern) {
	u64 initState = rand_64() & MASK_Z0Z1; 
	A5_1_S100 runner(initState);
	vector<int> hiBits = { 18,40,63 };
	set<int> haveUsedBit;
	for (int step = 0; step < 2; ++step) {
		u64 nextMove = runner.getNextMoveMask();
		switch (nextMove)
		{
		case 0:
			hiBits[0]--;
			hiBits[1]--;
			hiBits[2]--;
			break;
		case 1:
			hiBits[1]--;
			hiBits[2]--;
			break;
		case 2:
			hiBits[0]--;
			hiBits[2]--;
			break;
		case 3:
			hiBits[0]--;
			hiBits[1]--;
			break;
		}
		runner.doOneStep();
		if (1 == (bit64(zPattern, step) ^ runner.getCurrentZ())) {
			for (int j = 0; j < 3; ++j) {
				if (haveUsedBit.find(hiBits[j]) == haveUsedBit.end()) {
					flipBitVal(initState, hiBits[j]);
					break;
				}
			}
		}
		for (int j = 0; j < 3; ++j)haveUsedBit.insert(hiBits[j]);
	}
	return initState;
}

bool checkInternalStateByStatieZ0Z1(u64 internalState, u64 zPattern) {
	A5_1_S100 runner(internalState);
	runner.doOneStep();
	runner.doOneStep();
	if (runner.getPrefix() == zPattern)
		return true;
	else
		return false;
}



struct StateAndKnown {

	u64 state, known;
	StateAndKnown(u64 st, u64 kn= MASK_Z0Z1) {
		state = st;
		known = kn;
	}
	bool operator == ( const StateAndKnown& b) const{
		return (this->known == b.known && this->state == b.state);
	}
	bool operator != (const StateAndKnown& b) const {
		return !(*this==b);
	}
	bool operator < (const StateAndKnown& b) const {
		if (this->state < b.state)
			return true;
		else if (this->state > b.state)
			return false;
		else if (this->state == b.state && this->known < b.known)
			return true;
		else
			return false;
	}
	bool operator > (const StateAndKnown& b) const {
		return !(*this < b);
	}

	bool operator >= (const StateAndKnown& b) const {
		return (*this > b || *this==b);
	}

	bool operator <= (const StateAndKnown& b) const {
		return (*this < b || *this == b);
	}

	set<int> getKnownBits() {
		set<int> knownBits;
		for (int i = 0; i < 64; ++i) {
			if (bit64(known, i) == 1) {
				knownBits.insert(i);
			}
		}
		return knownBits;
	}


	u64 getClockBits() {
		u64 clockBits = bit64(state, 8);
		clockBits |= (bit64(state, 29) << 1);
		clockBits |= (bit64(state, 51) << 2);
		return clockBits;
	}

	set<int> getKnownBitsAfterMove() {
		u64 clockBits = bit64(state, 8);
		clockBits |= (bit64(state, 29) << 1);
		clockBits |= (bit64(state, 51) << 2);
		set<int> knownBits;
		switch (clockBits)
		{
		case 7:
		case 0:
			for (int i = 0; i < 64; ++i) {
				if (i == 18 || i == 40 || i == 63)
					continue;
				if (bit64(known, i) == 1) {
					knownBits.insert(i+1);
				}
			}
			break;
		case 1:
		case 6:
			for (int i = 0; i < 64; ++i) {
				if (i == 40 || i == 63)
					continue;
				if (bit64(known, i) == 1) {
					if(i < 19 && i >= 0)knownBits.insert(i);
					else {
						knownBits.insert(i + 1);
					}
				}
			}
			break;
		case 2:
		case 5:
			for (int i = 0; i < 64; ++i) {
				if (i == 18 || i == 63)
					continue;
				if (bit64(known, i) == 1) {
					if (i < 41 && i >= 19)knownBits.insert(i);
					else {
						knownBits.insert(i + 1);
					}
				}
			}
			break;
		case 4:
		case 3:
			for (int i = 0; i < 64; ++i) {
				if (i == 18 || i == 40)
					continue;
				if (bit64(known, i) == 1) {
					if (i < 63 && i >= 41)knownBits.insert(i);
					else {
						knownBits.insert(i + 1);
					}
				}
			}
			break;
		}
		return knownBits;
	}

	//Move the current bits by a clock backward
	set<int> getKnownBitsBeforeMove(u64 clockBits) {
		set<int> knownBits;
		switch (clockBits)
		{
		case 7:
		case 0:
			for (int i = 0; i < 64; ++i) {
				if (i == 0 || i == 19 || i == 41)
					continue;
				if (bit64(known, i) == 1) {
					knownBits.insert(i - 1);
				}
			}
			break;
		case 1:
		case 6:
			for (int i = 0; i < 64; ++i) {
				if (i == 19 || i == 41)
					continue;
				if (bit64(known, i) == 1) {
					if (i < 19 && i >= 0)knownBits.insert(i);
					else {
						knownBits.insert(i - 1);
					}
				}
			}
			break;
		case 2:
		case 5:
			for (int i = 0; i < 64; ++i) {
				if (i == 0 || i == 41)
					continue;
				if (bit64(known, i) == 1) {
					if (i < 41 && i >= 19)knownBits.insert(i);
					else {
						knownBits.insert(i - 1);
					}
				}
			}
			break;
		case 4:
		case 3:
			for (int i = 0; i < 64; ++i) {
				if (i == 0 || i ==19)
					continue;
				if (bit64(known, i) == 1) {
					if (i < 63 && i >= 41)knownBits.insert(i);
					else {
						knownBits.insert(i - 1);
					}
				}
			}
			break;
		}
		return knownBits;
	}


	StateAndKnown getStateAndKnownBeforeMove(u64 clockBits) {
		u64 r1 = (state & MASK64R1);
		u64 r2 = (state & MASK64R2);
		u64 r3 = (state & MASK64R3);
		u64 k1 = (known & MASK64R1);
		u64 k2 = (known & MASK64R2);
		u64 k3 = (known & MASK64R3);
		switch (clockBits)
		{
		case 7:
		case 0:
			r1 = (r1 >> 1) & MASK64R1;
			r2 = (r2 >> 1) & MASK64R2;
			r3 = (r3 >> 1) & MASK64R3;
			k1 = (k1 >> 1) & MASK64R1;
			k2 = (k2 >> 1) & MASK64R2;
			k3 = (k3 >> 1) & MASK64R3;
			break;
		case 1:
		case 6:
			r2 = (r2 >> 1) & MASK64R2;
			r3 = (r3 >> 1) & MASK64R3;
			k2 = (k2 >> 1) & MASK64R2;
			k3 = (k3 >> 1) & MASK64R3;
			break;
		case 2:
		case 5:
			r1 = (r1 >> 1) & MASK64R1;
			r3 = (r3 >> 1) & MASK64R3;
			k1 = (k1 >> 1) & MASK64R1;
			k3 = (k3 >> 1) & MASK64R3;
			break;
		case 4:
		case 3:
			r1 = (r1 >> 1) & MASK64R1;
			r2 = (r2 >> 1) & MASK64R2;
			k1 = (k1 >> 1) & MASK64R1;
			k2 = (k2 >> 1) & MASK64R2;
			break;
		}
		return StateAndKnown((r1 | r2 | r3), (k1 | k2 | k3));
	}
};


/*
vector<u64> getLZ0Z1withAlg3(u64 z0, u64 iteTime, u64 diff = 0x3) {
	//iteTime=4*2^15/99
	set<u64> collector;
	u64 z1 = (z0 ^ diff);
	for (u64 ite = 0; ite < iteTime; ++ite) {
		u64 interalState = getInteralStateByStaticZ0Z1(z1);
		for (int i = 0; i < DDT0x3.size(); ++i) {
			if (checkInternalStateByStatieZ0Z1(interalState ^ DDT0x3[i].isd, z0)) {
				collector.insert(interalState ^ DDT0x3[i].isd);
			}
		}
	}
	vector<u64> res;
	for (set<u64>::iterator ite = collector.begin(); ite != collector.end(); ++ite) {
		res.push_back(*ite);
	}
	return res;
}*/


set<StateAndKnown> getLZ0Z1withAlg3(u64 z0, u64 iteTime, u64 diff = 0x3) {
	//iteTime=4*2^15/99
	set<u64> collector;
	u64 z1 = (z0 ^ diff);
	for (u64 ite = 0; ite < iteTime; ++ite) {
		u64 interalState = getInteralStateByStaticZ0Z1(z1);
		for (int i = 0; i < DDT0x3.size(); ++i) {
			if (checkInternalStateByStatieZ0Z1(interalState ^ DDT0x3[i].isd, z0)) {
				collector.insert(interalState ^ DDT0x3[i].isd);
			}
		}
	}
	set<StateAndKnown> res;
	for (set<u64>::iterator ite = collector.begin(); ite != collector.end(); ++ite) {
		res.insert(StateAndKnown(*ite));
	}
	return res;
}

set<StateAndKnown> getLZ0Z1withAlg4(u64 z0, u64 iteTime, u64 diff = 0x3, int beta = 6) {
	set<StateAndKnown> initSet = getLZ0Z1withAlg3(z0, iteTime, diff);
	vector<StateAndKnown> collector;
	for (int i = 0; i < beta-1; ++i) {
		collector.clear();
		set<StateAndKnown> tmpSet=initSet;
		set<StateAndKnown> tmpSet1 = getLZ0Z1withAlg3(z0, iteTime, diff);
		set_intersection(tmpSet.begin(), tmpSet.end(), tmpSet1.begin(), tmpSet1.end(), back_inserter(collector));
		initSet.clear();
		for (int j = 0; j < collector.size(); ++j) {
			initSet.insert(collector[j]);
		}
	}
	//cout << "#Intersection=" << initSet.size() << endl;
	return initSet;
}

vector<StateAndKnown> getLZ0Z1withAlg5(u64 z0, u64 iteTime, u64 diff = 0x3, int beta = 6, int gamma=2) {
	set<StateAndKnown> initSet = getLZ0Z1withAlg4(z0, iteTime, diff,beta);
	vector<StateAndKnown> collector;
	for (int i = 0; i < gamma-1; ++i) {
		collector.clear();
		set<StateAndKnown> tmpSet = initSet;
		set<StateAndKnown> tmpSet1 = getLZ0Z1withAlg4(z0, iteTime, diff, beta);
		set_union(tmpSet.begin(), tmpSet.end(), tmpSet1.begin(), tmpSet1.end(), back_inserter(collector));
		initSet.clear();
		for (int j = 0; j < collector.size(); ++j) {
			initSet.insert(collector[j]);
		}
	}
	collector.clear();
	for (set<StateAndKnown>::iterator ite = initSet.begin(); ite != initSet.end(); ++ite) {
		collector.push_back(*ite);
	}
	return collector;
}



bool checkIntersectBits(StateAndKnown a, StateAndKnown b) {
	u64 intersectKnownMask = (a.known & b.known);
	if ((intersectKnownMask & a.state) == (intersectKnownMask & b.state))
		return true;
	else
		return false;
}


vector<StateAndKnown> merge2List(vector<StateAndKnown> L0, vector<StateAndKnown> L1) {
	vector<StateAndKnown> mergedList;
	for (int i = 0; i < L0.size(); ++i) {
		u64 clockBits = L0[i].getClockBits();
		for (int j = 0; j < L1.size(); ++j) {
			StateAndKnown back1 = L1[j].getStateAndKnownBeforeMove(clockBits);
			if (checkIntersectBits(L0[i], back1))
			{
				mergedList.push_back(StateAndKnown(
					L0[i].state | back1.state,
					L0[i].known | back1.known
				));
			}
		}
	}
	return mergedList;
}







