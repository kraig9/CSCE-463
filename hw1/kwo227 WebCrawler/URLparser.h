//Kraig Orcutt, CSCE 463-500 Spring 2018

#pragma once
#include "common.h"
#include "HTMLParserBase.h"
/*
	Much of this code is copied from the test project.
	test project by Dr. Dmitri Loguinov
*/
struct parsed {
	/*char query[MAX_REQUEST_LEN];
	char path[MAX_PATH_LEN];
	char port[4];
	char host[MAX_HOST_LEN];*/
	//char* query;
	//char* path;
	//char* port;
	//char* host;
	//parsed(char* _query, char* _path, char* _port, char* _host) {
	//	if (_query != NULL) {
	//		query = new char[strlen(_query) + 1];
	//		memcpy_s(query, strlen(_query) + 1, _query, strlen(_query) + 1);
	//	}
	//	else {
	//		query = new char[1];
	//		query[0] = 0;
	//	}
	//	if (_path != NULL) {
	//		path = new char[strlen(_path) + 1];
	//		memcpy_s(path, strlen(_path) + 1, _path, strlen(_path) + 1);
	//	}
	//	else {
	//		path = new char[1];
	//		path[0] = 0;
	//	}
	//	if (_port != NULL) {
	//		port = new char[strlen(_port) + 1];
	//		memcpy_s(port, strlen(_port) + 1, _port, strlen(_port) + 1);
	//	}
	//	else {
	//		port = new char[1];
	//		port[0] = 0;
	//	}
	//	if (_host != NULL) {
	//		host = new char[strlen(_host) + 1];
	//		memcpy_s(host, strlen(_host) + 1, _host, strlen(_host) + 1);
	//	}
	//	else {
	//		host = new char[1];
	//		host[0] = 0;
	//	}
	//}
	//void cleanup() {
	//	delete[] query;
	//	delete[] path;
	//	delete[] port;
	//	delete[] host;
	//}
	string query;
	string path;
	short port;
	string host;
	parsed(string query, string path, int port, string host) {
		this->query = query;
		this->path = path;
		this->port = (short)port;
		this->host = host;
	}
};
parsed URLparse(char* URL);
void getHeaderInfo(char* buf, char* headerInp);
