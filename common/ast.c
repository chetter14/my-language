#include "ast.h"
#include <antlr3.h>
#include "GeneratorLexer.h"
#include "GeneratorParser.h"


pANTLR3_BASE_TREE getAST(pANTLR3_INPUT_STREAM input, char* errorMessage)
{
	pGeneratorLexer lexer = GeneratorLexerNew(input);
	if (!lexer)
	{
		errorMessage = "Failed to make a lexer!";
		return NULL;
	}

	pANTLR3_COMMON_TOKEN_STREAM tokenStream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
	if (!tokenStream)
	{
		errorMessage = "Failed to make a token stream!";
		return NULL;
	}

	pGeneratorParser parser = GeneratorParserNew(tokenStream);
	if (!parser)
	{
		errorMessage = "Failed to make a parser!";
		return NULL;
	}

	GeneratorParser_source_return result = parser->source(parser);
	if (parser->pParser->rec->state->errorCount > 0)
	{
		errorMessage = "Failed to parse the file!";
		return NULL;
	}

	pANTLR3_BASE_TREE tree = result.tree;

	parser->free(parser);
	tokenStream->free(tokenStream);
	lexer->free(lexer);

	return tree;
}


