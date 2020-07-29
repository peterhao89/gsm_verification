#pragma once
#include"CommonUtils.hpp"
#include <NTL/ZZ_pX.h>
#include<NTL/mat_ZZ_p.h>
#include<NTL/mat_poly_ZZ_p.h>
#include<NTL/ZZ_pXFactoring.h>
#include<NTL/GF2XFactoring.h>
#include<NTL/GF2EXFactoring.h>
#include<NTL/mat_GF2.h>
#include<vector>
#include<set>
#include<algorithm>
#include <immintrin.h>
#include<fstream>
#include<omp.h>
NTL_CLIENT

#define A5_1_STATE_SIZE 64


class InternalStateEquations {
public:
	int haveDoneSteps;
	vector<u64> currentState;
	InternalStateEquations() {
		haveDoneSteps = 0;
		for (int i = 0; i < 64; ++i) {
			u64 one = 1;
			one <<= i;
			currentState.push_back(one);
		}
	}

	void restart() {
		haveDoneSteps = 0;
		for (int i = 0; i < 64; ++i) {
			u64 one = 1;
			one <<= i;
			currentState.push_back(one);
		}
	}

	u64 getOutputEquation() {
		return (currentState[18]^currentState[40]^currentState[63]);
	}

	u64 getRiClockBitEquation(int i) {
		switch (i)
		{
		case 1:
			return currentState[8];
			break;
		case 2:
			return currentState[19 + 10];
			break;
		case 3:
			return currentState[41 + 10];
			break;
		default:
			return 0;
		}

	}

	void doOneStep(u64 oneStepMask) {
		switch (oneStepMask)
		{
		case 0:
			updateR1();
			updateR2();
			updateR3();
			haveDoneSteps++;
			break;
		case 1:
			updateR2();
			updateR3();
			haveDoneSteps++;
			break;
		case 2:
			updateR1();
			updateR3();
			haveDoneSteps++;
			break;
		case 3:
			updateR1();
			updateR2();
			haveDoneSteps++;
			break;
		default:
			cerr << "InternalStateEquations oneStepMask has illegal value " << oneStepMask << "!\n";
			break;
		}
	}


	void updateR1() {
		u64 tmp = currentState[18] ^ currentState[17] ^ currentState[16] ^ currentState[13];
		for (int i = 0; i < 18; ++i) {
			currentState[18 - i] = currentState[17 - i];
		}
		currentState[0] = tmp;
	}

	//19-40 #22
	void updateR2() {
		u64 tmp = currentState[40] ^ currentState[39];
		for (int i = 0; i < 21; ++i) {
			currentState[40 - i] = currentState[39 - i];
		}
		currentState[19] = tmp;
	}

	//41-63 #23
	void updateR3() {
		u64 tmp = currentState[63] ^ currentState[62] ^ currentState[61]^currentState[48];
		for (int i = 0; i < 22; ++i) {
			currentState[63 - i] = currentState[62 - i];
		}
		currentState[41] = tmp;
	}


};



class PracticalAttack {
public:
	PracticalAttack(long eqCap = 96) {
		eqNumber = 0;
		eqMat.SetDims(eqCap, A5_1_STATE_SIZE + 1);
		eqMatExtend.SetDims(eqCap, A5_1_STATE_SIZE + 1);
		stateTrack = InternalStateEquations();
		matOrder = 0;
		eqCapacity = eqCap;
		
	}
	mat_GF2 eqMat, eqMatExtend;
	InternalStateEquations stateTrack;
	int eqNumber;
	long eqCapacity;
	long matOrder;


	u64 getSolution() {
		mat_GF2 A;
		A.SetDims(A5_1_STATE_SIZE, A5_1_STATE_SIZE);
		vec_GF2 b;
		b.SetLength(A5_1_STATE_SIZE);
		for (int i = 0; i < A5_1_STATE_SIZE; ++i) {
			for (int j = 0; j < A5_1_STATE_SIZE; ++j) {
				A[i][j] = eqMatExtend[i][j];
			}
			b[i] = eqMatExtend[i][A5_1_STATE_SIZE];
		}
		vec_GF2 x;
		GF2 d;
		solve(d, A, x, b);
		u64 recoveredState = 0;
		for (int i = 0; i < A5_1_STATE_SIZE; ++i)
			setBitVal(recoveredState, i, x[i]==1?1:0);
		return recoveredState;
	}

	bool isFeasible() {
		long orderMat = gauss(eqMat);
		long orderMatExt = gauss(eqMatExtend);
		if (orderMat == orderMatExt) {
			matOrder = orderMat;
			return true;
		}
		else return false;
	}

	void constructEquations(u64 moves, u64 prefix, int moveNumber) {
		eqNumber = 0;
		eqMat.SetDims(eqCapacity, A5_1_STATE_SIZE + 1);
		eqMatExtend.SetDims(eqCapacity, A5_1_STATE_SIZE + 1);
		stateTrack.restart();
		matOrder = 0;
		for (int i = 0; i < moveNumber; ++i) {
			doOneMove((moves >> (2 * i)) & 0x3, bit64(prefix, i));
		}
	}

	//lhEq: the Matrix A in Ax=b
	// rhs: 0 eq; 1 ueq
	void setOneEquation(u64 lhEq, u64 rhs) {
		for (int bitNo = 0; bitNo < A5_1_STATE_SIZE; ++bitNo) {
			if (bit64(lhEq, bitNo) == 1) {
				eqMat[eqNumber][bitNo] = 1;
				eqMatExtend[eqNumber][bitNo] = 1;
			}
		}
		if (rhs)eqMatExtend[eqNumber][A5_1_STATE_SIZE] = 1;
		++eqNumber;
	}

	void doOneMove(u64 oneMoveMask, u64 prefixBit) {
		vector<u64> clkBitAnfs = {
			stateTrack.getRiClockBitEquation(1),
			stateTrack.getRiClockBitEquation(2),
			stateTrack.getRiClockBitEquation(3) 
		};
		switch (oneMoveMask)
		{
		case 0:
			setOneEquation(clkBitAnfs[0] ^ clkBitAnfs[1], 0);
			setOneEquation(clkBitAnfs[0] ^ clkBitAnfs[2], 0);
			break;
		case 1:
			setOneEquation(clkBitAnfs[0] ^ clkBitAnfs[1], 1);
			setOneEquation(clkBitAnfs[0] ^ clkBitAnfs[2], 1);
			break;
		case 2:
			setOneEquation(clkBitAnfs[0] ^ clkBitAnfs[1], 1);
			setOneEquation(clkBitAnfs[0] ^ clkBitAnfs[2], 0);
			break;
		case 3:
			setOneEquation(clkBitAnfs[0] ^ clkBitAnfs[1], 0);
			setOneEquation(clkBitAnfs[0] ^ clkBitAnfs[2], 1);
			break;
		}
		stateTrack.doOneStep(oneMoveMask);
		u64 outputEq = stateTrack.getOutputEquation();
		setOneEquation(outputEq, prefixBit);
	}





};

enum NodeStatus {
	SUCCESS, 
	BREED,
	GUESS, 
	BACKTRACK,
	NEWBORN
};

class GuessNode {
public:
	GuessNode(GuessNode* pNode=NULL) {
		parent = pNode;
		if (parent != NULL) {
			parent->child = this;
			haveDoneSteps = parent->haveDoneSteps;
			haveDoneMoveMask = parent->haveDoneMoveMask;
			haveDonePrefix = parent->haveDonePrefix;
		}
		child = NULL;
		currentGuess = 0;
		
		haveDoneMoveMask = 0;
		haveDonePrefix = 0;
		eqMatOrder = 0;
		stateTrack = InternalStateEquations();
		currentStatus = NEWBORN;
		solveA51();
	}
	GuessNode* parent;
	GuessNode* child;
	u64 currentGuess;
	int haveDoneSteps;
	long eqMatOrder;
	u64 haveDoneMoveMask;
	u64 haveDonePrefix;
	InternalStateEquations stateTrack;
	NodeStatus currentStatus;

	void solveA51() {
		
	}

};
