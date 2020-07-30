#include<string>
#include<omp.h>
#include<cstring>
#define TEST 0
#define MERGE 0
#define GEN_GUESS_TABLE 0
#define PRACTICAL_ATTACK 1

#if GEN_GUESS_TABLE
#include"GuessDetermineCP.hpp"
#elif PRACTICAL_ATTACK
#include"A51Impl.hpp"
#include"PracticalAttack.hpp"
#endif


#if PRACTICAL_ATTACK

u64 sum64(u64 vec) {
	u64 summation = 0;
	for (int i = 0; i < 64; ++i)
		summation ^= bit64(vec, i);
	return summation;
}

//guess: 0 for correct guess; otherwise wrong guess
int testOnce(long guess) {
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



RR getAvgSteps(long guess, long testTime=(1<<20)) {
	RR counter = to_RR(1);
	RR collector = to_RR(0);
	RR totalTestTime = to_RR(testTime);
	for (; counter < totalTestTime; ++counter) {
		collector += to_RR(testOnce(guess));
	}
	return collector / totalTestTime;
}

int main() {
	srand(time(NULL));
	long testTime = 1;
	testTime <<= 20;
	clock_t end;

	clock_t start = clock();
	RR avgCorrect = getAvgSteps(0, testTime);
	RR avgWrong = getAvgSteps(1, testTime);
	end = clock();

	ofstream file1("WrongCorrectStepDiff.txt");
	cout << "Total:" << testTime << endl;
	cout << "Correct:" << avgCorrect << endl;
	cout << "Wrong:" << avgWrong << endl;
	cout << "Time:" << (end - start) << "ms" << endl;
	file1 << "Total:" << testTime << endl;
	file1 << "Correct:" << avgCorrect << endl;
	file1 << "Wrong:" << avgWrong << endl;
	file1 << "Time:" << (end - start) << "ms" << endl;
	file1.close();
	

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