#include "ast.h"
#include <antlr3.h>
#include <stdbool.h>
#include "MyLanguageLexer.h"
#include "MyLanguageParser.h"


static pMyLanguageLexer lexer;
static pANTLR3_COMMON_TOKEN_STREAM tokenStream;
static pMyLanguageParser parser;

// Rotate function calls and array accesses nodes to make the tree right-recursive
static void rotateFunctionCallsArrayAccesses(pANTLR3_BASE_TREE node)
{
	/*	Algorithm: (FC - function call, AA - array access)
	* 1) Use dfs to find FCs or AAs
	* 2) Check if the current node has one of its children a FC or AA 
	* 3) If it has, rotate the current node and its child (FC/AA)
	*	- set child instead of current node to the parent (node->getParent()->setChild(child))
	*	- delete the child from current node (node->deleteChild(child))
	*	- extract children of child, add current node to child, 
	*		and then add extracted children to child 
	*		(*copy children of child*, child->addChild(curNode),child->addChildren(*extracted children*))
	* 4) Otherwise, keep doing dfs from child (because current node is considered already)
	*/
}

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
		ASTcleanUpResources();
		return NULL;
	}

	parser = MyLanguageParserNew(tokenStream);
	if (!parser)
	{
		strcpy(errorMessage, "Failed to make a parser");
		ASTcleanUpResources();
		return NULL;
	}

	MyLanguageParser_source_return result = parser->source(parser);
	if (parser->pParser->rec->state->errorCount > 0)
	{
		// errorMessage = "Failed to parse the file";
		strcpy(errorMessage, "Failed to parse the file");
		ASTcleanUpResources();
		return NULL;
	}

	pANTLR3_BASE_TREE tree = result.tree;

	pANTLR3_STRING text = tree->getText(tree);
	const char* nodeText = (const char*)text->chars;
	printf("Root node text - %s\n", nodeText);

	rotateFunctionCallsArrayAccesses(tree);

	return tree;
}

bool makeDotFile(const char* outputFile, pANTLR3_BASE_TREE tree, char* errorMessage)
{
	FILE* dotFile = fopen(outputFile, "w");
	if (!dotFile)
	{
		errorMessage = "Failed to open the output file";
		ASTcleanUpResources();
		return false;
	}

	pANTLR3_STRING dotString = parser->adaptor->makeDot(parser->adaptor, tree);

	fprintf(dotFile, "%s", (char*)dotString->chars);
	fclose(dotFile);

	return true;
}

void ASTcleanUpResources()
{
	if (parser)
		parser->free(parser);

	if (tokenStream)
		tokenStream->free(tokenStream);

	if (lexer)
		lexer->free(lexer);
}