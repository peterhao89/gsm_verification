#pragma once
#include"A51Impl.hpp"

const vector<int> BITS_RELATED_Z0 = { 8,17,18, 29,39,40, 51,62,63 };


const vector<int> BITS_RELATED_Z0Z1 = { 7,8,16,17,18,  28,29,38,39,40,  50,51,61,62,63 };
const u64 MASK_Z0Z1 = 0xe00c01c030070180;
const u64 MASK_Z0 = 0xc008018020060100;

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


vector<u64> getLwithAlg3(u64 z0, u64 iteTime, u64 diff=0x3) {
	//iteTime=4*2^15/99
	u64 z1 = (z0 ^ diff);
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
		if (1 == (bit64(z0, step) ^ runner.getCurrentZ())) {
			for (int j = 0; j < 3; ++j) {
				if (haveUsedBit.find(hiBits[j]) == haveUsedBit.end()) {
					flipBitVal(initState, hiBits[j]);
					break;
				}
			}
		}
		for (int j = 0; j < 3; ++j)haveUsedBit.insert(hiBits[j]);		
	}
	return { initState };
}




