#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "cfg.h"
#include "callGraph.h"
#include <antlr3.h>

#define ERROR_MSG_SIZE 100


int main(int argc, char* argv[])
{
	/* Add handling of few input files: */

	pANTLR3_INPUT_STREAM input = antlr3FileStreamNew((pANTLR3_UINT8)argv[1], ANTLR3_ENC_8BIT);
	if (!input)
	{
		printf("Error: Failed to open the input file! Termination!\n");
		return 1;
	}

	char errorMessage[ERROR_MSG_SIZE];
	pANTLR3_BASE_TREE tree = getAST(input, errorMessage);
	if (!tree)
	{
		printf("Parsing error: %s! Termination!\n", errorMessage);
		return 1;
	}

	// reset message char string
	memset(&errorMessage[0], 0, ERROR_MSG_SIZE);	

	if (!makeDotFile(argv[2], tree, errorMessage))
	{
		printf("Making dot file error: %s! Termination!\n", errorMessage);
		return 1;
	}

	printf("Building of AST completed successfully!\n");

	/* reset message char string */
	memset(&errorMessage[0], 0, ERROR_MSG_SIZE);

	CfgFile cfg = getCfg(argv[1], tree, errorMessage);

	/* Clean up resources used for AST building */
	ASTcleanUpResources();

	/* Clean up a file stream used for printing out call graph */
	closeCallGraphFile();

	return 0;
}