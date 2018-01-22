#include "Socket.h"

char* Socket::getBuf() {
	return buf;
}
int Socket::getSize() {
	return allocatedSize;
}
int Socket::getCurPos() {
	return curPos;
}