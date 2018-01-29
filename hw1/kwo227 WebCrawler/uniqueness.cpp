#include "uniqueness.h"

void Uniqueness::checkUniqueHost(string host)
{
	printf("\n\tChecking host uniqueness... ");
	int prevSize = seenHosts.size();
	seenHosts.insert(host);
	if (seenHosts.size() == prevSize) {
		throw (12);
	}
	else printf("passed");
}

void Uniqueness::checkUniqueIp(DWORD IP)
{
	printf("\n\tChecking IP uniqueness... ");
	int prevSize = seenIPs.size();
	seenIPs.insert(IP);
	if (seenIPs.size() == prevSize) {
		throw (12);
	}
	else printf("passed");
}
