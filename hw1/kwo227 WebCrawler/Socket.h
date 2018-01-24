#pragma once
#define INITIAL_BUF_SIZE 8000
#define THRESHOLD 2000

#include "common.h"
#include <winsock2.h>
#include "URLparser.h"

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
	bool Read();
};

void winsock_test(parsed parsedURL);