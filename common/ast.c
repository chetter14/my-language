#include "ast.h"
#include <antlr3.h>
#include <stdbool.h>
#include "MyLanguageLexer.h"
#include "MyLanguageParser.h"


static pMyLanguageLexer lexer;
static pANTLR3_COMMON_TOKEN_STREAM tokenStream;
static pMyLanguageParser parser;

pANTLR3_BASE_TREE getAST(pANTLR3_INPUT_STREAM input, char* errorMessage)
{
	lexer = MyLanguageLexerNew(input);
	if (!lexer)
	{
		strcpy(errorMessage, "Failed to make a lexer");
		return NULL;
	}

	tokenStream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
	if (!tokenStream)
	{
		strcpy(errorMessage, "Failed to make a token stream");
		cleanUpResources();
		return NULL;
	}

	parser = MyLanguageParserNew(tokenStream);
	if (!parser)
	{
		strcpy(errorMessage, "Failed to make a parser");
		cleanUpResources();
		return NULL;
	}

	MyLanguageParser_source_return result = parser->source(parser);
	if (parser->pParser->rec->state->errorCount > 0)
	{
		// errorMessage = "Failed to parse the file";
		strcpy(errorMessage, "Failed to parse the file");
		cleanUpResources();
		return NULL;
	}

	pANTLR3_BASE_TREE tree = result.tree;

	return tree;
}

bool makeDotFile(const char* outputFile, pANTLR3_BASE_TREE tree, char* errorMessage)
{
	FILE* dotFile = fopen(outputFile, "w");
	if (!dotFile)
	{
		errorMessage = "Failed to open the output file";
		return false;
	}

	pANTLR3_STRING dotString = parser->adaptor->makeDot(parser->adaptor, tree);

	fprintf(dotFile, "%s", (char*)dotString->chars);
	fclose(dotFile);

	return true;
}

void cleanUpResources()
{
	if (parser)
		parser->free(parser);

	if (tokenStream)
		tokenStream->free(tokenStream);

	if (lexer)
		lexer->free(lexer);
}