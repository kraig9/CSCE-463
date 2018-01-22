#pragma once
#define INITIAL_BUF_SIZE 8000
#define THRESHOLD 8000

#include "common.h"

class Socket {
	SOCKET sock;
	char *buf;
	int allocatedSize;
	int curPos;

public:
	Socket();

	char* getBuf();
	int getSize();
	int getCurPos();
	bool Read();
};