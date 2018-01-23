#pragma once
#include "common.h"
/*
	Much of this code is copied from the test project.
	test project by Dr. Dmitri Loguinov
*/
struct parsed {
	char query[128];
	char path[128];
	char port[128];
	char host[128];
	parsed(char* _query, char* _path, char* _port, char* _host) {
		memset(query, 0, 128);
		memset(path, 0, 128);
		memset(host, 0, 128);
		memset(port, 0, 128);
		if (_query != NULL) memcpy(query, _query, sizeof(query));
		if (_path != NULL) memcpy(path, _path, sizeof(path));
		if (_host != NULL) memcpy(host, _host, sizeof(host));
		if (_port != NULL) memcpy(port, _port, sizeof(port));
	}
};

class URL {
private:
	char* url;
public:
	//TODO: if path returned is null, put root directory in here!
	parsed URLparse(char* URL);
	char* getURL();

	URL(char* url) {
		URL::url = url;
	}
};

void getHeaderInfo(char* buf);
