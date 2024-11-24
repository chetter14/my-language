#include "ast.h"
#include <antlr3.h>
#include <stdbool.h>
#include "MyLanguageLexer.h"
#include "MyLanguageParser.h"


static pMyLanguageLexer lexer;
static pANTLR3_COMMON_TOKEN_STREAM tokenStream;
static pMyLanguageParser parser;

// Returns true if parameter string is equal to "FunctionCall" or "ArrayAccess"
static bool isEqualToFcOrAa(const char* str)
{
	return strcmp(str, "FunctionCall") == 0 || 
			strcmp(str, "ArrayAccess") == 0;
}

/* Rotate function calls and array accesses nodes to make the tree right - recursive.
*
*	Algorithm: (FC - function call, AA - array access)
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
static void rotateFunctionCallsArrayAccesses(pANTLR3_BASE_TREE node, int curNodeIndexInParent)
{
	const char* nodeText = (const char*)node->getText(node)->chars;
	pANTLR3_BASE_TREE child;

	for (int i = 0; i < node->getChildCount(node); ++i)	// iterate over children
	{
		child = (pANTLR3_BASE_TREE)node->getChild(node, i);
		const char* childText = (const char*)child->getText(child)->chars;

		// printf("Node text - %s\n", nodeText);
		// printf("Child text - %s\n", childText);

		// If current node is FunctionCall or ArrayAccess and child as well
		if (isEqualToFcOrAa(nodeText) && isEqualToFcOrAa(childText) && i != 0)	
		// And it's not the first child (because in this case they are already rotated)
		{										
			// Rotate left the current node and its child (FC/AA):
			// printf("Do rotation\n");

			// Set child instead of current node to the parent
			pANTLR3_BASE_TREE parent = node->getParent(node);
			parent->setChild(parent, curNodeIndexInParent, child);
			child->setParent(child, parent);

			// Delete the child from current node
			node->deleteChild(node, i);

			// Extract children of child
			int grandchildrenSize = child->getChildCount(child);
			pANTLR3_BASE_TREE* grandchild = malloc(sizeof(pANTLR3_BASE_TREE) * grandchildrenSize);

			// printf("Get grandchildren:\n");
			for (int i = 0; i < grandchildrenSize; ++i)
			{
				grandchild[i] = child->getChild(child, i);
				// printf("Grandchild name - %s\n", (const char*)grandchild[i]->getText(grandchild[i])->chars);
			}

			for (int i = grandchildrenSize - 1; i >= 0; --i)
			{
				child->deleteChild(child, i);
			}

			// Add current node to child
			child->addChild(child, node);
			node->setParent(node, child);

			// Add extracted children to child
			// printf("Add grandchildren:\n");
			for (int i = 0; i < grandchildrenSize; ++i)
			{
				child->addChild(child, grandchild[i]);
			//	printf("Grandchild name - %s\n", (const char*)grandchild[i]->getText(grandchild[i])->chars);
			}

			// printf("New grandchildren:\n");
			for (int i = 0; i < child->getChildCount(child); ++i)
			{
				pANTLR3_BASE_TREE grandchild = (pANTLR3_BASE_TREE)child->getChild(child, i);
			//	printf("Grandchild name - %s\n", (const char*)grandchild->getText(grandchild)->chars);
			}

			free(grandchild);

			// Do not dfs leftmost node from child now, because it is considered already
			rotateFunctionCallsArrayAccesses(child, curNodeIndexInParent);
		}
		else
		{
			// Keep dfs'ing for each child node
			rotateFunctionCallsArrayAccesses(child, i);
		}	
	}
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

	rotateFunctionCallsArrayAccesses(tree, 0);

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