#include "HTMLParserBase.h"
#include "common.h"

void createParser() {
	// create new parser object
	HTMLParserBase *parser = new HTMLParserBase;

	char baseUrl[] = "http://www.tamu.edu";		// where this page came from; needed for construction of relative links

	int nLinks;
	char *linkBuffer = parser->Parse(fileBuf, fileSize, baseUrl, (int)strlen(baseUrl), &nLinks);

	// check for errors indicated by negative values
	if (nLinks < 0)
		nLinks = 0;

	printf("Found %d links:\n", nLinks);
}