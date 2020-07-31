#include<string>
#include<omp.h>
#include<cstring>
#define TEST 0
#define MERGE 0
#define GEN_GUESS_TABLE 0
#define PRACTICAL_ATTACK 0
#define DDT_GENERATE 1

#if GEN_GUESS_TABLE
#include"GuessDetermineCP.hpp"
#elif PRACTICAL_ATTACK 
#include"A51Impl.hpp"
#include"PracticalAttack.hpp"
#endif

#if DDT_GENERATE
#include"Merge.hpp"




int main(){

	u64 a = 0;
	flipBitVal(a, 0);
	cout << a << endl;
	int totalSteps =1;
	u64 iterTime = 1;
	u64 diff = 0x3;
	u64 z0 = rand_64() & 0x3;
	while (1) {
		vector<u64> vec = getLwithAlg3(z0, iterTime, diff);
		u64 initState = vec[0];
		A5_1_S100 check(initState);
		for (int step = 0; step < totalSteps; ++step) {
			check.doOneStep();
			if (check.getCurrentZ() != bit64(z0, step)) {
				cout << step << ":" << "Fail!\n";
			}
		}
	}


	/*
	while (1) {
		u64 initState = rand_64() & maskZ0Z1;
		vector<int> hiBits = { 18,40,63 };
		A5_1_S100 runner(initState);
		set<int> haveUsedBit;
		for (int step = 0; step < totalSteps; ++step) {
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
		A5_1_S100 check(initState);
		for (int step = 0; step < totalSteps; ++step) {
			check.doOneStep();
			if (check.getCurrentZ() != bit64(z0, step)) {
				cout <<step<<":"<< "Fail!\n";
			}
		}
	}
	
	*/







	return 0;
}

#endif





#if PRACTICAL_ATTACK

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
	srand(time(NULL));
	long testTime = 1;
	testTime <<= 30;

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

#if ROUNDFILTER_RANDOM_WRONG_GUESSES
	string header = "Rd;#pss;#matOrder;#test";
	ofstream file1("RoundFilterEval.txt");
	file1 << header << endl;
	for (int round = 14; round < 30; ++round) {
		FilterStrengthAtRoundReport mm = getFilterStrengthAtRound(1, round, testTime);
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

int main(int argc, char const* argv[]) {
	u64 count = 0;
	int prefixLength = 2;
	int threadNumber = 1;
	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-pl")) prefixLength = atoi(argv[i + 1]);
		if (!strcmp(argv[i], "-t")) threadNumber = atoi(argv[i + 1]);
	}
	cout << prefixLength << endl;
	cout << threadNumber << endl;
	vector<int> relatedBitsOf1Output = { 18, 19 + 21, 41 + 22, 17,19 + 20, 41 + 21, 8, 19 + 10, 41 + 10 };
	set<int> relatedBitsOf5Outputs;
	for (int offset = 0; offset < prefixLength; ++offset) {
		for (int i = 0; i < relatedBitsOf1Output.size(); ++i) {
			relatedBitsOf5Outputs.insert(relatedBitsOf1Output[i]-offset);
		}
	}
	vector<int> involvedBits;
	for (set<int>::iterator ite = relatedBitsOf5Outputs.begin(); ite != relatedBitsOf5Outputs.end(); ++ite) {
		involvedBits.push_back(*ite);
	}
	int involvedBitNumber = involvedBits.size();
	u64 total = 1;
	total <<= involvedBitNumber;
	omp_set_num_threads(6);	
#pragma omp parallel for 
	for (int prefix = 0; prefix < (1 << prefixLength); prefix++) {
		u64 pattern = prefix;
		set<u64> Lz0_z4;
		vector<u64> patternBits;
		for (int i = 0; i < prefixLength; ++i) {
			patternBits.push_back(bit64(pattern, i));
		}
		A5_1_S100 runner(0);
		u64 iniState = 0;
		bool satisfyPattern = true;
		for (count = 0; count < total; ++count) {
			iniState = 0;
			satisfyPattern = true;
			for (int loop = 0; loop < involvedBits.size(); ++loop) {
				setBitVal(iniState, involvedBits[loop], bit64(count, loop));
			}
			runner.update(iniState);
			for (int i = 0; i < prefixLength; ++i) {
				if (runner.output() != patternBits[i]) {
					satisfyPattern = false;
					break;
				}
			}
			if (satisfyPattern) {
				Lz0_z4.insert(iniState);
			}
		}
		ofstream file1;
		file1.open("L"+to_string(prefixLength)+"prefix"+ to_string(prefix)+".txt");
		file1 << hex << "prefix=" << pattern << endl;
		file1 << dec << "The merged list L has #L=" << Lz0_z4.size() << endl;
		file1.close();
		cout << hex << "prefix=" << pattern << endl;
		cout << dec << "The merged list L has #L=" << Lz0_z4.size() << endl;
	}
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