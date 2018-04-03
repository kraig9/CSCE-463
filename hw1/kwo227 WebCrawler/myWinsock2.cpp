//Kraig Orcutt, CSCE 463-500 Spring 2018

//This code was taken from the sample code in winsock.cpp
//written by Dr. Dmitri Loguinov, modified by Kraig Orcutt

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include <stdio.h>
#include <winsock2.h>
#include "Socket.h"
#include "URLparser.h"
#include "HTMLParserBase.h"

void winsock_test2(parsed parsedURL, threadParams &inpParam)
{
	Uniqueness* unique = inpParam.uniquePoint;
	// string pointing to an HTTP server (DNS name or IP)
	unique->checkUniqueHost(parsedURL.host);
	inpParam.incH();
	string str = parsedURL.host;
	string baseUrl = "http://";
	baseUrl = baseUrl + parsedURL.host;
	//char str [] = "128.194.135.72";

	WSADATA wsaData;

	//Initialize WinSock; once per program run
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		WSACleanup();
		return;
	}
	// open a TCP socket
	//https://stackoverflow.com/questions/5248915/execution-time-of-c-program
	clock_t begin = clock();
	Socket sock;
	sock.sockt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock.sockt == INVALID_SOCKET)
	{
		WSACleanup();
		return;
	}

	// structure used in DNS lookups
	struct hostent *remote;

	// structure for connecting to server
	struct sockaddr_in server;

	// first assume that the string is an IP address
	DWORD IP = inet_addr(str.c_str());
	if (IP == INADDR_NONE)
	{
		// if not a valid IP, then do a DNS lookup
		if ((remote = gethostbyname(str.c_str())) == NULL)
		{
			throw(3);
		}
		else // take the first IP address and copy into sin_addr
			memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
	}
	else
	{
		// if a valid IP, directly drop its binary version into sin_addr
		server.sin_addr.S_un.S_addr = IP;
	}
	// setup the port # and protocol type
	server.sin_family = AF_INET;
	inpParam.incD();
	//https://stackoverflow.com/questions/28492110/convert-string-to-short-in-c
	server.sin_port = htons(parsedURL.port);		// host-to-network flips the byte order
	clock_t end = clock();
	int duration = (int)(end - begin);
	unique->checkUniqueIp(server.sin_addr.S_un.S_addr);
	inpParam.incI();

	begin = clock();
	if (connect(sock.sockt, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		throw(10);
	}
	end = clock();
	duration = (int)(end - begin);

	//Send http requests here
	//robots HEAD request
	char sendBuf[4096];
	string restOfHead = " HTTP/1.0\r\nUser-agent: kwo227TAMUCrawler/1.2\r\nHost: " + parsedURL.host + "\r\nConnection: close\r\n\r\n";
	string GETHostCat = " /robots.txt" + restOfHead;
	string thing = "HEAD" + GETHostCat;
	strcpy_s(sendBuf, thing.c_str());
	int result = send(sock.sockt, sendBuf, (int)strlen(sendBuf), 0);
	if (result == SOCKET_ERROR) throw(3);
	begin = clock();
	sock.Read(true);
	string socketBuf = string(sock.buf);
	char headerInfo[10000];
	getHeaderInfo(sock.buf, headerInfo);
	char statusBuff[10];
	char* statusPart = headerInfo + 9;
	strncpy_s(statusBuff, statusPart, 3);
	end = clock();
	duration = (int)(end - begin);
	if (statusBuff[0] != '4') {
		closesocket(sock.sockt);
		throw(13);
	}
	inpParam.incMB((strlen(sock.buf)*8)/1000000);
	closesocket(sock.sockt);
	inpParam.incR();

	Socket sock2;
	sock2.sockt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock2.sockt == INVALID_SOCKET)
	{
		WSACleanup();
		return;
	}

	if (connect(sock2.sockt, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		throw(10);
	}
	end = clock();
	duration = (int)(end - begin);

	//page GET request
	char sendBuf2[4096];
	begin = clock();
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530747(v=vs.85).aspx
	string GETPath;
	if (parsedURL.path[0] == '/') GETPath = (string)parsedURL.path;
	else GETPath = "/" + (string)parsedURL.path;
	string GETQuery = (string)parsedURL.query;
	string restOfGet = " HTTP/1.0\r\nUser-agent: kwo227TAMUCrawler/1.2\r\nHost: " + parsedURL.host + "\r\nConnection: close\r\n\r\n";
	string GETPathCat = GETPath + GETQuery + restOfGet;
	string thing2 = "GET " + GETPathCat;
	strcpy_s(sendBuf2, thing2.c_str());
	int result2 = send(sock2.sockt, sendBuf2, (int)strlen(sendBuf2), 0);
	if (result2 == SOCKET_ERROR) throw(3);
	end = clock();
	duration = (int)(end - begin);


	begin = clock();
	sock2.Read(false);
	string socketBuf2 = string(sock2.buf);
	char headerInfo2[10000];
	getHeaderInfo(sock2.buf, headerInfo2);
	char statusBuff2[10];
	char* statusPart2 = headerInfo2 + 9;
	strncpy_s(statusBuff2, statusPart2, 3);
	end = clock();
	duration = (int)(end - begin);
	switch (statusBuff2[0]) {
	case '2':inpParam.inc2xx();
		break;
	case '3':inpParam.inc3xx();
		break;
	case '4':inpParam.inc4xx();
		break;
	case '5':inpParam.inc5xx();
		break;
	default:inpParam.incother();
		break;
	}
	if (statusBuff2[0] == '2') {
		begin = clock();
		//int nLinks = createParser(sock2.buf, strlen(sock2.buf), baseUrl);
		EnterCriticalSection(inpParam.parserCriticalSection);
		int nLinks = HTMLParse(inpParam.parser, sock2.buf, strlen(sock2.buf), baseUrl);
		LeaveCriticalSection(inpParam.parserCriticalSection);
		end = clock();
		duration = (int)(end - begin);
		inpParam.incC();
		inpParam.incL(nLinks);
		inpParam.incMB((strlen(sock2.buf)*8)/1000000);
		inpParam.incpp(1);
	}

	// close the socket to this server; open again for the next one
	closesocket(sock2.sockt);

	// call cleanup when done with everything and ready to exit program
}