#pragma once

#include "stdafx.h"

/* DNS query types */
#define DNS_A 1 /* name -> IP */
#define DNS_NS 2 /* name server */ 
#define DNS_CNAME 5 /* canonical name */ 
#define DNS_PTR 12 /* IP -> name */
#define DNS_HINFO 13 /* host info/SOA */
#define DNS_MX 15 /* mail exchange */
#define DNS_AXFR 252 /* request for zone transfer */
#define DNS_ANY 255 /* all records */ 

/* query classes */
#define DNS_INET 1 

/*reading raw buffers*/
#define MAX_DNS_SIZE 512 // largest valid UDP packet 

/* flags */
#define DNS_QUERY (0 << 15)
#define DNS_RESPONSE (1 << 15) 

#define DNS_STDQUERY (0 << 11)

#define DNS_AA (1 << 10)
#define DNS_TC (1 << 9)
#define DNS_RD (1 << 8)
#define DNS_RA (1 << 7)

//other constants
#define MAX_DNS_SIZE 512
#define MAX_ATTEMPTS 3

#pragma pack(push,1)
struct QueryHeader {
	USHORT qType;
	USHORT qClass;
};
struct FixedDNSheader{
	USHORT ID;
	USHORT flags;
	USHORT questions;
	USHORT answers;
	USHORT authority;
	USHORT additional;
};
#pragma pack(pop)
#pragma pack(push,1)
struct FixedRR {
	USHORT qType;
	USHORT qClass;
	int TTL;
	USHORT length;
};
#pragma pack(pop)

int sendStuff(int size, std::string serverIP, char* buf1, char* buf2);
std::vector<char> QuestionParser(char * buf2);
std::vector<char> jumpyFunc(int size, int& pos, char* buf);
//queryStruct queryConstructor(std::string input);
//response UDPFunc(queryStruct query, std::string serverAddr);
//userOutput responseParse(response input);