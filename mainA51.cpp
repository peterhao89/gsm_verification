#include"A5_1.hpp"
#include<string>
#include<omp.h>
#include<cstring>

#define TEST 0
#define MERGE (1-TEST)

#if TEST

int main() {
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
	//omp_set_num_threads(6);	
#pragma omp parallel for 
	for (int prefix = 0; prefix < (1 << prefixLength); prefix++) {
		u64 pattern = prefix;
		set<u64> Lz0_z4;
		vector<u64> patternBits;
		for (int i = 0; i < prefixLength; ++i) {
			patternBits.push_back(bit(pattern, i));
		}
		A5_1_S100 runner(0);
		u64 iniState = 0;
		bool satisfyPattern = true;
		for (count = 0; count < total; ++count) {
			iniState = 0;
			satisfyPattern = true;
			for (int loop = 0; loop < involvedBits.size(); ++loop) {
				setBitVal(iniState, involvedBits[loop], bit(count, loop));
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
