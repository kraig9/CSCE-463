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

void winsock_test2(parsed parsedURL, Uniqueness &unique)
{
	// string pointing to an HTTP server (DNS name or IP)
	unique.checkUniqueHost(parsedURL.host);
	char* str = parsedURL.host;
	char baseUrl[256] = "http://";
	strcat_s(baseUrl, parsedURL.host);
	//char str [] = "128.194.135.72";

	WSADATA wsaData;

	//Initialize WinSock; once per program run
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("WSAStartup error %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// open a TCP socket
	printf("\n\tDoing DNS... ");
	//https://stackoverflow.com/questions/5248915/execution-time-of-c-program
	clock_t begin = clock();
	Socket sock;
	sock.sockt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock.sockt == INVALID_SOCKET)
	{
		printf("socket() generated error %d", WSAGetLastError());
		WSACleanup();
		return;
	}

	// structure used in DNS lookups
	struct hostent *remote;

	// structure for connecting to server
	struct sockaddr_in server;

	// first assume that the string is an IP address
	DWORD IP = inet_addr(str);
	if (IP == INADDR_NONE)
	{
		// if not a valid IP, then do a DNS lookup
		if ((remote = gethostbyname(str)) == NULL)
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
	//https://stackoverflow.com/questions/28492110/convert-string-to-short-in-c
	string stringPort = string(parsedURL.port);
	short num = (short)stoi(stringPort);
	server.sin_port = htons(num);		// host-to-network flips the byte order
	clock_t end = clock();
	int duration = (int)(end - begin);
	printf("done in %d ms, found %s", duration, inet_ntoa(server.sin_addr));
	unique.checkUniqueIp(server.sin_addr.S_un.S_addr);

	printf("\n    Connecting on robots... ");
	begin = clock();
	if (connect(sock.sockt, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf("failed with: %d\n", WSAGetLastError());
		throw(10);
	}
	end = clock();
	duration = (int)(end - begin);
	printf("done in %d ms\n", duration);

	//Send http requests here
	//robots HEAD request
	char sendBuf[1000];
	string restOfHead = " HTTP/1.0\r\nUser-agent: kwo227TAMUCrawler/1.2\r\nHost: " + string(parsedURL.host) + "\r\nConnection: close\r\n\r\n";
	string GETHostCat = " /robots.txt" + restOfHead;
	string thing = "HEAD" + GETHostCat;
	strcpy_s(sendBuf, thing.c_str());
	int result = send(sock.sockt, sendBuf, (int)strlen(sendBuf), 0);
	if (result == SOCKET_ERROR) throw(3);
	printf("\tLoading... ");
	begin = clock();
	sock.Read(true);
	string socketBuf = string(sock.buf);
	char* headerInfo = getHeaderInfo(sock.buf);
	char statusBuff[10];
	char* statusPart = headerInfo + 9;
	strncpy_s(statusBuff, statusPart, 3);
	end = clock();
	duration = (int)(end - begin);
	printf("done in %d ms with %d bytes", duration, strlen(sock.buf));
	printf("\n\tVerifying header... status code %s", statusBuff);
	if (statusBuff[0] != '4') {
		closesocket(sock.sockt);
		throw(13);
	}
	closesocket(sock.sockt);

	Socket sock2;
	sock2.sockt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock2.sockt == INVALID_SOCKET)
	{
		printf("socket() generated error %d", WSAGetLastError());
		WSACleanup();
		return;
	}

	printf("\n  * Connecting on page... ");
	if (connect(sock2.sockt, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf("\n\tfailed with: %d", WSAGetLastError());
		throw(10);
	}
	end = clock();
	duration = (int)(end - begin);
	printf("\tdone in %d ms", duration);

	//page GET request
	char sendBuf2[1000];
	begin = clock();
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530747(v=vs.85).aspx
	string GETPath;
	if (parsedURL.path[0] == '/') GETPath = (string)parsedURL.path;
	else GETPath = "/" + (string)parsedURL.path;
	string GETQuery = (string)parsedURL.query;
	string restOfGet = " HTTP/1.0\r\nUser-agent: kwo227TAMUCrawler/1.2\r\nHost: " + string(parsedURL.host) + "\r\nConnection: close\r\n\r\n";
	string GETPathCat = GETPath + GETQuery + restOfGet;
	string thing2 = "GET " + GETPathCat;
	strcpy_s(sendBuf2, thing2.c_str());
	int result2 = send(sock2.sockt, sendBuf2, (int)strlen(sendBuf2), 0);
	if (result2 == SOCKET_ERROR) throw(3);
	end = clock();
	duration = (int)(end - begin);
	printf("done in %d ms\n", duration);


	printf("\tLoading... ");
	begin = clock();
	sock2.Read(false);
	string socketBuf2 = string(sock2.buf);
	char* headerInfo2 = getHeaderInfo(sock2.buf);
	char statusBuff2[10];
	char* statusPart2 = headerInfo2 + 9;
	strncpy_s(statusBuff2, statusPart2, 3);
	end = clock();
	duration = (int)(end - begin);
	printf("done in %d ms with %d bytes", duration, strlen(sock2.buf));
	printf("\n\tVerifying header... status code %s", statusBuff2);

	if (statusBuff2[0] == '2') {
		printf("\n  + Parsing page... ");
		begin = clock();
		int nLinks = createParser(sock2.buf, strlen(sock2.buf), baseUrl);
		end = clock();
		duration = (int)(end - begin);
		printf("done in %d ms with %d links", duration, nLinks);
	}
	// close the socket to this server; open again for the next one
	closesocket(sock2.sockt);

	// call cleanup when done with everything and ready to exit program
	WSACleanup();
}