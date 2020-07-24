#pragma once
#include"CommonUtils.hpp"

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
		return mVal;
	}
	inline u64 getCurrentZ() {
		return (bit(R1, 18) ^ bit(R2, 21) ^ bit(R3, 22));
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



class Deductor {
public:

	Deductor(MaskAndClock mc,  int stp) {
		haveUsed = false;
		currentStep = 0;
		MaskClockParcer parcer(mc);
		initMC = mc;
		initState = 0;
		for (int i = 0; i < 64; ++i) {
			if (mc[64 + i] == 1) { 
				knownBitPositions.insert(i); 
			}
			if (mc[i] == 1)setBitVal(initState, i, 1);
		}
		bitPositions= vector<vector<int>>(3, vector<int>());
		for (int i = 0; i < 64; ++i) {
			if (i < 19)bitPositions[0].push_back(i);
			else if (i >= 19 && i < (19 + 22))bitPositions[1].push_back(i);
			else bitPositions[2].push_back(i);
		}
		clockBitInR = { {} ,{} ,{} };
		clockBitPositions = {};
		countR1 = 8;
		countR2 = 10;
		countR3 = 10;
		checkRunner=A5_1_S100(initState);		
		for (currentStep = 0; currentStep < stp; ++currentStep) {
			if (!doKnownStep())
				cout << "Step "<<currentStep<<" wrong!" << endl;;
		}
	}
	
	MaskClockSet getCollector() {
		if (!haveUsed) {
			doUnknownStep();
		}
		return collector;
	}

	string getClockValue() {
		string output = "";
		for (set<int>::iterator ite = clockBitPositions.begin(); ite != clockBitPositions.end(); ++ite) {
			output += initMC[*ite]?"1":"0";
		}
		return output;
	}

	string getReportLine() {
		string line = "";
		line += getClockBitPositions();
		line += ";";
		line += getRiPositions();
		line += ";";
		line += getClockValue();
		line += ";";
		line += getKnownBitPositions();
		line += ";";
		line += getNonClockBitPositions();
		line += ";";
		//clock bit length
		line += to_string(clockBitPositions.size());
		line += ";";
		//non-clock bit length
		line += to_string(knownBitPositions.size()-clockBitPositions.size());
		line += ";";
		//total bit length
		line += to_string(knownBitPositions.size());
		line += ";";
		return line;
	}

	string getRiPositions() {
		string output = "";
		for (int i = 0; i < 3; ++i) {
			output += getRiBits(i);
			if(i!=2)output += ";";
		}
		return output;
	}

	string getClockBitPositions() {
		string output = "|";
		for (set<int>::iterator ite = clockBitPositions.begin(); ite != clockBitPositions.end(); ++ite) {
			output += to_string(*ite);
			output += "|";
		}
		return output;
	}

	string getKnownBitPositions() {
		string output = "|";
		for (set<int>::iterator ite = knownBitPositions.begin(); ite != knownBitPositions.end(); ++ite) {
			output += to_string(*ite);
			output += "|";
		}
		return output;
	}

	string getNonClockBitPositions() {
		string output = "|";
		for (set<int>::iterator ite = knownBitPositions.begin(); ite != knownBitPositions.end(); ++ite) {
			if (clockBitPositions.find(*ite) != clockBitPositions.end()) {
				output += to_string(*ite);
				output += "|";
			}
		}
		return output;
	}



private:
	u64 initState;
	int currentStep;
	bool haveUsed;

	set<int> knownBitPositions;
	vector<vector<int>> bitPositions;
	MaskAndClock initMC;
	MaskClockSet collector;
	A5_1_S100 checkRunner;
	vector<vector<int>> clockBitInR;
	set<int> clockBitPositions;
	int countR1, countR2, countR3;

	string getRiBits(int index) {
		string output = "|";
		for (int j = 0; j < clockBitInR[index].size(); ++j) {
			output += to_string(clockBitInR[index][j]);
			output += "|";
		}
		return output;
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
		if (clockBitPositions.find(bitPositions[0][8]) == clockBitPositions.end()) {
			clockBitPositions.insert(bitPositions[0][8]);
			clockBitInR[0].push_back(countR1--);
		}
		if (clockBitPositions.find(bitPositions[1][10]) == clockBitPositions.end()) {
			clockBitPositions.insert(bitPositions[1][10]);
			clockBitInR[1].push_back(countR2--);
		}
		if (clockBitPositions.find(bitPositions[2][10]) == clockBitPositions.end()) {
			clockBitPositions.insert(bitPositions[2][10]);
			clockBitInR[2].push_back(countR3--);
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
		vector<int> counterBits;
		if (knownBitPositions.find(bitPositions[0][8]) == knownBitPositions.end()) {
			counterBits.push_back(bitPositions[0][8]);
			knownBitPositions.insert(bitPositions[0][8]);
			initMC[64 + bitPositions[0][8]] = 1;
		}
		if (knownBitPositions.find(bitPositions[1][10]) == knownBitPositions.end()) {
			counterBits.push_back(bitPositions[1][10]);
			knownBitPositions.insert(bitPositions[1][10]);
			initMC[64 + bitPositions[1][10]] = 1;
		}
		if (knownBitPositions.find(bitPositions[2][10]) == knownBitPositions.end()) {
			counterBits.push_back(bitPositions[2][10]);
			knownBitPositions.insert(bitPositions[2][10]);
			initMC[64+bitPositions[2][10]] = 1;
		}
		u64 total = 1;
		total <<= counterBits.size();
		for (u64 count = 0; count < total; ++count) {
			MaskAndClock tmpInitMC = initMC;
			u64 tmpInitState = initState;
			for (int btNo = 0; btNo < counterBits.size(); ++btNo) {
				setBitVal(tmpInitState, counterBits[btNo], bit(count, btNo));
				tmpInitMC[64 + counterBits[btNo]] = 1;
				tmpInitMC[counterBits[btNo]] = bit(count,btNo);
			}
			u64 a1, a2, a3;
			a1 = bit(tmpInitState, bitPositions[0][8]);
			a2 = bit(tmpInitState, bitPositions[1][10]);
			a3 = bit(tmpInitState, bitPositions[2][10]);
			u64 mVal = maj(a1, a2, a3);
			tmpInitMC[64 + bitPositions[0][a1 != mVal ? 18 : 17]] = 1;
			tmpInitMC[64 + bitPositions[1][a2 != mVal ? 21 : 20]] = 1;
			tmpInitMC[64 + bitPositions[2][a3 != mVal ? 22 : 21]] = 1;
			collector.insert(tmpInitMC);
		}
		
		
	}


	void rotateR1() {
		for (int i = 18; i >0; --i) {
			bitPositions[0][i] = bitPositions[0][i - 1];
		}
		bitPositions[0][0] = -1;
	}
	void rotateR2() {
		for (int i = 21; i > 0; --i) {
			bitPositions[1][i] = bitPositions[1][i - 1];
		}
		bitPositions[1][0] = -1;
	}
	void rotateR3() {
		for (int i = 22; i > 0; --i) {
			bitPositions[2][i] = bitPositions[2][i - 1];
		}
		bitPositions[2][0] = -1;
	}
};



class KnownBitsDeduction {
public:
	int totalSteps;
	int currentStep;
	KnownBitsDeduction(int stps = 1) {
		totalSteps = stps;
		currentStep = 0;
		collector.clear();
		while (currentStep < totalSteps) {
			cout << "Do Step " << currentStep << ":\n";
			updateCollector();
			cout << "Acquire " << collector.size() 
				<< " vectors corresponding to prefix=z[0-"<<(currentStep-1)<<"]\n";
		}
	}

	void outputTable(ostream & o) {
		string header = "clkPos;r1Pos;r2Pos;r3Pos;clkVal;knownPos;nclkPos;#clk;#nclk;#total;\n";
		for (MaskClockSet::iterator ite = collector.begin(); ite != collector.end(); ++ite) {
			Deductor ddct(*ite, totalSteps);
			o << ddct.getReportLine() << "\n";
		}
	}

private:

	MaskClockSet collector;


	void updateCollector() {
		if (collector.empty()) {
			MaskAndClock mc;
			Deductor ddct = Deductor(mc, 0);
			collector = ddct.getCollector();
			currentStep++;
		}
		else {
			MaskClockSet newCollector;
			for (MaskClockSet::iterator ite = collector.begin(); ite != collector.end(); ++ite) {
				Deductor one(*ite, currentStep);
				MaskClockSet msk = one.getCollector();
				for (MaskClockSet::iterator nite = msk.begin(); nite != msk.end(); ++nite) {
					newCollector.insert(*nite);
				}
			}
			collector = newCollector;
			currentStep++;
		}
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
