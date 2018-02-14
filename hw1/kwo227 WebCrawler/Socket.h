//Kraig Orcutt, CSCE 463-500 Spring 2018

#pragma once
#define INITIAL_BUF_SIZE 4000
#define THRESHOLD 2000

#include "common.h"
#include <winsock2.h>
#include "URLparser.h"
#include "uniqueness.h"
#include "multithread.h"

struct Socket {
	SOCKET sockt;
	char* buf = new char[INITIAL_BUF_SIZE];
	int allocatedSize = INITIAL_BUF_SIZE;
	int curPos = 0;

	//constructors
	Socket() {

	}
	Socket(char* buffer, int allSize, int currPos) {
		memset(buf, 0, INITIAL_BUF_SIZE);
		strcpy_s(buf, allSize, buffer);
		allocatedSize = allSize;
		curPos = currPos;
	}
	//member functions
	bool Read(bool robot);
};

void winsock_test(parsed parsedURL);
void winsock_test2(parsed parsedURL, threadParams &inpParam);