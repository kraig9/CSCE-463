// hw2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

//https://msdn.microsoft.com/en-us/library/1z2f6c2k.aspx
ostream& operator<<(ostream& os, const vector<char> charVec) {
	for (int i = 0; i < charVec.size(); i++) {
		os << charVec[i];
	}
	return os;
}
char* operator<<(char* buf, const vector<char> charVec) {
	for (int i = 0; i < charVec.size(); i++) {
		buf[i] = charVec[i];
	}
	buf[charVec.size()] = '\0';
	return buf;
}
void debuggyFunc(int position, char* buf2) {
	printf("value at pos %d is %x %x %x %x %x %x %x %x\n",position, buf2[position], buf2[position + 1], buf2[position + 2], buf2[position + 3], buf2[position + 4], buf2[position + 5], buf2[position + 6], buf2[position + 7]);
}

int main(int argc, char* argv[])
{
	try {
		if (argc < 3) {
			string error = "bad args";
			throw(error);
		}
		string lookup (argv[1]);
		string DNSIP (argv[2]);
		string lookup2 = lookup;
		string DNSIP2 = DNSIP;
		if(inet_addr(lookup.c_str()) != INADDR_NONE) {
			//insert code for reverse lookup here
			string part1, part2, part3, part4;
			int periodCount = 0;
			for (int i = 0; i < lookup.size(); i++) {
				if (lookup[i] == '.') {
					periodCount++;
					continue;
				}
				switch (periodCount) {
				case 0:
					part4.push_back(lookup[i]);
					break;
				case 1:
					part3.push_back(lookup[i]);
					break;
				case 2:
					part2.push_back(lookup[i]);
					break;
				case 3:
					part1.push_back(lookup[i]);
					break;
				}
			}
			string reverseIP = part1 + '.' + part2 + '.' + part3 + '.' + part4
				+ '.' + "in-addr.arpa";
			string tempString = "0";
			int size = 0;
			for (int i = 0; i <= reverseIP.size(); i++) {
				if (reverseIP[i] == '.' || i == reverseIP.size()) {
					tempString[tempString.size() - size - 1] = (char)size;
					size = 0;
					tempString.push_back(reverseIP[i]);
				}
				else {
					tempString.push_back(reverseIP[i]);
					size++;
				}
			}
			lookup = tempString;
		}

		//construct the query
		int pkt_size;
		if (inet_addr(lookup2.c_str()) != INADDR_NONE) {
			pkt_size = lookup.size() + sizeof(FixedDNSheader) + sizeof(QueryHeader);
		}
		else {
			pkt_size = lookup.size() + 2 + sizeof(FixedDNSheader) + sizeof(QueryHeader);
		}
		char *buf1 = new char[pkt_size];
		FixedDNSheader *fdh = (FixedDNSheader *)buf1;
		QueryHeader *qh = (QueryHeader*)(buf1 + pkt_size - sizeof(QueryHeader));

		//fixed field intialization
		fdh->ID = htons(0);
		fdh->flags = htons(DNS_QUERY | DNS_RD | DNS_STDQUERY);
		fdh->questions = htons(1);
		fdh->answers = htons(0);
		fdh->authority = htons(0);
		fdh->additional = htons(0);
		if (short inetRet = inet_addr(lookup2.c_str()) != INADDR_NONE) qh->qType = htons(DNS_PTR);
		else qh->qType = htons(DNS_A);
		qh->qClass = htons(DNS_INET);
		if (qh->qType == htons(DNS_A)) {
			string tempString = "0";
			int size = 0;
			for (int i = 0; i <= lookup.size(); i++) {
				if (lookup[i] == '.' || i == lookup.size()) {
					tempString[tempString.size() - size - 1] = (char)size;
					size = 0;
					tempString.push_back(lookup[i]);
				}
				else {
					tempString.push_back(lookup[i]);
					size++;
				}
			}
			lookup = tempString;
		}
		
		char* query = (char*)(buf1 + pkt_size - sizeof(QueryHeader) - lookup.size());
		char* tempQuery = new char[lookup.size() + 1];
		memcpy(tempQuery, lookup.c_str(), lookup.size());
		tempQuery[lookup.size()] = '\0';
		memcpy(query, tempQuery, strlen(tempQuery) + 1);
		delete[] tempQuery;
		char *buf2 = new char[MAX_DNS_SIZE];
		cout << "Lookup\t: " << lookup2 << endl;
		if (qh->qType == htons(DNS_PTR)) {
			cout << "Query\t: " << lookup << ", type " << ntohs(qh->qType) << ", TXID " << std::hex << 0 << endl;
		}
		else {
			cout << "Query\t: " << lookup2 << ", type " << ntohs(qh->qType) << ", TXID " << std::hex << 0 << endl;
		}
		cout << "Server\t: " << DNSIP2 << endl;
		cout << "********************************" << endl;

		int recvSize = sendStuff(pkt_size, DNSIP, buf1, buf2);

		//parse response
		FixedDNSheader *fdh2 = (FixedDNSheader *)buf2;
		if (ntohs(fdh->ID) != ntohs(fdh2->ID)) {
			printf("  ++ invalid reply, TXID mismatch, sent 0x%x, recieved 0x%x\n", fdh->ID, fdh2->ID);
			exit(1);
		}

		printf("  TXID 0x%x, flags 0x%x, questions %d, answers %d, authority %d, additional %d\n", ntohs(fdh2->ID), ntohs(fdh2->flags), ntohs(fdh2->questions), ntohs(fdh2->answers), ntohs(fdh2->authority), ntohs(fdh2->additional));

		//check the rcode
		USHORT rcode = ntohs(fdh2->flags)&0000000000001111;
		if (rcode != 0) {
			printf("  failed with Rcode = %d\n", rcode);
			exit(1);
		}
		else {
			printf("  succeeded with Rcode = 0\n");
		}

		printf("  ------------ [questions] ---------- \n");
		//get the question out
		char questionBuf[1000];
		//position will keep track of where we are throughout the parsing of the response.
		int position = sizeof(FixedDNSheader);
		for (; buf2[position] != 0; position++) {
			questionBuf[position - sizeof(FixedDNSheader)] = buf2[position];
		}
		questionBuf[position - sizeof(FixedDNSheader)] = 0;
		vector<char> parseReturn = QuestionParser(questionBuf);
		position += 1;
		QueryHeader *qh2 = (QueryHeader*)(buf2 + position);
		position += sizeof(QueryHeader);
		cout << "  \t" << parseReturn << " type " << ntohs(qh2->qType) << " class " << ntohs(qh2->qClass) << endl;

		//position should now be past the fixedDNSHeader, the question, and the question header.

		//now need to check answer for compression and parse the asnwers. 
		//first answers
		printf("  ------------ [answers] ------------ \n");
		for (int i = 0; i < htons(fdh2->answers); i++) {
			vector<char> answerRet = jumpyFunc(recvSize, position, buf2);
			char* answerBuf = new char[answerRet.size()+1];
			answerBuf << answerRet;
			answerRet = QuestionParser(answerBuf);
			//now cast the header.
			FixedRR *ah = (FixedRR*)(buf2 + position);
			if (htons(ah->length) > MAX_DNS_SIZE) {
				printf("RR value lenth beyond packet\n");
				exit(1);
			}
			if (ntohs(ah->qType) == DNS_A || ntohs(ah->qType) == DNS_CNAME || ntohs(ah->qType) == DNS_NS || ntohs(ah->qType) == DNS_PTR) {
				string output;
				switch (ntohs(ah->qType)) {
				case DNS_A:
					output = "A";
					break;
				case DNS_CNAME:
					output = "CNAME";
					break;
				case DNS_NS:
					output = "NS";
					break;
				case DNS_PTR:
					output = "PTR";
					break;
				}
				cout << "  \t" << answerRet << " " << output << ' ';
			}
			//got the header, now check the answer
			position += sizeof(FixedRR);
			//check if cname or IP or some other
			vector<char> holder;
			//check if IP
			if (htons(ah->qType) == DNS_A) {
				//check length of IP
				if (htons(ah->length) != 4) {
					printf("IP length is less than 4.");
					exit(1);
				}
				int ip = *((int*)(buf2 + position));
				//https://stackoverflow.com/questions/1680365/integer-to-ip-address-c
				unsigned char bytes[4];
				bytes[0] = (ip >> 24) & 0xFF;
				bytes[1] = (ip >> 16) & 0xFF;
				bytes[2] = (ip >> 8) & 0xFF;
				bytes[3] = ip & 0xFF;
				printf("%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
				position += 4;
			}
			//special case. if type AAAA skip over it.
			else if (htons(ah->qType) == 28) {
				position += htons(ah->length);
				continue;
			}
			//parse the answer given, probably a CNAME or something
			else {
				int savedPos = position;
				holder = jumpyFunc(recvSize, position, buf2);
				char* answerBuf2 = new char[holder.size() + 1];
				answerBuf2 << holder;
				holder = QuestionParser(answerBuf2);
				cout << holder;
				savedPos += htons(ah->length);
				position = savedPos;
			}
			cout << " TTL = " << ntohs(ah->TTL) << endl;
			delete[] answerBuf;
		}
		//now for authoritative answers
		cout << "  ------------ [authority] ---------- " << endl;
		for (int i = 0; i < htons(fdh2->authority); i++) {
			vector<char> answerRet = jumpyFunc(recvSize, position, buf2);
			char* answerBuf = new char[answerRet.size() + 1];
			answerBuf << answerRet;
			answerRet = QuestionParser(answerBuf);
			//now cast the header.
			FixedRR *ah = (FixedRR*)(buf2 + position);
			if (htons(ah->length) > MAX_DNS_SIZE) {
				printf("RR value lenth beyond packet\n");
				exit(1);
			}
			if (ntohs(ah->qType) == DNS_A || ntohs(ah->qType) == DNS_CNAME || ntohs(ah->qType) == DNS_NS || ntohs(ah->qType) == DNS_PTR) {
				string output;
				switch (ntohs(ah->qType)) {
				case DNS_A:
					output = "A";
					break;
				case DNS_CNAME:
					output = "CNAME";
					break;
				case DNS_NS:
					output = "NS";
					break;
				case DNS_PTR:
					output = "PTR";
					break;
				}
				cout << "  \t" << answerRet << " " << output << ' ';
			}
			//got the header, now check the answer
			position += sizeof(FixedRR);
			//check if cname or IP or some other
			vector<char> holder;
			//check if IP
			if (htons(ah->qType) == DNS_A) {
				//check length of IP
				if (htons(ah->length) != 4) {
					printf("IP length is less than 4.");
					exit(1);
				}
				int ip = *((int*)(buf2 + position));
				//https://stackoverflow.com/questions/1680365/integer-to-ip-address-c
				unsigned char bytes[4];
				bytes[0] = (ip >> 24) & 0xFF;
				bytes[1] = (ip >> 16) & 0xFF;
				bytes[2] = (ip >> 8) & 0xFF;
				bytes[3] = ip & 0xFF;
				printf("%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
				position += 4;
			}
			//special case. if type AAAA skip over it.
			else if (htons(ah->qType) == 28) {
				position += htons(ah->length);
				continue;
			}
			//parse the answer given, probably a CNAME or something
			else {
				int savedPos = position;
				holder = jumpyFunc(recvSize, position, buf2);
				char* answerBuf2 = new char[holder.size() + 1];
				answerBuf2 << holder;
				holder = QuestionParser(answerBuf2);
				cout << holder;
				savedPos += htons(ah->length);
				position = savedPos;
			}
			cout << " TTL = " << ntohs(ah->TTL) << endl;
			delete[] answerBuf;
		}
		cout << "  ------------ [additional] ---------  " << endl;
		//now for additional
		for (int i = 0; i < htons(fdh2->additional); i++) {
			vector<char> answerRet = jumpyFunc(recvSize, position, buf2);
			char* answerBuf = new char[answerRet.size() + 1];
			answerBuf << answerRet;
			answerRet = QuestionParser(answerBuf);
			//now cast the header.
			FixedRR *ah = (FixedRR*)(buf2 + position);
			if (htons(ah->length) > MAX_DNS_SIZE) {
				printf("RR value lenth beyond packet\n");
				exit(1);
			}
			if (ntohs(ah->qType) == DNS_A || ntohs(ah->qType) == DNS_CNAME|| ntohs(ah->qType) == DNS_NS|| ntohs(ah->qType) == DNS_PTR) {
				string output;
				switch(ntohs(ah->qType)){
				case DNS_A:
					output = "A";
					break;
				case DNS_CNAME:
					output = "CNAME";
					break;
				case DNS_NS:
					output = "NS";
					break;
				case DNS_PTR:
					output = "PTR";
					break;
				}
				cout << "  \t" << answerRet << " " << output << ' ';
			}

			//got the header, now check the answer
			position += sizeof(FixedRR);
			//check if cname or IP or some other
			vector<char> holder;
			//check if IP
			if (htons(ah->qType) == DNS_A) {
				//check length of IP
				if (htons(ah->length) != 4) {
					printf("IP length is less than 4.");
					exit(1);
				}
				int ip = *((int*)(buf2 + position));
				//https://stackoverflow.com/questions/1680365/integer-to-ip-address-c
				unsigned char bytes[4];
				bytes[0] = (ip >> 24) & 0xFF;
				bytes[1] = (ip >> 16) & 0xFF;
				bytes[2] = (ip >> 8) & 0xFF;
				bytes[3] = ip & 0xFF;
				printf("%d.%d.%d.%d ", bytes[3], bytes[2], bytes[1], bytes[0]);
				position += 4;
			}
			//special case. if type AAAA skip over it.
			else if (htons(ah->qType) == 28) {
				position += htons(ah->length);
				continue;
			}
			//parse the answer given, probably a CNAME or something
			else {
				int savedPos = position;
				holder = jumpyFunc(recvSize, position, buf2);
				char* answerBuf2 = new char[holder.size() + 1];
				answerBuf2 << holder;
				holder = QuestionParser(answerBuf2);
				cout << holder;
				savedPos += htons(ah->length);
				position = savedPos;
			}
			cout << " TTL = " << ntohs(ah->TTL) << endl;
			delete[] answerBuf;
		}
		

		//userOutput output = responseParse(DNSresponse);
		delete[] buf1;
		delete[] buf2;
	}
	//catch specific errors
	catch (string error) {
		if (error == "bad args") {
			cout << "Wrong number of arguments. "
				<< "correct usage is <hostname or IP to lookup> "
				<< "<DNS server IP>" << endl;
		}
		else if (error == "bind failed") cout << "failure on bind function" << endl;
		else if (error == "sendto failed") cout << "failure on sendto function" << endl;
		else if (error == "recvfrom failed") {
			cout << "failure on recvfrom function" << endl;
			WSAGetLastError();
		}
		else if (error == "phony") cout << "bogus reply" << endl;
		return 1;
	}
	//catch other errors like WSAGetLastError
	/*catch (int error) {

	}*/
    return 0;
}

