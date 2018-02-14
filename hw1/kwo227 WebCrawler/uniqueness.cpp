#include "uniqueness.h"

void Uniqueness::checkUniqueHost(string host)
{
	EnterCriticalSection(hostUniqueSection);
	size_t prevSize = seenHosts.size();
	seenHosts.insert(host);
	if (seenHosts.size() == prevSize) {
		LeaveCriticalSection(hostUniqueSection);
		throw (12);
	}
	LeaveCriticalSection(hostUniqueSection);
}

void Uniqueness::checkUniqueIp(DWORD IP)
{
	EnterCriticalSection(IPUniqueSection);
	size_t prevSize = seenIPs.size();
	seenIPs.insert(IP);
	if (seenIPs.size() == prevSize) {
		LeaveCriticalSection(IPUniqueSection);
		throw (12);
	}
	LeaveCriticalSection(IPUniqueSection);
}
