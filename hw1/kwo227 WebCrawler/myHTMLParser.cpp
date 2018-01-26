//Kraig Orcutt, CSCE 463-500 Spring 2018

#include "HTMLParserBase.h"
#include "common.h"
#include "Socket.h"

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