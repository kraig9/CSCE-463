#pragma once
#include "common.h"
#define INITIAL_BUF_SIZE 8000
#define THRESHOLD 8000

class Socket {
	SOCKET sock;
	char *buf;
	int allocatedSize;
	int curPos;
	
public:
	Socket();

	bool Read();
};