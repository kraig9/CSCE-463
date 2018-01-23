//This code was taken from the sample code in winsock.cpp
//written by Dr. Dmitri Loguinov, modified by Kraig Orcutt

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include <stdio.h>
#include <winsock2.h>
#include "Socket.h"
#include "URLparser.h"

void winsock_test(parsed parsedURL)
{
	// string pointing to an HTTP server (DNS name or IP)
	printf(parsedURL.host);
	char* str = parsedURL.host;
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
			printf("Invalid string: neither FQDN, nor IP address\n");
			return;
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
		printf("Connection error: %d\n", WSAGetLastError());
		return;
	}

	printf("Successfully connected to %s (%s) on port %d\n", str, inet_ntoa(server.sin_addr), htons(server.sin_port));

	// send HTTP requests here
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb530747(v=vs.85).aspx
	char sendBuf[1000];
	string thing = "GET /?status=15 HTTP/1.0\r\n\r\n";
	strcpy_s(sendBuf, thing.c_str());
	int result = send(sock.sockt, sendBuf, (int)strlen(sendBuf), 0);
	printf("THE RESULT FROM THE GET WAS: %d", result);
	// close the socket to this server; open again for the next one
	sock.Read();
	//recv(sock.sockt, sock.buf, sock.allocatedSize, 0);
	printf("%s",sock.buf);

	closesocket(sock.sockt);

	// call cleanup when done with everything and ready to exit program
	WSACleanup();
}