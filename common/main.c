#include <stdio.h>
#include <string.h>
#include "ast.h"
#include <antlr3.h>

#define ERROR_MSG_SIZE 100


int main(int argc, char* argv[])
{
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
		printf("Error: %s! Termination!\n", errorMessage);
		return 1;
	}

	// make DOT file by AST tree:
	memset(&errorMessage[0], 0, ERROR_MSG_SIZE);	// reset message char string

	return 0;
}