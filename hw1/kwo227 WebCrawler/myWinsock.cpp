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

void winsock_test(parsed parsedURL)
{
	// string pointing to an HTTP server (DNS name or IP)
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
	printf("\tDoing DNS... ");
	//https://stackoverflow.com/questions/5248915/execution-time-of-c-program
	clock_t begin = clock();
	Socket sock;
	sock.sockt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock.sockt == INVALID_SOCKET)
	{
		printf("socket() generated error %d\n", WSAGetLastError());
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
			printf("failed with %d\n",WSAGetLastError());
			throw(10);
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
	printf("done in %d ms, found %s\n", duration, inet_ntoa(server.sin_addr));

	printf("      * Connecting on page... ");
	if (connect(sock.sockt, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf("failed with: %d\n", WSAGetLastError());
		throw(10);
	}
	
	//printf("Successfully connected to %s (%s) on port %d\n", str, inet_ntoa(server.sin_addr), htons(server.sin_port));

	// send HTTP requests here
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530747(v=vs.85).aspx
	char sendBuf[1000];
	string GETPath;
	if (parsedURL.path[0] == '/') GETPath = (string)parsedURL.path;
	else GETPath = "/" + (string)parsedURL.path;
	string GETQuery = (string)parsedURL.query;
	string restOfGet = " HTTP/1.0\r\nUser-agent: kwo227TAMUCrawler/1.0\r\nHost: " + string(parsedURL.host) + "\r\nConnection: close\r\n\r\n";
	string GETPathCat = GETPath + GETQuery + restOfGet;
	string thing = "GET " + GETPathCat + " HTTP/1.0\r\n";
	strcpy_s(sendBuf, thing.c_str());
	begin = clock();
	int result = send(sock.sockt, sendBuf, (int)strlen(sendBuf), 0);
	if (result == SOCKET_ERROR) throw(3);
	end = clock();
	duration = (int)(end - begin);
	printf("done in %d ms\n", duration);

	// close the socket to this server; open again for the next one
	printf("\tLoading... ");
	begin = clock();
	sock.Read();
	string socketBuf = string(sock.buf);
	char* headerInfo = getHeaderInfo(sock.buf);
	char statusBuff[10];
	char* statusPart = headerInfo + 9;
	strncpy_s(statusBuff, statusPart, 3);
	end = clock();
	duration = (int)(end - begin);
	printf("done in %d ms with %d bytes", duration, strlen(sock.buf));
	printf("\n\tVerifying header... status code %s", statusBuff);

	if (statusBuff[0] == '2') {
		printf("\n      + Parsing page... ");
		begin = clock();
		int nLinks = createParser(sock.buf, strlen(sock.buf), baseUrl);
		end = clock();
		duration = (int)(end - begin);
		printf("done in %d ms with %d links", duration, nLinks);
	}
	printf("\n\n--------------------------------------\n%s", headerInfo);
	closesocket(sock.sockt);

	// call cleanup when done with everything and ready to exit program
	WSACleanup();
}