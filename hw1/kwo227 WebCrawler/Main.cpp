//Kraig Orcutt, CSCE 463-500 Spring 2018

#include "common.h"
#include "URLparser.h"
#include <process.h>
#include "Socket.h"

int main(int argc, char** argv) {
	try {
		if (argc < 2) {
			cout << "Error: not enough arguments provided." << endl
				<< "Syntax is scheme://host[port][/path][?query][#fragment]" << endl
				<< "Params in [] are optional." << endl;
			return 1;
		}
		else if (argc > 2) {
			cout << "Error: too many arguments provided." << endl
				<< "Syntax is scheme://host[port][/path][?query][#fragment]" << endl
				<< "Params in [] are optional." << endl;
			return 1;
		}
		URL url(argv[1]);
		printf("URL: %s \n", argv[1]);
		parsed parsedURL = url.URLparse(url.getURL());
		winsock_test(parsedURL);
	}
	catch (int e) {
		if (e == 1) printf("failed with invalid scheme");
		else if (e == 2) printf("failed with invalid port");
		else if (e == 3) printf("failed with %d", WSAGetLastError());
		else if (e == 4) printf("failed with non-HTTP header");
		//9 is for getting a status code other than 2xx
		else if (e == 9) return 0;
		else if (e == 10);
		return 1;
	}
}