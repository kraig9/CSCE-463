#include "stdafx.h"

//response UDPFunc(queryStruct query, std::string serverAddr)
//{
//	unsigned long binRepAddr = inet_addr(queryChar);
//	queryStruct queryContainer;
//	if (!INADDR_NONE) {
//		queryContainer.typePTR = true;
//		queryContainer.inetAddrIP = binRepAddr;
//	}
//	else {
//		queryContainer.inetAddrIP = stoul(*queryChar);
//	}
//	//queryContainer.TXID = (std::string);
//	delete[] queryChar;
//	response UDPResponse;
//
//	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
//	//handle errors
//	struct sockaddr_in local;
//	memset(&local, 0, sizeof(local));
//	local.sin_family = AF_INET;
//	local.sin_addr.s_addr = INADDR_ANY;
//	local.sin_port = htons(0);
//	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR) {
//		// handle errors	
//		throw("bind failed");
//	}
//		
//	struct sockaddr_in remote;
//	memset(&remote, 0, sizeof(remote));
//	remote.sin_family = AF_INET;
//	remote.sin_addr = inet_addr(...);
//	remote.sin_port = htons(53);
//	if (sendto(sock, buf, size, 0, (struct sockaddr*)&remote, sizeof(remote)) == SOCKET_ERROR) {
//		throw("sendto failed");
//	}
//	return UDPResponse;
//}
//
//userOutput responseParse(response input)
//{
//	userOutput output;
//	return output;
//}


int sendStuff(int size, std::string serverIP, char* buf1, char* buf2) {
	WSADATA wsaData;
	int recvSize;
	//Initialize WinSock; once per program run
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("WSAStartup error %d\n", WSAGetLastError());
		WSACleanup();
		exit(1);
	}
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	//handle errors

	struct sockaddr_in local;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(0);
	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR) {
		// handle errors	
		printf("socket error %d\n", WSAGetLastError());
		exit(1);
	}
	
	struct sockaddr_in remote;
	memset(&remote, 0, sizeof(remote));
	remote.sin_family = AF_INET;
	remote.sin_addr.S_un.S_addr = inet_addr(serverIP.c_str());
	remote.sin_port = htons(53);
	struct sockaddr_in response;
	int responseSize = sizeof(response);
	
	int count = 0;
	while (count++ < MAX_ATTEMPTS) {
		std::cout << "Attempt " << count << " with " << std::dec << size << " bytes... ";
		//send request to the server
		clock_t begin = clock();
		if (sendto(sock, buf1, size, 0, (struct sockaddr*)&remote, sizeof(remote)) == SOCKET_ERROR) {
			printf("socket error %d\n", WSAGetLastError());
			exit(1);
		}
		
		//get ready to recieve
		fd_set fd;
		timeval timeout;
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		FD_ZERO(&fd); // clear the set
		FD_SET(sock, &fd); // add your socket to the set
		int available = select(0, &fd, NULL, NULL, &timeout);
		if (available > 0) {
			recvSize = recvfrom(sock, buf2, MAX_DNS_SIZE, 0, (struct sockaddr*) &response, &responseSize);
			if (recvSize == SOCKET_ERROR) {
				printf("socket error %d\n", WSAGetLastError());
				exit(1);
			}
			clock_t end = clock();
			int duration = (int)(end - begin);
			std::cout << "response in " << duration << " ms with " << recvSize << " bytes" << std::endl;
			if (recvSize < sizeof(FixedDNSheader)) {
				printf("  ++ invalid reply: smaller than fixed header\n");
				exit(1);
			}
			//parse the response
			//break from the loop
			break;
		}
		//some error checking here
	}
	//check if the packet came from the server that we sent the query to earlier
	if (response.sin_addr.S_un.S_addr != remote.sin_addr.S_un.S_addr || response.sin_port != remote.sin_port) {
		printf("%d", WSAGetLastError());
		exit(1);
	}
	return recvSize;
}

//parses queries and gets the dots out
std::vector<char> QuestionParser(char * buf2)
{
	std::vector<char> charVec;
	int i = int(buf2[0]);
	buf2++;
	while (i >= 0) {
		if (i == 0) {
			i = buf2[0];
			if (i == 0)break;
			i = int(buf2[0]);
			charVec.push_back('.');
			buf2++;
		}
		else{
			charVec.push_back(buf2[0]);
			buf2++;
			i--;
		}	
	}
	return charVec;
}

//recursion function for nested jumps
std::vector<char> jumpRecur(std::unordered_set<short>& loopCheck, int size, short offset, char* buf) {
	std::vector<char> charVec;
	int pos = offset + 1;
	if (pos + 1 > size) {
		printf("  ++ invalid record: truncated jump offset\n");
		exit(1);
	}
	offset = ntohs(*((short*)(buf + pos))) & 0x3fff;
	int size1 = loopCheck.size();
	loopCheck.insert(offset);
	int size2 = loopCheck.size();
	if (size1 == size2) {
		printf(" ++ invalid record: jump loop\n");
		exit(1);
	}
	if (offset < sizeof(FixedDNSheader)) {
		printf("  ++  invalid record: jump into fixed RR header\n");
		exit(1);
	}
	else if (offset > size) {
		printf("  ++ invalid record: jump beyond packet boundary\n");
		exit(1);
	}
	unsigned char jumpCheck = buf[offset];
	while (jumpCheck < (unsigned char)0xc0 && buf[offset] != 0) {
		charVec.push_back(buf[offset]);
		jumpCheck = buf[offset + 1];
		if (jumpCheck >= (unsigned char)0xc0) {
			std::vector<char> tempVec = jumpRecur(loopCheck, size, offset, buf);
			//https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors
			charVec.insert(charVec.end(), tempVec.begin(), tempVec.end());
			break;
		}
		offset++;
	}
	return charVec;
}

//The function to parse all answers. 
std::vector<char> jumpyFunc(int size, int& pos, char * buf)
{
	std::vector<char> answerVec;
	std::unordered_set<short> loopCheck;
	unsigned char jumpCheck = buf[pos];
	while (buf[pos] != 0) {
		jumpCheck = buf[pos];
		if (pos + 1 > size) {
			printf("  ++ invalid record: truncated jump offset\n");
			exit(1);
		}
		if (jumpCheck >= (unsigned char)0xc0) {
			//we have a jump
			short off = ntohs(*((short*)(buf + pos))) & 0x3fff;
			int size1 = loopCheck.size();
			loopCheck.insert(off);
			int size2 = loopCheck.size();
			if (size1 == size2) {
				printf(" ++ invalid record: jump loop\n");
				exit(1);
			}
			else if (off < sizeof(FixedDNSheader)) {
				printf("  ++  invalid record: jump into fixed RR header\n");
				exit(1);
			}
			else if (off > size) {
				printf("  ++ invalid record: jump beyond packet boundary\n");
				exit(1);
			}
			jumpCheck = buf[off + 1];
			while (jumpCheck < (unsigned char)0xc0 && buf[off] != 0) {
				answerVec.push_back(buf[off]);
				jumpCheck = buf[off + 1];
				if (jumpCheck >= (unsigned char)0xc0) {
					std::vector<char> tempVec = jumpRecur(loopCheck, size, off, buf);
					//https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors
					answerVec.insert(answerVec.end(), tempVec.begin(), tempVec.end());
					pos += 2;
					return answerVec;
				}
				off++;
			}
			if (buf[off] == 0) {
				pos += 2;
				break;
			}
			//if (jumpCheck < (unsigned char)0xc0)pos += 2;
		}
		else {
			answerVec.push_back(buf[pos]);
			if (buf[pos + 1] == 0) {
				pos += 2;
				break;
			}
			pos++;
		}
	}
	return answerVec;
}
