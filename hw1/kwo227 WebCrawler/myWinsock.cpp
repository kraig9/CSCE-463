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
	server.sin_port = htons(80);		// host-to-network flips the byte order

										// connect to the server on port 80
	if (connect(sock.sockt, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf("failed with: %d\n", WSAGetLastError());
		throw(10);
	}
	clock_t end = clock();
	int duration = (int)(end - begin);
	//printf("Successfully connected to %s (%s) on port %d\n", str, inet_ntoa(server.sin_addr), htons(server.sin_port));
	printf("done in %d ms, found %s\n", duration, inet_ntoa(server.sin_addr));

	// send HTTP requests here
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530747(v=vs.85).aspx
	char sendBuf[1000];
	string thing = "GET /?status=15 HTTP/1.0\r\n\r\n";
	strcpy_s(sendBuf, thing.c_str());
	begin = clock();
	printf("      * Connecting on page... ");
	int result = send(sock.sockt, sendBuf, (int)strlen(sendBuf), 0);
	if (result == SOCKET_ERROR) throw(3);
	end = clock();
	duration = (int)(end - begin);
	printf("done in %d ms\n", duration);

	// close the socket to this server; open again for the next one
	printf("\tLoading... ");
	begin = clock();
	sock.Read();
	end = clock();
	duration = (int)(end - begin);
	printf("done in %d ms with %d bytes\n", duration, strlen(sock.buf));
	char HTMLBody[500000];
	char* headerInfo = getHeaderInfo(sock.buf, HTMLBody);
	char statusBuff[10];
	char* statusPart = sock.buf + 9;
	strncpy_s(statusBuff, statusPart, 3);
	printf("\tVerifying header... status code %s\n", statusBuff);

	if (statusBuff[0] == '2') {
		printf("      + Parsing page... ");
		begin = clock();
		int nLinks = createParser(HTMLBody, strlen(HTMLBody), baseUrl);
		end = clock();
		duration = (int)(end - begin);
		printf("done in %d ms with %d links\n", duration, nLinks);
	}
	printf("\n\n--------------------------------------\n%s", headerInfo);
	closesocket(sock.sockt);

	// call cleanup when done with everything and ready to exit program
	WSACleanup();
}