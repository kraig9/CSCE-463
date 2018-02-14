#pragma once
#include "common.h"
#include "URLparser.h"
#include "uniqueness.h"

struct threadParams {
	int Q = 0, E = 0, H = 0, D = 0,
		I = 0, R = 0, C = 0,
		L = 0, activeThreads = 0,
		time = 0;
	Uniqueness* uniquePoint;
	queue<parsed>* queuePoint;
	CRITICAL_SECTION* lpCriticalSection;
	CRITICAL_SECTION* statsCriticalSection;
public:
	threadParams(CRITICAL_SECTION lpCriticalSection, CRITICAL_SECTION statsCriticalSection) {
		threadParams::lpCriticalSection = &lpCriticalSection;
		threadParams::statsCriticalSection = &statsCriticalSection;
	}
	void activeThreadsDec();
	void activeThreadsInc();
	void incQ();
	void decQ();
	void incE();
	void decE();
	void incH();
	void decH();
	void incD();
	void decD();
	void incI();
	void decI();
	void incR();
	void decR();
	void incC();
	void decC();
	void incL();
	void decL();

	//ostream& operator<<(ostream os)
	//{
	//	os << "[ " << time << "]  " << activeThreads << " Q " << Q << " E "
	//		<< E << " H\t" << H << " D\t" << D << " I\t"
	//		<< I << " R\t" << R << " C\t" << C << " L\t"
	//		<< L << endl;
	//	return os;
	//}
};




DWORD WINAPI threadFunc(LPVOID lpParams);