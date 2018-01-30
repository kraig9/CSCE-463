//Kraig Orcutt, CSCE 463-500 Spring 2018

#include "common.h"
#include "URLparser.h"
#include <process.h>
#include "Socket.h"
#include "uniqueness.h"

//part 2 accept 2 agrs as well
//Opened URL-input.txt with size [SIZE]
int main(int argc, char** argv) {
	try {
		if (argc < 2) {
			cout << "Error: not enough arguments provided." << endl;
			return 1;
		}
		else if (argc > 3) {
			cout << "Error: too many arguments provided." << endl;
			return 1;
		}
		if (argc == 2) {
			URL url;
			memcpy_s(url.url, strlen(argv[1]), argv[1], strlen(argv[1]));
			url.url[strlen(argv[1])] = '\0';
			printf("URL: %s \n", argv[1]);
			parsed parsedURL = url.URLparse(url.url);
			winsock_test(parsedURL);
		}
		else if (argc == 3) {
			if (atoi(argv[1]) != 1) throw(11);
			int numThreads = atoi(argv[1]);
			//http://www.cplusplus.com/doc/tutorial/files/
			char URLbuf[4096];
			ifstream inputFile(argv[2],ios::ate);
			if (inputFile.good() != 1) throw(12);
			cout << "Opened " + string(argv[2]) + " with size " + to_string((inputFile.tellg()));
			//https://stackoverflow.com/questions/5343173/returning-to-beginning-of-file-after-getline
			inputFile.clear();
			inputFile.seekg(0, ios::beg);
			vector<parsed> parsedURLS;
			Uniqueness uniqueStruct;
			while (inputFile.getline(URLbuf, 4096)) {
				try {
					URL url;
					memcpy_s(url.url, strlen(URLbuf), URLbuf, strlen(URLbuf));
					url.url[strlen(URLbuf)] = '\0';
					printf("\nURL: %s", url.url);
					parsed parsedURL = url.URLparse(url.url);
					parsedURLS.push_back(parsedURL);
					winsock_test2(parsedURL, uniqueStruct);
				}
				catch (int f) {
					if (f == 1) printf("failed with invalid scheme");
					else if (f == 2) printf("failed with invalid port");
					else if (f == 3) printf("failed with %d", WSAGetLastError());
					else if (f == 4) printf("failed with non-HTTP header");
					else if (f == 5) printf("failed with exceeding max");
					//9 is for getting a status code other than 2xx
					else if (f == 9) return 0;
					else if (f == 10) printf("a timeout occurred");
					else if (f == 11) printf("Program only supports 1 thread. Input args are [threads][file to input]");
					else if (f == 12) printf("failed");
					//this is for the case that the robots header isn't status code 4xx
					else if (f == 13);
				}
			}
			inputFile.close();
		}
	}
	catch (int e) {
		if (e == 1) printf("failed with invalid scheme");
		else if (e == 2) printf("failed with invalid port");
		else if (e == 3) printf("failed with %d", WSAGetLastError());
		else if (e == 4) printf("failed with non-HTTP header");
		else if (e == 5) printf("failed with exceeding max");
		//9 is for getting a status code other than 2xx
		else if (e == 9) return 0;
		else if (e == 10) printf(" a timeout occurred");
		else if (e == 11) printf("Program only supports 1 thread. Input args are [threads][file to input]");
		else if (e == 12) printf("The input file couldn't be opened. Check the filename passed in.");
		return 1;
	}
}