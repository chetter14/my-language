#include "cfg.h"
#include <antlr3.h>
#include <stdlib.h>
#include <string.h>

void fillArrayType(ArrayType* arrayType, pANTLR3_BASE_TREE typeNode)
{
	/* Get the type of elements in the array */
	pANTLR3_BASE_TREE arrayTypeNode = typeNode->getChild(typeNode, 0);
	const char *arrayTypeString = (const char*)arrayTypeNode->getText(arrayTypeNode)->chars;

	if (strcmp("bool", arrayTypeString) == 0) {			/* BOOL */
		arrayType->tag = ARRAY_TYPE_BUILT_IN_TYPE;
		arrayType->builtInType = BUILT_IN_TYPE_BOOL;
	} else if (strcmp("byte", arrayTypeString) == 0) {	/* BYTE */
		arrayType->tag = ARRAY_TYPE_BUILT_IN_TYPE;
		arrayType->builtInType = ARRAY_TYPE_BUILT_IN_TYPE;
	}
	else if (strcmp("int", arrayTypeString) == 0) {		/* INT */
		arrayType->tag = ARRAY_TYPE_BUILT_IN_TYPE;
		arrayType->builtInType = BUILT_IN_TYPE_INT;
	}
	else if (strcmp("uint", arrayTypeString) == 0) {		/* UINT */
		arrayType->tag = ARRAY_TYPE_BUILT_IN_TYPE;
		arrayType->builtInType = BUILT_IN_TYPE_UINT;
	}
	else if (strcmp("long", arrayTypeString) == 0) {		/* LONG */
		arrayType->tag = ARRAY_TYPE_BUILT_IN_TYPE;
		arrayType->builtInType = BUILT_IN_TYPE_LONG;
	}
	else if (strcmp("ulong", arrayTypeString) == 0) {	/* ULONG */
		arrayType->tag = ARRAY_TYPE_BUILT_IN_TYPE;
		arrayType->builtInType = BUILT_IN_TYPE_ULONG;
	}
	else if (strcmp("char", arrayTypeString) == 0) {		/* CHAR */
		arrayType->tag = ARRAY_TYPE_BUILT_IN_TYPE;
		arrayType->builtInType = BUILT_IN_TYPE_CHAR;
	}
	else if (strcmp("string", arrayTypeString) == 0) {	/* STRING */
		arrayType->tag = ARRAY_TYPE_BUILT_IN_TYPE;
		arrayType->builtInType = BUILT_IN_TYPE_STRING;
	}
	else {															/* identifier type */
		arrayType->tag = ARRAY_TYPE_IDENTIFIER;
		arrayType->identifier = arrayTypeString;
	}

	/* Determine the dimensions of an array */
	arrayType->numberOfDimensions = 0;

	pANTLR3_BASE_TREE arrayNode = typeNode->getChild(typeNode, 1);

	while (arrayNode) {
		pANTLR3_BASE_TREE curDimensionSizeNode = arrayNode->getChild(arrayNode, 0);
		int curDimSize = atoi((const char*)curDimensionSizeNode->getText(curDimensionSizeNode)->chars);
		arrayType->dimensions[arrayType->numberOfDimensions] = curDimSize;
		arrayType->numberOfDimensions++;

		if (arrayNode->getChildCount(arrayNode) == 1) {		/* No more dimensions left */
			arrayNode = NULL;
		} else {
			arrayNode = arrayNode->getChild(arrayNode, 1);
		}
	}
}

void fillTypeRef(TypeRef *typeRef, pANTLR3_BASE_TREE typeRefNode)
{
	/* 
	Do comparisons of node text with possible type references
	Depending on text - assign built-in, identifier, or array type
	*/
	pANTLR3_BASE_TREE typeNode = typeRefNode->getChild(typeRefNode, 0);
	const char *typeString = (const char*)typeNode->getText(typeNode)->chars;

	if (strcmp("ArrayType", typeString) == 0) {
		typeRef->tag = TYPE_REF_ARRAY_TYPE;
		fillArrayType(&typeRef->arrayType, typeNode);
	} else if (strcmp("bool", typeString) == 0) {		/* BOOL */
		typeRef->tag = TYPE_REF_BUILT_IN_TYPE;
		typeRef->builtInType = BUILT_IN_TYPE_BOOL;
	} else if (strcmp("byte", typeString) == 0) {		/* BYTE */
		typeRef->tag = TYPE_REF_BUILT_IN_TYPE;
		typeRef->builtInType = BUILT_IN_TYPE_BYTE;
	} else if (strcmp("int", typeString) == 0) {			/* INT */
		typeRef->tag = TYPE_REF_BUILT_IN_TYPE;
		typeRef->builtInType = BUILT_IN_TYPE_INT;
	} else if (strcmp("uint", typeString) == 0) {		/* UINT */
		typeRef->tag = TYPE_REF_BUILT_IN_TYPE;
		typeRef->builtInType = BUILT_IN_TYPE_UINT;
	} else if (strcmp("long", typeString) == 0) {		/* LONG */
		typeRef->tag = TYPE_REF_BUILT_IN_TYPE;
		typeRef->builtInType = BUILT_IN_TYPE_LONG;
	} else if (strcmp("ulong", typeString) == 0) {		/* ULONG */
		typeRef->tag = TYPE_REF_BUILT_IN_TYPE;
		typeRef->builtInType = BUILT_IN_TYPE_ULONG;
	} else if (strcmp("char", typeString) == 0) {		/* CHAR */
		typeRef->tag = TYPE_REF_BUILT_IN_TYPE;
		typeRef->builtInType = BUILT_IN_TYPE_CHAR;
	} else if (strcmp("string", typeString) == 0) {		/* STRING */
		typeRef->tag = TYPE_REF_BUILT_IN_TYPE;
		typeRef->builtInType = BUILT_IN_TYPE_STRING;
	} else {														/* identifier type */
		typeRef->tag = TYPE_REF_IDENTIFIER;
		typeRef->identifier = typeString;
	}
}

Parameter *getParameter(pANTLR3_BASE_TREE parameterNode)
{
	Parameter* parameter = (Parameter*)malloc(sizeof(Parameter));
	if (!parameter) {
		printf("Failed to allocate memory for parameter object\n");
		return NULL;
	}

	/* Parse an identifier of parameter */
	pANTLR3_BASE_TREE identifierNode = parameterNode->getChild(parameterNode, 0);
	parameter->identifier = (char*)identifierNode->getText(identifierNode)->chars;

	/* printf("Parameter node child count is %d\n", parameterNode->getChildCount(parameterNode)); */
	/* printf("Parameter node text is %s\n", parameterNode->getText(parameterNode)->chars); */

	/* Parse a type reference of parameter */
	if (parameterNode->getChildCount(parameterNode) == 2) {		/* Type is specified */
		fillTypeRef(&parameter->typeRef, parameterNode->getChild(parameterNode, 1));
	} else {
		parameter->typeRef.tag = TYPE_REF_NO_TYPE;
	}
	
	return parameter;
}

void fillParameterList(ParameterList* parameterList, pANTLR3_BASE_TREE paramListNode)
{
	/* printf("Parameter list node text is %s\n", paramListNode->getText(paramListNode)->chars); */

	Parameter *curParameter, *prevParameter = NULL;
	int numberOfParameters = paramListNode->getChildCount(paramListNode);
	for (int i = 0; i < numberOfParameters; ++i) {
		/* Parse each parameter */
		curParameter = getParameter(paramListNode->getChild(paramListNode, i));
		if (i == 0) {
			parameterList->headParameter = curParameter;
		} else {
			prevParameter->next = curParameter;
		}
		prevParameter = curParameter;
	}
	parameterList->numberOfParams = numberOfParameters;
}

void parseOperationTree(OpTreeNode* opTreeNode, pANTLR3_BASE_TREE exprElemNode)
{
	/* 
	Get the text of exprElemNode, assign type and data to opTreeNode
	If the type is incomplete (required to have children),
	recursively call parseOperationTree on child nodes
	*/
	
}



CfgNode *parseExpressionStatement(CfgNode* curNode, pANTLR3_BASE_TREE exprStatNode)
{
	pANTLR3_BASE_TREE exprElemNode = exprStatNode->getChild(exprStatNode, 0);
	CfgNode *exprCfgNode = (CfgNode*)malloc(sizeof(CfgNode));
	if (!exprCfgNode) {
		printf("Failed to allocate memory for expression cfg node object\n");
		return NULL;
	}
	exprCfgNode->type = NODE_TYPE_EXPR_STAT;

	OpTreeNode *opTree = (OpTreeNode*)malloc(sizeof(OpTreeNode));
	parseOperationTree(opTree, exprElemNode);

	exprCfgNode->opTree = opTree;
	return exprCfgNode;
}

CfgSubroutine *getCfgSubroutine(pANTLR3_BASE_TREE functionNode)
{
	CfgSubroutine* cfgSubroutine = (CfgSubroutine*)malloc(sizeof(CfgSubroutine));
	if (!cfgSubroutine) {
		printf("Failed to allocate memory for cfg subroutine object\n");
		return NULL;
	}

	/* Parse function signature: */ 
	pANTLR3_BASE_TREE funcSignature = functionNode->getChild(functionNode, 0);

	/* Name */
	pANTLR3_BASE_TREE funcNameNode = funcSignature->getChild(funcSignature, 0);
	cfgSubroutine->functionName = (char*)funcNameNode->getText(funcNameNode)->chars;

	/* Parameter list */
	if (funcSignature->getChildCount(funcSignature) >= 2) {
		pANTLR3_BASE_TREE parameterListNode = funcSignature->getChild(funcSignature, 1);
		fillParameterList(&cfgSubroutine->parameterList, parameterListNode);
	} else {		/* No parameters specified */
		cfgSubroutine->parameterList.numberOfParams = 0;
	}
	
	/* Return type */
	pANTLR3_BASE_TREE returnTypeNode = funcSignature->getChild(funcSignature, 2);

	if (funcSignature->getChildCount(funcSignature) == 3) {		/* Return type is specified */
		fillTypeRef(&cfgSubroutine->returnType, returnTypeNode);
	} else {
		cfgSubroutine->returnType.tag = TYPE_REF_NO_TYPE;
	}

	/* Build a CFG of the body of function */
	CfgNode* startNode = (CfgNode*)malloc(sizeof(CfgNode));
	if (!startNode) {
		printf("Failed to allocate memory for cfg node object\n");
		return NULL;
	}
	startNode->type = NODE_TYPE_START;

	CfgNode *curNode = startNode;

	int numberOfStatements = functionNode->getChildCount(functionNode) - 1;
	for (int i = 1; i < numberOfStatements + 1; ++i) {
		pANTLR3_BASE_TREE statementNode = functionNode->getChild(functionNode, i);
		if (statementNode->getChildCount(statementNode) == 2) {		/* repeat-until loop */
			;		/* to handle */
		} else {
			pANTLR3_BASE_TREE baseStatementNode = statementNode->getChild(statementNode, 0);
			const char* baseStatementString = (const char*)baseStatementNode->getText(baseStatementNode)->chars;
			if (strcmp("ExpressionStatement", baseStatementString) == 0) {
				parseExpressionStatement(curNode, baseStatementNode);
			}
			/* Add handling of other expressions */
		}
	}
	
	CfgNode *endNode = (CfgNode*)malloc(sizeof(CfgNode));
	if (!endNode) {
		printf("Failed to allocate memory for cfg node object\n");
		return NULL;
	}
	endNode->type = NODE_TYPE_END;

	cfgSubroutine->cfgStart = startNode;
	return cfgSubroutine;
}

CfgFile getCfg(char* sourceFile, pANTLR3_BASE_TREE sourceAST, char* errorMessage)
{
	CfgFile cfgFile;
	cfgFile.fileName = sourceFile;

	CfgSubroutine *curSubroutine, *prevSubroutine = NULL;
	int numberOfFunctions = sourceAST->getChildCount(sourceAST);
	for (int i = 0; i < numberOfFunctions; ++i) {
		curSubroutine = getCfgSubroutine(sourceAST->getChild(sourceAST, i));
		if (i == 0) {
			cfgFile.headSubroutine = curSubroutine;
		} else {
			prevSubroutine->next = curSubroutine;
		}
		prevSubroutine = curSubroutine;
	}

	return cfgFile;

	/* 
	- iterate over Source children
	- for each Function get into FuncSignature and parse
		function name, parameter list, and return type
	- for each Function get into its Statements
	- create blocks in cfg for statements: if, while, repeat, break, expression
	- nodes below these statements above put into operation tree
	*/

	/* 
	Algorithm:
	- Iterate over source files:
		(create cfg for each subroutine)
		1) Define subroutine structure: name, parameter list, return type
		2) Step through function statements (each cfg node, made of operations trees - expressions),
			create operation tree for each basic statement (while, if, repeat, expression)
		3) If any error occurs, keep it in "errorMessage"
	- Create call graph (functions that are being called from one another)
	- Return control flow graph for all files
	*/

}