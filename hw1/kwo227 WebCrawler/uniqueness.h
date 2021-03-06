#pragma once
#include "common.h"
#include "URLParser.h"

struct Uniqueness{
	//This code was taken from HW1 Part 2 of
	//Dr. Loguinov's HW1 assignment
	unordered_set<string> seenHosts;
	unordered_set<DWORD> seenIPs;

	void checkUniqueHost(string host);
	void checkUniqueIp(DWORD IP);
	CRITICAL_SECTION* hostUniqueSection;
	CRITICAL_SECTION* IPUniqueSection;
public:
	Uniqueness(CRITICAL_SECTION& hostUniqueSection,CRITICAL_SECTION& IPUniqueSection) {
		Uniqueness::hostUniqueSection = &hostUniqueSection;
		Uniqueness::IPUniqueSection = &IPUniqueSection;
	}
};