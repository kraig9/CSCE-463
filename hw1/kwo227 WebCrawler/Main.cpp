#include "common.h"
#include "URLparser.h"
#include <process.h>
#include "Socket.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "Error: not enough arguments provided. Terminating..." << endl;
		return 1;
	}
	else if (argc > 2) {
		cout << "Error: too many arguments provided. Terminating..." << endl;
		return 1;
	}
	URL url(argv[1]);
	parsed parsedURL = url.URLparse(url.getURL());
	winsock_test(parsedURL);
}