#include<string>
#include<omp.h>
#include<cstring>
#define TEST 0
#define MERGE 0
#define GEN_GUESS_TABLE 0
#define PRACTICAL_ATTACK 1

#if GEN_GUESS_TABLE
#include"A5_1.hpp"
#elif PRACTICAL_ATTACK
#include"PracticalAttack.hpp"
#endif


#if PRACTICAL_ATTACK
int main() {

	long dimension = 10;
	vec_GF2 x,b;
	GF2 d;
	b.SetLength(dimension);
	mat_GF2 B;
	B.SetDims(dimension+3, dimension + 1);
	for (int i = 0; i < dimension; ++i) {
		B[i+1][i] = 1;
	}
	B[1][dimension] = 1;
	cout << B << endl;

	long orderB = gauss(B);
	cout << orderB << endl;
	cout << B << endl;


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
