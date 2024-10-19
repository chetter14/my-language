#ifndef MY_LANGUAGE_AST_H
#define MY_LANGUAGE_AST_H

#include <antlr3.h>
#include <stdbool.h>

pANTLR3_BASE_TREE getAST(pANTLR3_INPUT_STREAM input, char* errorMessage);
bool makeDotFile(const char* outputFile, pANTLR3_BASE_TREE tree, char* errorMessage);
void cleanUpResources();

#endif