#pragma once
#include "common.h"
/*
	Much of this code is copied from the test project.
	test project by Dr. Dmitri Loguinov
*/
struct parsed {
	char* query, *path, *port, *host;
	parsed(char* query, char* path, char* port, char* host) {
		this->query = query;
		this->path = path;
		this->port = port;
		this->host = host;
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
