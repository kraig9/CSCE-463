//Kraig Orcutt, CSCE 463-500 Spring 2018

#include "URLparser.h"
// scheme://[user:pass@]host[:port][/path][?query][#fragment] 

parsed URLparse(char* URL) {
	try {
		//get scheme
		char* schemeCheck = strstr(URL, "http");
		char* schemeCheck2 = strstr(URL, "https");
		if (schemeCheck == NULL || schemeCheck2 != NULL) throw(1);
		char* schemePart = strstr(URL, "://");
		char scheme[10];
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
		char pathPtr[MAX_PATH_LEN];
		memset(pathPtr, 0, MAX_PATH_LEN);
		pathPtr[0] = '/';
		pathPtr[1] = '\0';
		char* pathPart = strchr(URL, '/');
		if (pathPart != NULL) {
			strcpy_s(pathPtr, pathPart);
			*pathPart = '\0';
		}
		//4) Find :, extract port, truncate, obtain host
		char port[4];
		strcpy_s(port, "80");
		char host[MAX_HOST_LEN];
		char* portPart = strchr(URL, ':');
		if (portPart != NULL) {
			strcpy_s(port, portPart + 1);
			//https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
			char* portErr;
			long converted = strtol(port, &portErr, 10);
			if (converted == NULL || converted == 0) {
				// conversion failed because the input wasn't a number
				throw(2);
			}
			strncpy_s(host, URL, portPart - URL);
			*portPart = '\0';
		}
		else if (portPart == NULL) {
			if (strlen(URL) > MAX_HOST_LEN) throw 1;
			strncpy_s(host, URL, strlen(URL));
			host[strlen(URL)] = '\0';
		}
		parsed parsedURL(query, pathPtr, port, host);
		return parsedURL;
	}
	catch(int f){
		if (f == 1);
		else if (f == 2);
	}
}

void getHeaderInfo(char* buf, char* headerInp) {
	char headerInfo[10000];
	char* headerPart = strstr(buf, "\r\n\r\n");
	strncpy_s(headerInfo, buf, headerPart - buf);
	char* headerErr = strstr(headerInfo, "HTTP");
	//http://www.cplusplus.com/forum/beginner/68260/
	int headerErr2 = buf[9]-48;
	if (headerErr == NULL || headerPart == NULL) throw(4);
	if (headerErr2 == 2) {
		//strncpy_s(buf, strlen(headerPart+1), _TRUNCATE);
		size_t headpartlen = strlen(headerPart + 1);
		size_t buflen = strlen(buf);
		strcpy_s(buf,strlen(buf),headerPart+1);
	}
	memcpy_s(headerInp, strlen(headerInfo), headerInfo, strlen(headerInfo));
	headerInp[strlen(headerInfo)] = '\0';
}
