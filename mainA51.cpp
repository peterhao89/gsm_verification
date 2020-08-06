#include<string>
#include<omp.h>
#include<cstring>
#include<time.h>
#define TEST 0
#define MERGE 0
#define GEN_GUESS_TABLE 0
#define PRACTICAL_ATTACK 1
#define DDT_GENERATE 0

#if GEN_GUESS_TABLE
#include"GuessDetermineCP.hpp"
#elif PRACTICAL_ATTACK 
#include"A51Impl.hpp"
#include"PracticalAttack.hpp"
#endif

#if DDT_GENERATE
#include"Merge.hpp"



int main(){
	srand(time(NULL));


	int totalStep = 5;
	u64 initState = rand_64();
	A5_1_S100 correctRunner(initState);
	for (int i = 0; i < totalStep; ++i)
		correctRunner.doOneStep();
	cout << "Correct internal state: " << hex << initState << endl;
	cout <<"Correct "<<totalStep<<"-bit Prefix: "<< hex << correctRunner.getPrefix() << dec << endl;
	u64 prefix = correctRunner.getPrefix();
	u64 iterTime = (1<<17)/99;
	u64 diff = 0x3;
	int beta = 7;
	int gamma = 2;

	



	//1st level
	vector<StateAndKnown> Lz0z1= getLZ0Z1withAlg5( prefix & 0x3, iterTime, diff, beta, gamma);
	vector<StateAndKnown> Lz1z2 = getLZ0Z1withAlg5( (prefix >> 1) & 0x3, iterTime, diff, beta, gamma);
	vector<StateAndKnown> Lz2z3 = getLZ0Z1withAlg5( (prefix >> 2) & 0x3, iterTime, diff, beta, gamma);
	vector<StateAndKnown> Lz3z4 = getLZ0Z1withAlg5( (prefix >> 3) & 0x3, iterTime, diff, beta, gamma);
	cout << dec << "#Lz0z1=" << Lz0z1.size() << endl;
	cout << dec << "#Lz1z2=" << Lz1z2.size() << endl;
	cout << dec << "#Lz2z3=" << Lz2z3.size() << endl;
	cout << dec << "#Lz3z4=" << Lz3z4.size() << endl;


	//2nd level
	vector<StateAndKnown> Lz0z1z2 = merge2List(Lz0z1, Lz1z2);
	cout <<dec<<"#Lz0z1z2="<< Lz0z1z2.size() << endl;
	for (int i = 0; i < Lz0z1z2.size(); ++i) {
		A5_1_S100 check(Lz0z1z2[i].state);
		for (int r = 0; r < totalStep; ++r) {
			check.doOneStep();
		}
		if ((check.getPrefix()&0x7) != (prefix&0x7))
			cout << "Prefix=" << hex << check.getPrefix() << endl;
	}

	Lz0z1.clear();
	vector<StateAndKnown> Lz1z2z3 = merge2List(Lz1z2, Lz2z3);
	cout << dec << "#Lz1z2z3=" << Lz1z2z3.size() << endl;
	Lz1z2.clear();
	vector<StateAndKnown> Lz2z3z4 = merge2List(Lz2z3, Lz3z4);
	cout << dec << "#Lz2z3z4=" << Lz2z3z4.size() << endl;
	Lz2z3.clear();
	Lz3z4.clear();

	//3rd level
	vector<StateAndKnown> Lz0z1z2z3 = merge2List(Lz0z1z2, Lz1z2z3);
	cout << dec << "#Lz0z1z2z3=" << Lz0z1z2z3.size() << endl;
	Lz0z1z2.clear();
	vector<StateAndKnown> Lz1z2z3z4 = merge2List(Lz1z2z3, Lz2z3z4);
	cout << dec << "#Lz1z2z3z4=" << Lz1z2z3z4.size() << endl;
	Lz1z2z3.clear();
	Lz2z3z4.clear();

	//4th level
	vector<StateAndKnown> Lz0z1z2z3z4 = merge2List(Lz0z1z2z3, Lz1z2z3z4);
	cout << dec << "#Lz0z1z2z3z4=" << Lz0z1z2z3z4.size() << endl;
	Lz0z1z2z3.clear();
	Lz1z2z3z4.clear();


	for (int i = 0; i < Lz0z1z2z3z4.size(); ++i) {
		A5_1_S100 check(Lz0z1z2z3z4[i].state);
		for (int stp = 0; stp < totalStep; ++stp)
			check.doOneStep();
		if (check.getPrefix() != prefix)
			cout << "Prefix=" << hex << check.getPrefix() << endl;
	}
	bool success = false;
	for (int i = 0; i < Lz0z1z2z3z4.size(); ++i) {
		if ((initState & Lz0z1z2z3z4[i].known) == (Lz0z1z2z3z4[i].state & Lz0z1z2z3z4[i].known)) {
			success = true;
			set<int> knownBits = Lz0z1z2z3z4[i].getKnownBits();
			cout << "Successfully recovered the internal state bits at positions: ";
			for (set<int>::iterator ite = knownBits.begin(); ite != knownBits.end(); ++ite) {
				cout << dec << *ite << ",";
			}
			cout << dec << endl;
			cout << "That's " << knownBits.size() << " in total!\n";
		}
	}
	if (!success)
		cout << "Haven't recovered the internal state!\n";
	
	/*
	vector<StateAndKnown> coll = getLZ0Z1withAlg3(z0, iterTime, 0x3);
	cout <<dec<< "Collect " << coll.size() << " in total\n";
	cout << "z0=" << hex << z0 << endl;
	for (int i = 0; i < coll.size(); ++i) {
		A5_1_S100 check(coll[i].state);
		check.doOneStep();
		check.doOneStep();
		if(check.getPrefix()!=z0)
			cout << "Prefix=" << hex << check.getPrefix() << endl;
	}
	*/





	return 0;
}

#endif





#if PRACTICAL_ATTACK
#include"Merge.hpp"

u64 sum64(u64 vec) {
	u64 summation = 0;
	for (int i = 0; i < 64; ++i)
		summation ^= bit64(vec, i);
	return summation;
}





//guess: 0 for correct guess; otherwise wrong guess
int testOnceOrder64(long guess) {
	u64 initState = rand_64();
	PracticalAttack attack = PracticalAttack();
	A5_1_S100 orderCheckRunnter(initState);
	int currentHaveDoneStep = 0;
	do {
		orderCheckRunnter.doOneStep();
		u64 thisMove;
		if (guess == 0) {
			thisMove = orderCheckRunnter.getLastMoveMask();
		}
		else {
			do {
				thisMove = rand_64() & 0x3;
			} while (thisMove == orderCheckRunnter.getLastMoveMask());
		}

		attack.doOneMove(thisMove, orderCheckRunnter.getCurrentZ());
		++currentHaveDoneStep;
		bool passCurrent = attack.isFeasible();
		if (!passCurrent) {
			break;
		}
	} while (attack.matOrder != 64 && currentHaveDoneStep < 32);
	return currentHaveDoneStep;
}



RR getOrder64AvgSteps(long guess, long testTime=(1<<20)) {
	RR counter = to_RR(1);
	RR collector = to_RR(0);
	RR totalTestTime = to_RR(testTime);
	for (; counter < totalTestTime; ++counter) {
		collector += to_RR(testOnceOrder64(guess));
	}
	return collector / totalTestTime;
}

struct FilterStrengthAtRoundReport{
	long testTime;
	u64 matOrderTotal;
	long passedTimeTotal;
};

FilterStrengthAtRoundReport getFilterStrengthAtRound(long guess, int round, long testTime = (1 << 20), int threadNumber=6) {
	u64 maskMask = 0;
	for (int i = 0; i < round; ++i) {
		maskMask <<= 2;
		maskMask |= 0x3;
	}
	long passedCount = 0;
	u64 totalOrder = 0;
	omp_set_num_threads(threadNumber);
#pragma omp parallel for reduction(+:passedCount) reduction(+:totalOrder)
	for (int i = 0; i < threadNumber; ++i) {
		u64 initState = rand_64();
		A5_1_S100 orderCheckRunnter(initState);
		for (long testCount = i; testCount < testTime; testCount += threadNumber) {

			orderCheckRunnter.init(initState);
			for (int r = 0; r < round; ++r) {
				orderCheckRunnter.doOneStep();
			}
			u64 movesMask = orderCheckRunnter.getHaveDoneMoveMask();
			if (guess != 0) {
				do {
					movesMask = rand_64() & maskMask;
				} while (movesMask == orderCheckRunnter.getHaveDoneMoveMask());
			}
			PracticalAttack attack = PracticalAttack();
			attack.constructEquations(movesMask,
				orderCheckRunnter.getPrefix(),
				round);
			if (attack.isFeasible()) {
				++passedCount;
			}
			totalOrder += attack.matOrder;
		}
	}

	return FilterStrengthAtRoundReport{ testTime, totalOrder,passedCount };
}

FilterStrengthAtRoundReport getFilterStrengthAtRoundAfter5(long guess, int round, long testTime = (1 << 20), int threadNumber = 6) {
	u64 maskMask = 0;
	for (int i = 0; i < round; ++i) {
		maskMask <<= 2;
		maskMask |= 0x3;
	}
	u64 haveDone = 0x3ff;

	long passedCount = 0;
	u64 totalOrder = 0;
	omp_set_num_threads(threadNumber);
#pragma omp parallel for reduction(+:passedCount) reduction(+:totalOrder)
	for (int i = 0; i < threadNumber; ++i) {
		
		for (long testCount = i; testCount < testTime; testCount += threadNumber) {
			vector<vector<StateAndKnown>> levelLists = {
			{StateAndKnown(rand_64() & MASK_Z0Z1),StateAndKnown(rand_64() & MASK_Z0Z1),StateAndKnown(rand_64() & MASK_Z0Z1),
				StateAndKnown(rand_64() & MASK_Z0Z1)},
			{},{},{}
			};
			for (int level = 0; level < 3; ++level) {
				for (int i = 0; i < (3 - level); ++i) {
					u64 move0 = levelLists[level][i].getClockBits();
					StateAndKnown tmp = levelLists[level][i + 1].getStateAndKnownBeforeMove(move0);
					u64 orMask = tmp.known | levelLists[level][i].known;
					StateAndKnown vec2Add = StateAndKnown(levelLists[level][i].state |
						(tmp.state & (orMask ^ levelLists[level][i].known)), orMask);
					levelLists[level + 1].push_back(vec2Add);
				}
			}
			StateAndKnown cpPart = levelLists[3][0];
			u64 initState = cpPart.state | (rand_64() & (~cpPart.known));

			A5_1_S100 orderCheckRunnter(initState);
			orderCheckRunnter.init(initState);
			for (int r = 0; r < round; ++r) {
				orderCheckRunnter.doOneStep();
			}
			u64 movesMask = orderCheckRunnter.getHaveDoneMoveMask();
			if (guess != 0) {
				do {
					movesMask ^= rand_64() & (maskMask^haveDone);
				} while (movesMask == orderCheckRunnter.getHaveDoneMoveMask());
			}
			PracticalAttack attack = PracticalAttack(96+33);
			attack.constructEquations(movesMask,
				orderCheckRunnter.getPrefix(),
				round);
			u64 bitPos = 1;
			for (int i = 0; i < 64; ++i) {
				if ((bitPos & cpPart.known) != 0) {
					attack.setOneEquation(bitPos, bit64(cpPart.state, i));
				}
				bitPos <<= 1;
			}
			if (attack.isFeasible()) {
				++passedCount;
			}
			totalOrder += attack.matOrder;
		}
	}
	return FilterStrengthAtRoundReport{ testTime, totalOrder,passedCount };
}

inline long getRemained(int round, int threadNumber) {
	
	
	u64 initState = rand_64();
	A5_1_S100 orderCheckRunnter(initState);
	orderCheckRunnter.init(initState);
	for (int r = 0; r < round; ++r) {
		orderCheckRunnter.doOneStep();
	}
	long passedCount = 0;
	u64 prefix = orderCheckRunnter.getPrefix();
	u64 correctMask = orderCheckRunnter.getHaveDoneMoveMask();
	
	u64 totalMaskNumber = 1;
	totalMaskNumber <<= (2 * round);
	omp_set_num_threads(threadNumber);
#pragma omp parallel for reduction(+:passedCount)
	for (int i = 0; i < threadNumber; ++i) {
		for (u64 wrongMask = i; wrongMask < totalMaskNumber; wrongMask = wrongMask + threadNumber) {
			PracticalAttack attack = PracticalAttack();
			attack.constructEquations(wrongMask, prefix, round);
			if (attack.isFeasible()) {
				passedCount++;
			}
		}
	}

	return passedCount;
}


int main() {
	srand_64(time(NULL));
	long testTime = 1;
	testTime <<= 10;
	vector<vector<StateAndKnown>> level1Lists = {
		{
			StateAndKnown(rand_64() & MASK_Z0Z1),
			StateAndKnown(rand_64() & MASK_Z0Z1),
			StateAndKnown(rand_64() & MASK_Z0Z1),
			StateAndKnown(rand_64() & MASK_Z0Z1)
		},
		{},{},{}
	};
	for (int level = 0; level < 3; ++level) {
		cout <<dec<< "Level " << level + 1 << endl;
		for (int i = 0; i < (3 - level); ++i) {
			u64 move0 = level1Lists[level][i].getClockBits();
			StateAndKnown tmp = level1Lists[level][i + 1].getStateAndKnownBeforeMove(move0);
			u64 orMask = tmp.known | level1Lists[level][i].known;
			StateAndKnown vec2Add = StateAndKnown(level1Lists[level][i].state |
				(tmp.state & (orMask ^ level1Lists[level][i].known)), orMask);
			level1Lists[level+1].push_back(vec2Add);
			std::set<int> knSet = vec2Add.getKnownBits();
			cout << hex << "Move " << move0 << endl;
			cout << dec << "Total: " << knSet.size() << endl;
			cout << dec << "Known Bits:";
			for (std::set<int>::iterator ite = knSet.begin(); ite != knSet.end(); ++ite) {
				cout << dec << *ite << ",";
			}
			cout << endl;
		}
	}





#if EXACT_FILTER_AT_ROUND
	int threadNumber = 6;
	ofstream file1("ExactRoundFilterEval.txt");
	for (int round = 14; round < 17; ++round) {
		long passedCount = getRemained(round,threadNumber);
		u64 total = 1;
		total <<= (2 * round);
		for (int i = 0; i < 2; ++i) {
			ostream& o = (i == 0) ? cout : file1;
			o << dec << round << ";" << passedCount << ";" << total << endl;
		}
	}
	file1.close();
#endif
	
#ifndef ROUNDFILTER_RANDOM_WRONG_GUESSES 
#define ROUNDFILTER_RANDOM_WRONG_GUESSES 1
#endif

#if ROUNDFILTER_RANDOM_WRONG_GUESSES==1
	string header = "Rd;#pss;#matOrder;#test";
	ofstream file1("RoundFilterEval.txt");
	file1 << header << endl;
	for (int round = 6; round < 30; ++round) {
		FilterStrengthAtRoundReport mm = getFilterStrengthAtRoundAfter5(1, round, testTime);
			//getFilterStrengthAtRound(1, round, testTime);
		for (int i = 0; i < 2; ++i) {
			ostream& o = (i == 0) ? cout : file1;
			o << dec <<round<<";"<< mm.passedTimeTotal << ";"<<mm.matOrderTotal <<";"<< mm.testTime << endl;
		}
	}
	file1.close();
#endif


#if AVGROUNDS_RIGHT_OR_WRONG
	clock_t end;
	clock_t start = clock();
	RR avgCorrect = getOrder64AvgSteps(0, testTime);
	RR avgWrong = getOrder64AvgSteps(1, testTime);
	end = clock();

	ofstream file1("WrongCorrectStepDiff.txt");
	for (int i = 0; i < 2; ++i) {
		ostream & o = (i == 0) ? cout : file1;
		o << "Total:" << testTime << endl;
		o << "Correct:" << avgCorrect << endl;
		o << "Wrong:" << avgWrong << endl;
		o << "Time:" << (end - start) << "ms" << endl;
	}
	file1.close();
#endif

	return 0;
}

#endif

#if GEN_GUESS_TABLE

int main() {
	int bitNumber = 5;
	KnownBitsDeduction dducer(5);
	ofstream tabFile;
	tabFile.open("L" + to_string(bitNumber) + "ClockBitRelation.txt");
	dducer.outputTable(tabFile);
	tabFile.close();
	cout << "Finish prefix of size " << bitNumber << "\n";

	/*
	MaskAndClock mc;
	Deductor ddct= Deductor(mc, 0);
	MaskClockSet collector = ddct.getCollector();
	cout << collector.size() << endl;
	int count = 0;
	for (MaskClockSet::iterator ite = collector.begin(); ite != collector.end(); ++ite) {
		cout << "No. " << count++ << endl;
		for (int i = 0; i < 64; ++i) {
			if ((*ite)[64 + i] == 1)cout << i << ",";
		}
		cout << endl;


		Deductor one(*ite, 1);
		MaskClockSet msk = one.getCollector();
		cout << msk.size() << endl;
	}


	*/

		return 0;
}

#endif


#if MERGE
#include"Merge.hpp"

int main(int argc, char const* argv[]) {
	srand_64(time(NULL));



	int totalStep = 5;
	u64 initState = rand_64();
	A5_1_S100 correctRunner(initState);
	for (int i = 0; i < totalStep; ++i)
		correctRunner.doOneStep();
	cout << "Correct internal state: " << hex << initState << endl;
	cout << "Correct " << totalStep << "-bit Prefix: " << hex << correctRunner.getPrefix() << dec << endl;
	u64 prefix = correctRunner.getPrefix();
	u64 iterTime = (1 << 17) / 99;
	u64 diff = 0x3;
	int beta =6;
	int gamma = 2;





	//1st level
	vector<StateAndKnown> Lz0z1 = getLZ0Z1withAlg5(prefix & 0x3, iterTime, diff, beta, gamma);
	vector<StateAndKnown> Lz1z2 = getLZ0Z1withAlg5((prefix >> 1) & 0x3, iterTime, diff, beta, gamma);
	vector<StateAndKnown> Lz2z3 = getLZ0Z1withAlg5((prefix >> 2) & 0x3, iterTime, diff, beta, gamma);
	vector<StateAndKnown> Lz3z4 = getLZ0Z1withAlg5((prefix >> 3) & 0x3, iterTime, diff, beta, gamma);
	cout << dec << "#Lz0z1=" << Lz0z1.size() << endl;
	cout << dec << "#Lz1z2=" << Lz1z2.size() << endl;
	cout << dec << "#Lz2z3=" << Lz2z3.size() << endl;
	cout << dec << "#Lz3z4=" << Lz3z4.size() << endl;


	//2nd level
	vector<StateAndKnown> Lz0z1z2 = merge2List(Lz0z1, Lz1z2);
	cout << dec << "#Lz0z1z2=" << Lz0z1z2.size() << endl;
	for (int i = 0; i < Lz0z1z2.size(); ++i) {
		A5_1_S100 check(Lz0z1z2[i].state);
		for (int r = 0; r < totalStep; ++r) {
			check.doOneStep();
		}
		if ((check.getPrefix() & 0x7) != (prefix & 0x7))
			cout << "Prefix=" << hex << check.getPrefix() << endl;
	}

	Lz0z1.clear();
	vector<StateAndKnown> Lz1z2z3 = merge2List(Lz1z2, Lz2z3);
	cout << dec << "#Lz1z2z3=" << Lz1z2z3.size() << endl;
	Lz1z2.clear();
	vector<StateAndKnown> Lz2z3z4 = merge2List(Lz2z3, Lz3z4);
	cout << dec << "#Lz2z3z4=" << Lz2z3z4.size() << endl;
	Lz2z3.clear();
	Lz3z4.clear();

	//3rd level
	vector<StateAndKnown> Lz0z1z2z3 = merge2List(Lz0z1z2, Lz1z2z3);
	cout << dec << "#Lz0z1z2z3=" << Lz0z1z2z3.size() << endl;
	Lz0z1z2.clear();
	vector<StateAndKnown> Lz1z2z3z4 = merge2List(Lz1z2z3, Lz2z3z4);
	cout << dec << "#Lz1z2z3z4=" << Lz1z2z3z4.size() << endl;
	Lz1z2z3.clear();
	Lz2z3z4.clear();

	//4th level
	vector<StateAndKnown> Lz0z1z2z3z4 = merge2List(Lz0z1z2z3, Lz1z2z3z4);
	cout << dec << "#Lz0z1z2z3z4=" << Lz0z1z2z3z4.size() << endl;
	Lz0z1z2z3.clear();
	Lz1z2z3z4.clear();


	for (int i = 0; i < Lz0z1z2z3z4.size(); ++i) {
		A5_1_S100 check(Lz0z1z2z3z4[i].state);
		for (int stp = 0; stp < totalStep; ++stp)
			check.doOneStep();
		if (check.getPrefix() != prefix)
			cout << "Prefix=" << hex << check.getPrefix() << endl;
	}
	bool success = false;
	for (int i = 0; i < Lz0z1z2z3z4.size(); ++i) {
		if ((initState & Lz0z1z2z3z4[i].known) == (Lz0z1z2z3z4[i].state & Lz0z1z2z3z4[i].known)) {
			success = true;
			set<int> knownBits = Lz0z1z2z3z4[i].getKnownBits();
			cout << "Successfully recovered the internal state bits at positions: ";
			for (set<int>::iterator ite = knownBits.begin(); ite != knownBits.end(); ++ite) {
				cout << dec << *ite << ",";
			}
			cout << dec << endl;
			cout << "That's " << knownBits.size() << " in total!\n";
		}
	}
	if (!success)
		cout << "Haven't recovered the internal state!\n";
	return 0;
}
#endif

#if TEST
int main() {
	srand(time(NULL));
	int targetSteps = 23;
	u64 initState = rand_64();
	A5_1_S100 runner(initState);
	vector<u64> trackZ;
	for (int r = 0; r < targetSteps; ++r) {
		cout << "R1[8]:" << runner.getRiBit(1, 8) << endl;
		cout << "R2[10]:" << runner.getRiBit(2, 10) << endl;
		cout << "R3[10]:" << runner.getRiBit(3, 10) << endl;
		runner.doOneStep();
		cout << "Last Move Mask=" << runner.getLastMoveMask() << endl;
		cout << "z" << r << "=" << runner.getCurrentZ() << endl;
		trackZ.push_back(runner.getCurrentZ());
	}
	u64 prefix = runner.getPrefix();
	for (int i = 0; i < trackZ.size(); ++i) {
		cout << (trackZ[i] ^ bit64(prefix, i)) << ",";
	}
	cout << endl;

	A5_1_S100 eqCheckRunner(initState);
	InternalStateEquations stateEq = InternalStateEquations();


	for (int r = 0; r < targetSteps; ++r) {
		eqCheckRunner.doOneStep();
		stateEq.doOneStep(eqCheckRunner.getOneStepMoveMask(r));
		u64 currentWholeState = eqCheckRunner.getWholeState();
		for (int bitNo = 0; bitNo < 64; ++bitNo) {
			if (sum64(stateEq.currentState[bitNo] & initState) != bit64(currentWholeState, bitNo)) {
				cout << "Fail step " << r << " at bitNo " << bitNo << endl;
			}
		}
		if (sum64(stateEq.getOutputEquation() & initState) != eqCheckRunner.getCurrentZ()) {
			cout << "Fail step " << r << " at output\n";
		}
	}

	int currentHaveDoneStep = 0;
	PracticalAttack attack = PracticalAttack();
	A5_1_S100 orderCheckRunnter(initState);
	do {
		cout << "Step " << currentHaveDoneStep << endl;
		orderCheckRunnter.doOneStep();
		//Currect guess
		//attack.doOneMove(orderCheckRunnter.getLastMoveMask(), orderCheckRunnter.getCurrentZ());
		//Incorrect guess
		u64 wrongMove;
		do {
			wrongMove = rand_64() & 0x3;
		} while (wrongMove == orderCheckRunnter.getLastMoveMask());
		attack.doOneMove(wrongMove, orderCheckRunnter.getCurrentZ());
		++currentHaveDoneStep;
		bool passCurrent = attack.isFeasible();
		cout << "MatOrder: " << attack.matOrder << endl;
		if (!passCurrent) {
			cout << "Current step infeasible!\n";
			cout << "Have passed " << currentHaveDoneStep << " steps in total!\n";
			break;
		}
	} while (attack.matOrder != 64 && currentHaveDoneStep < 32);
	cout << currentHaveDoneStep << " steps in total!\n";
	
	return 0;
}
#endif