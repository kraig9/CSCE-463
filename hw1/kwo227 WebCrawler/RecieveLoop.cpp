//Kraig Orcutt, CSCE 463-500 Spring 2018

#include "common.h"
#include "RecieveLoop.h"
#include "Socket.h"

bool Socket::Read() {
	//https://msdn.microsoft.com/en-us/library/ms740141%28VS.85%29.aspx?f=255&MSPPError=-2147217396
	//used MSDN for reference about the select function and related file descriptors etc.
	//Select parameters
	int nfds=1;
	fd_set readfds;
	readfds.fd_array[0]=sockt;
	readfds.fd_count=1;
	timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	//ofstream outBuf;

	//This code was copied from the homework paper, page 4
	while (true) {
		int ret = select(0, &readfds, 0, 0, &timeout);
		if (ret > 0) {
			int bytes = recv(sockt, buf + curPos, allocatedSize - curPos, 0);
			if (bytes == SOCKET_ERROR) {
				throw(10);
			}
			if (bytes == 0) {
				buf[curPos + 1] = '\0';
				return true;
			}
			curPos += bytes;
			if (allocatedSize - curPos < THRESHOLD) {
				char* tempBuf = new char[2 * allocatedSize];
				//resize buffer with realloc or HeapReAlloc, or memcpy into bigger array
				memcpy(tempBuf, buf, allocatedSize);
				char* old_buf = buf;
				buf = tempBuf;
				allocatedSize = allocatedSize * 2;
				delete old_buf;
			}
		}
		else if (ret == 0) {
			//report timeout
			printf("failed with %u on recv\n", WSAGetLastError());
			throw(10);
		}
		else {
			printf((char*)WSAGetLastError());
			break;
		}
		/*outBuf.open("kwo227RecievedHTML.txt", ofstream::out);
		outBuf << buf;
		outBuf.close();*/
	}

	return false;
}