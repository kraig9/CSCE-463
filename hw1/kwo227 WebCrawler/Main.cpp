//Kraig Orcutt, CSCE 463-500 Spring 2018

#include "common.h"
#include "URLparser.h"
#include <process.h>
#include "Socket.h"
#include "uniqueness.h"
#include "multithread.h"
#include "HTMLParserBase.h"
//#include "vld.h"

DWORD statThreadFunc(LPVOID input) {
	threadParams* params = (threadParams*)input;
	//EnterCriticalSection(params->lpCriticalSection);
	while (params->Q > 0) {
		//LeaveCriticalSection(params->lpCriticalSection);
		Sleep(2000);
		//EnterCriticalSection(params->statsCriticalSection);
		//EnterCriticalSection(params->lpCriticalSection);
		params->time = params->time + 2;
		//cout << "[ " << time << "]  " << params->activeThreads<< " Q " << params->Q << " E "
		//    << params->E << " H\t" << params->H << " D\t" << params->D << " I\t"
		//	<< params->I << " R\t" << params->R << " C\t" << params->C << " L\t"
		//	<< params->L << endl;
		printf("[ %3d] %4d Q %6d E %7d H %7d D %7d I %5d R %5d C %5d L %4d\n",
			params->time, params->activeThreads, params->Q, params->E, params->H, params->D,
			params->I, params->R, params->C, params->L);
		printf("      *** crawling %.0f pps @ %.0f Mbps\n", params->pp / 2, params->MB / 2);
		params->setpp(0);
		params->setMB(0);
		//LeaveCriticalSection(params->lpCriticalSection);
		//LeaveCriticalSection(params->statsCriticalSection);
	}
	EnterCriticalSection(params->statsCriticalSection);
	EnterCriticalSection(params->lpCriticalSection);
	if (params->Q == 0) {
		printf("Extracted %d URLs @ %d/s\n", params->E, params->E / params->time);
		printf("Looked up %d DNS names @ %d/s\n", params->D, params->D / params->time);
		printf("Downloaded %d robots @ %d/s\n", params->I, params->I / params->time);
		printf("Crawled %d pages @ %d/s (%.0f MB)\n", params->C, params->C / params->time, params->totalMB);
		printf("Parsed %d links @ %d/s\n", params->L, params->L / params->time);
		printf("HTTP codes: 2xx = %d, 3xx = %d, 4xx = %d, 5xx = %d, other = %d",
			params->twoxx, params->threexx, params->fourxx, params->fivexx, params->other);
	}
	LeaveCriticalSection(params->statsCriticalSection);
	LeaveCriticalSection(params->lpCriticalSection);
	return 0;
}
//part 2 accept 2 args as well
//Opened URL-input.txt with size [SIZE]
int main(int argc, char** argv) {
	try {
		if (argc < 2) {
			cout << "Error: not enough arguments provided." << endl;
			return 1;
		}
		else if (argc > 3) {
			cout << "Error: too many arguments provided." << endl;
			return 1;
		}
		if (argc == 2) {
			printf("URL: %s \n", argv[1]);
			parsed parsedURL = URLparse(argv[1]);
			winsock_test(parsedURL);
		}
		else if (argc == 3) {
			//parse command line args
			if (atoi(argv[1]) < 1) throw(11);
			int numThreads = atoi(argv[1]);

			//initialize shared data structures & parameters sent to threads
			CRITICAL_SECTION lpCriticalSection;
			CRITICAL_SECTION hostUniqueSection;
			CRITICAL_SECTION IPUniqueSection;
			CRITICAL_SECTION statsCriticalSection;
			CRITICAL_SECTION parserCriticalSection;
			InitializeCriticalSection(&lpCriticalSection);
			InitializeCriticalSection(&hostUniqueSection);
			InitializeCriticalSection(&IPUniqueSection);
			InitializeCriticalSection(&statsCriticalSection);
			InitializeCriticalSection(&parserCriticalSection);

			threadParams params(lpCriticalSection, statsCriticalSection);
			Uniqueness uniqueStruct(hostUniqueSection, IPUniqueSection);
			queue<parsed> queueStruct;
			params.queuePoint = &queueStruct;
			params.uniquePoint = &uniqueStruct;
			char URLbuf[MAX_URL_LEN];
			DWORD*   dwThreadIdArray = new DWORD[numThreads];
			HTMLParserBase* parser = new HTMLParserBase;
			params.parser = parser;
			params.parserCriticalSection = &parserCriticalSection;

			//read file and populate shared queue
			//http://www.cplusplus.com/doc/tutorial/files/
			ifstream inputFile(argv[2],ios::ate);
			if (inputFile.good() != 1) throw(12);
			cout << "Opened " + string(argv[2]) + " with size " + to_string((inputFile.tellg())) << endl;
			//https://stackoverflow.com/questions/5343173/returning-to-beginning-of-file-after-getline
			inputFile.clear();
			inputFile.seekg(0, ios::beg);
			while (inputFile.getline(URLbuf, MAX_URL_LEN)) {
				parsed parsedURL = URLparse(URLbuf);
				queueStruct.push(parsedURL);
				params.incQ();
			}
			inputFile.close();

			//start stats thread
			//https://msdn.microsoft.com/en-us/library/windows/desktop/ms682516(v=vs.85).aspx
			HANDLE statHandle = CreateThread(NULL, 0, statThreadFunc, (LPVOID*)&params, 0, dwThreadIdArray);
			


			//starts N crawling thread
			//https://msdn.microsoft.com/en-us/library/windows/desktop/ms682516(v=vs.85).aspx
			try {
				int threadCount = 0;
				HANDLE* pDataArray = new HANDLE[numThreads];
				for (int i = 0; i < numThreads; i++) {
					params.activeThreadsInc();
					pDataArray[i]=CreateThread(NULL, 0, threadFunc, (LPVOID*)&params, 0, dwThreadIdArray);
					threadCount++;
				}
				WaitForMultipleObjects(numThreads, pDataArray, true, INFINITE);
				WaitForSingleObject(statHandle, INFINITE);
				
				delete[] pDataArray;
				delete[] dwThreadIdArray;
				delete parser;
				WSACleanup();
				return 0;
			}
			//https://msdn.microsoft.com/en-us/library/windows/desktop/ms687025(v=vs.85).aspx
			catch (int f) {
				if (f == 1) printf("failed with invalid scheme");
				else if (f == 2) printf("failed with invalid port");
				else if (f == 3) printf("failed with %d", WSAGetLastError());
				else if (f == 4) printf("failed with non-HTTP header");
				else if (f == 5) printf("failed with exceeding max");
				//9 is for getting a status code other than 2xx
				else if (f == 9) return 0;
				else if (f == 10) printf("a timeout occurred");
				else if (f == 11) printf("Can't have less than 1 thread. Input args are [threads][file to input]");
				else if (f == 12) printf("failed");
				//this is for the case that the robots header isn't status code 4xx
				else if (f == 13);
			}
		}
	}
	catch (int e) {
		if (e == 1) printf("failed with invalid scheme");
		else if (e == 2) printf("failed with invalid port");
		else if (e == 3) printf("failed with %d", WSAGetLastError());
		else if (e == 4) printf("failed with non-HTTP header");
		else if (e == 5) printf("failed with exceeding max");
		//9 is for getting a status code other than 2xx
		else if (e == 9) return 0;
		else if (e == 10) printf(" a timeout occurred");
		else if (e == 11) printf("Program only supports 1 thread. Input args are [threads][file to input]");
		else if (e == 12) printf("The input file couldn't be opened. Check the filename passed in.");
		return 1;
	}
	return 0;
}