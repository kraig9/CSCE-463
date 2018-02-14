//Kraig Orcutt, CSCE 463-500 Spring 2018

#pragma once
#include "common.h"
#include "HTMLParserBase.h"
/*
	Much of this code is copied from the test project.
	test project by Dr. Dmitri Loguinov
*/
struct parsed {
	char query[MAX_REQUEST_LEN];
	char path[MAX_PATH_LEN];
	char port[4];
	char host[MAX_HOST_LEN];
	parsed(char* _query, char* _path, char* _port, char* _host) {
		memset(query, 0, MAX_REQUEST_LEN);
		memset(path, 0, MAX_PATH_LEN);
		memset(host, 0, MAX_HOST_LEN);
		memset(port, 0, 4);
		if (_query != NULL) memcpy(query, _query, sizeof(query));
		if (_path != NULL) memcpy(path, _path, sizeof(path));
		if (_host != NULL) memcpy(host, _host, sizeof(host));
		if (_port != NULL) memcpy(port, _port, sizeof(port));
	}
};
parsed URLparse(char* URL);
void getHeaderInfo(char* buf, char* headerInp);
