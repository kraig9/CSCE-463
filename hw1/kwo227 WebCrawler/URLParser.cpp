#include "URLparser.h"
// scheme://[user:pass@]host[:port][/path][?query][#fragment] 

parsed URL::URLparse(char* URL) {
	//get scheme
	printf("\tParsing URL... ");
	char* schemeCheck = strstr(URL, "http");
	if (schemeCheck == NULL) throw(1);
	char* schemePart = strstr(URL, "://");
	char scheme[128];
	//http://www.cplusplus.com/reference/cstring/strncpy/
	strncpy_s(scheme, URL, schemePart - URL);
	URL = schemePart + 3;
	
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
	}
	//3) Find / , extract path, truncate 
	char path[1024];
	char* pathPtr = path;
	memset(path, 0, 1024);
	path[0] = '/';
	path[1] = '\0';
	char* pathPart = strchr(URL, '/');
	if (pathPart != NULL) {
		pathPtr = pathPart + 1;
		*pathPart = '\0';
	}
	//4) Find :, extract port, truncate, obtain host
	char port[128];
	strcpy_s(port, "80");
	char host[10000];
	char* portPart = strchr(URL, ':');
	if (portPart != NULL) {
		strcpy_s(port, portPart + 1);
		//https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
		char* portErr;
		long converted = strtol(port, &portErr, 10);
		if (converted==NULL||converted==0) {
			// conversion failed because the input wasn't a number
			throw(2);
		}
		strncpy_s(host, URL, portPart - URL);
		*portPart = '\0';
	}
	else if (portPart == NULL) {
		strncpy_s(host, URL, strlen(URL));
	}
	parsed parsedURL(query, path, port, host);
	if(query!=NULL) strcat_s(path, query);
	printf("host %s, port %s, request %s \n", host, port, path);
	return parsedURL;
}

//getter
char* URL::getURL() {
	return url;
}

char* getHeaderInfo(char* buf, char* body) {
	char headerInfo[10000];
	char* headerPart = strstr(buf, "\r\n\r\n");
	strncpy_s(headerInfo, buf, headerPart - buf);
	char* headerErr = strstr(headerInfo, "http");
	if (headerErr == NULL) throw(4);
	memcpy_s(body, 500000, headerPart, strlen(headerPart));
	body[strlen(headerPart) - 1] = '\0';

	return headerInfo;
}
