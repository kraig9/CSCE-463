//Kraig Orcutt, CSCE 463-500 Spring 2018

#include "HTMLParserBase.h"
#include "common.h"
#include "Socket.h"
#include "URLparser.h"

int numTAMU = 0;

int createParser(char* htmlCode, int codeSize, char* URLIn) {
	// create new parser object
	HTMLParserBase *parser = new HTMLParserBase;

	char baseUrl[256];	// where this page came from; needed for construction of relative links
	strcpy_s(baseUrl, URLIn);

	int nLinks;
	char *linkBuffer = parser->Parse(htmlCode, codeSize, baseUrl, (int)strlen(baseUrl), &nLinks);

	// check for errors indicated by negative values
	if (nLinks < 0)	nLinks = 0;


	return nLinks;
	delete parser;
}
int HTMLParse(HTMLParserBase* parser, char* htmlCode, int codeSize, string URLIn) {
	char baseUrl[256];	// where this page came from; needed for construction of relative links
	strcpy_s(baseUrl, URLIn.c_str());

	int nLinks;
	char *linkBuffer = parser->Parse(htmlCode, codeSize, baseUrl, (int)strlen(baseUrl), &nLinks);
	//if (nLinks > 0) {
	//	for (int i = 0; i < nLinks; i++) {
	//		parsed parseBuf = URLparse(linkBuffer);
	//		char bigassbuffer[100000];
	//		memcpy(bigassbuffer, URLIn.c_str(), URLIn.size());
	//		bigassbuffer[URLIn.size() + 1] = 0;
	//		parsed parsebuf2 = URLparse(bigassbuffer);
	//		if (strcmp(parseBuf.host.c_str() + parseBuf.host.size() - 8, "tamu.edu")==0) {
	//			if (strcmp(parsebuf2.host.c_str() + parsebuf2.host.size() - 8, "tamu.edu") == 0) {
	//				EnterCriticalSection(inp);
	//				numTAMU++;
	//				printf("%d\n", numTAMU);
	//				LeaveCriticalSection(inp);
	//			}
	//		}
	//		linkBuffer + strlen(linkBuffer) + 1;
	//	}

	//}
	// check for errors indicated by negative values
	if (nLinks < 0)	nLinks = 0;

	return nLinks;
}