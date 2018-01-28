//Kraig Orcutt, CSCE 463-500 Spring 2018

#include "URLparser.h"
// scheme://[user:pass@]host[:port][/path][?query][#fragment] 

parsed URL::URLparse(char* URL) {
	//get scheme
	printf("\tParsing URL... ");
	char* schemeCheck = strstr(URL, "http");
	char* schemeCheck2 = strstr(URL, "https");
	if (schemeCheck == NULL || schemeCheck2 != NULL) throw(1);
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
	char pathPtr[1024];
	memset(pathPtr, 0, 1024);
	pathPtr[0] = '/';
	pathPtr[1] = '\0';
	char* pathPart = strchr(URL, '/');
	if (pathPart != NULL) {
		strcpy_s(pathPtr, pathPart);
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
	parsed parsedURL(query, pathPtr, port, host);
	if (pathPtr[0] == '/' && query == NULL) printf("host %s, port %s, request %s \n", host, port, pathPtr);
	else if (pathPtr[0] == '/' && query != NULL)printf("host %s, port %s, request %s?%s \n", host, port, pathPtr, query);
	else printf("host %s, port %s, request /%s?%s \n", host, port, pathPtr, query);
	return parsedURL;
}

//getter
char* URL::getURL() {
	return url;
}

char* getHeaderInfo(char* buf) {
	char headerInfo[10000];
	char* headerPart = strstr(buf, "\r\n\r\n");
	strncpy_s(headerInfo, buf, headerPart - buf);
	char* headerErr = strstr(headerInfo, "HTTP");
	//http://www.cplusplus.com/forum/beginner/68260/
	int headerErr2 = buf[9]-48;
	if (headerErr == NULL || headerPart == NULL) throw(4);
	if (headerErr2 == 2) {
		//strncpy_s(buf, strlen(headerPart+1), _TRUNCATE);
		int headpartlen = strlen(headerPart + 1);
		int buflen = strlen(buf);
		strcpy_s(buf,strlen(buf),headerPart+1);
	}
	return headerInfo;
}
