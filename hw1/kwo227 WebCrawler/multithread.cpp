#include "multithread.h"
#include "Socket.h"

DWORD WINAPI threadFunc(LPVOID lpParams)
{
	threadParams* input = (threadParams*)lpParams;
	EnterCriticalSection(input->lpCriticalSection);
	while (input->queuePoint->size() > 0) {
		try {
			input->queuePoint->front();
			parsed parsedURL = input->queuePoint->front();
			input->queuePoint->pop();
			input->decQ();
			input->incE();
			LeaveCriticalSection(input->lpCriticalSection);
			winsock_test2(parsedURL, *input);
		}
		catch (int f) {
			EnterCriticalSection(input->lpCriticalSection);
			continue;
			//if (f == 1);
			//else if (f == 2);
			//else if (f == 3);
			//else if (f == 4);
			//else if (f == 5);
			////9 is for getting a status code other than 2xx
			//else if (f == 9);
			//else if (f == 10);
			//else if (f == 11);
			//else if (f == 12);
			////this is for the case that the robots header isn't status code 4xx
			//else if (f == 13);
		}
		EnterCriticalSection(input->lpCriticalSection);
	}
	input->activeThreadsDec();
	LeaveCriticalSection(input->lpCriticalSection);
	return 0;
}
//thread safe decrementers
void threadParams::activeThreadsDec()
{
	EnterCriticalSection(statsCriticalSection);
	activeThreads--;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decQ()
{
	EnterCriticalSection(statsCriticalSection);
	Q--;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decE()
{
	EnterCriticalSection(statsCriticalSection);
	E--;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decH()
{
	EnterCriticalSection(statsCriticalSection);
	H--;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decD()
{
	EnterCriticalSection(statsCriticalSection);
	D--;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decI()
{
	EnterCriticalSection(statsCriticalSection);
	I--;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decR()
{
	EnterCriticalSection(statsCriticalSection);
	R--;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decC()
{
	EnterCriticalSection(statsCriticalSection);
	C--;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decL()
{
	EnterCriticalSection(statsCriticalSection);
	L--;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incNumTAMU()
{
	EnterCriticalSection(statsCriticalSection);
	numTAMU++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decpp(float amount)
{
	EnterCriticalSection(statsCriticalSection);
	pp = pp - amount;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::decMB(float amount)
{
	EnterCriticalSection(statsCriticalSection);
	MB = MB - amount;
	LeaveCriticalSection(statsCriticalSection);
}

//thread safe incrementers
void threadParams::incMB(float amount) {
	EnterCriticalSection(statsCriticalSection);
	MB = MB + amount;
	totalMB = totalMB + amount;
	LeaveCriticalSection(statsCriticalSection);
}

void threadParams::incpp(float amount)
{
	EnterCriticalSection(statsCriticalSection);
	pp = pp + amount;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::activeThreadsInc() {
	EnterCriticalSection(statsCriticalSection);
	activeThreads++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incQ()
{
	EnterCriticalSection(statsCriticalSection);
	Q++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incE()
{
	EnterCriticalSection(statsCriticalSection);
	E++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incH()
{
	EnterCriticalSection(statsCriticalSection);
	H++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incD()
{
	EnterCriticalSection(statsCriticalSection);
	D++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incI()
{
	EnterCriticalSection(statsCriticalSection);
	I++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incR()
{
	EnterCriticalSection(statsCriticalSection);
	R++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incC()
{
	EnterCriticalSection(statsCriticalSection);
	C++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incL(int amount)
{
	EnterCriticalSection(statsCriticalSection);
	L=L+amount;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::inc2xx()
{
	EnterCriticalSection(statsCriticalSection);
	twoxx++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::inc3xx()
{
	EnterCriticalSection(statsCriticalSection);
	threexx++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::inc4xx()
{
	EnterCriticalSection(statsCriticalSection);
	fourxx++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::inc5xx()
{
	EnterCriticalSection(statsCriticalSection);
	fivexx++;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::incother()
{
	EnterCriticalSection(statsCriticalSection);
	other++;
	LeaveCriticalSection(statsCriticalSection);
}

//setters
void threadParams::setpp(float amount) {
	EnterCriticalSection(statsCriticalSection);
	pp = amount;
	LeaveCriticalSection(statsCriticalSection);
}
void threadParams::setMB(float amount) {
	EnterCriticalSection(statsCriticalSection);
	MB = amount;
	LeaveCriticalSection(statsCriticalSection);
}