#pragma once
#include "common.h"
/*
	Much of this code is copied from the test project.
	test project by Dr. Dmitri Loguinov
*/
struct parsed {
	char* query;
	char* path = (char*)"/";
	char* port = (char*)"80";
	char* host;
	parsed(char* query, char* path, char* port, char* host) {
		this->query = query;
		this->path = path;
		this->host = host;
		if(port!=NULL) this->port = port;
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
