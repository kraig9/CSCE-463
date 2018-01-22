#include "common.h"
#include "URLparser.h"
#include <process.h>

int main(int argc, char** argv) {
	cout << argc << endl;
	if (argc < 2) {
		cout << "Error: not enough arguments provided. Terminating..." << endl;
		return 1;
	}
	else if (argc > 2) {
		cout << "Error: too many arguments provided. Terminating..." << endl;
		return 1;
	}
	cout << argv[1] << endl;
	URL url(argv[1]);
	parsed parsedURL = url.URLparse(url.getURL());
	cout << "parsed data is: " << endl
		<< "query: " << parsedURL.query << endl
		<< "path: " << parsedURL.path << endl
		<< "port: " << parsedURL.port << endl
		<< "host: " << parsedURL.host << endl;
	system("pause");
	return 0;
}