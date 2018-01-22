#include "RecieveLoop.h"

Socket::Socket(void) {
	buf = new char[INITIAL_BUF_SIZE];
}

bool Socket::Read() {
	//https://msdn.microsoft.com/en-us/library/ms740141%28VS.85%29.aspx?f=255&MSPPError=-2147217396
	//used MSDN for reference about the select function and related file descriptors etc.
	//Select parameters
	_In_ int nfds;
	_Inout_ fd_set *readfds;
	_Inout_ fd_set *writefds;
	_Inout_ fd_set *exceptfds;
	_In_ const struct timeval *timeout;

	//recv parameters
	_In_  SOCKET s;
	_Out_ char   *buf;
	_In_  int    len;
	_In_  int    flags;

	//This code was copied from the homework paper, page 4
	while (true) {
		int ret = select(nfds, readfds, writefds, exceptfds, timeout);
		if (ret > 0) {
			int bytes = recv(sock, buf + curPos, allocatedSize - curPos, flags);
			if (bytes == SOCKET_ERROR) {
				printf((char*)WSAGetLastError());
				break;
			}
			if (bytes == 0) {
				*buf++ = (char)'\0';
				return true;
			}
			curPos += bytes;
			if (allocatedSize - curPos < THRESHOLD) {
				//resize buffer with realloc or HeapReAlloc, or memcpy into bigger array

			}
		}
		else if (ret == 0) {
			//report timeout
			printf("Timeout occured");
			break;
		}
		else {
			printf((char*)WSAGetLastError());
			break;
		}
	}
	return false;
}