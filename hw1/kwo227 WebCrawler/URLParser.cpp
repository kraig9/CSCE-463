#include "URLparser.h"
// scheme://[user:pass@]host[:port][/path][?query][#fragment] 

parsed URL::URLparse(char* URL) {
	//get scheme
	char* schemePart = strstr(URL, "://");
	char scheme[128];
	//http://www.cplusplus.com/reference/cstring/strncpy/
	strncpy_s(scheme, URL, schemePart - URL);
	URL = schemePart + 3;
	
	cout << "schemePart URL is: " << URL << endl;
	cout << "extracted scheme is: " << scheme << endl;
	//TODO: if scheme is null either append one or throw error

	//1) Find # using strchr() and truncate 
	//www.cplusplus.com/reference/cstring/strchr/
	char* fragmentPart = strchr(URL, '#');
	if (fragmentPart != NULL) *fragmentPart = '\0';

	//2) Find ? , extract query, truncate 
	char* query = NULL;
	char* queryPart = strchr(URL, '?');
	if (queryPart != NULL) {
		query = queryPart + 1;
		*queryPart = '\0';
		cout << "extracted query is: " << query << endl;
	}
	else if(queryPart == NULL) cout << "extracted query is NULL " << endl;
	//3) Find / , extract path, truncate 
	char* path = NULL;
	char* pathPart = strchr(URL, '/');
	if (pathPart != NULL) {
		path = pathPart + 1;
		*pathPart = '\0';
		cout << "extracted path is: " << path << endl;
	}
	else if (pathPart == NULL) {
		*path = '/';
		cout << "extracted path is: " << path << endl;
	}
	//4) Find :, extract port, truncate, obtain host
	cout << "URL is: " << URL << endl;
	char* port = NULL;
	char host[10000];
	char* portPart = strchr(URL, ':');
	if (portPart != NULL) {
		port = portPart + 1;
		strncpy_s(host, URL, portPart - URL);
		*portPart = '\0';
		cout << "extracted port is: " << port << endl;
		cout << "extracted host is: " << host << endl;
	}
	else if (portPart == NULL) {
		strncpy_s(host, URL, strlen(URL));
		cout << "extracted port is NULL " << endl;
		cout << "extracted host is: " << host << endl;
	}
	parsed parsedURL(query, path, port, host);
	return parsedURL;
}

//getter
char* URL::getURL() {
	return url;
}