#include "cfg.h"
#include <antlr3.h>
#include <stdlib.h>
#include <string.h>

static int cfgNodeCounter = 1;
static int opTreeNodeCounter = 1;

char* getDescByNodeType(CfgNodeType type)
{
	switch (type) {
	case NODE_TYPE_IF_STAT: return "IF-ELSE";
	case NODE_TYPE_WHILE_STAT: return "WHILE";
	case NODE_TYPE_REPEAT_STAT: return "REPEAT UNTIL";
	case NODE_TYPE_EXPR_STAT: return "EXPR";
	case NODE_TYPE_BREAK_STAT: return "BREAK";
	case NODE_TYPE_START: return "START";
	case NODE_TYPE_END: return "END";
	case NODE_TYPE_EMPTY: return "empty";
	}
}

char* getDescByOpTreeNodeType(OpTreeNodeType type)
{
	switch (type) {
	case OP_TREE_NODE_TYPE_WRITE: return "WRITE";
	case OP_TREE_NODE_TYPE_READ: return "READ";
	case OP_TREE_NODE_TYPE_ADDITION: return "ADD";
	case OP_TREE_NODE_TYPE_SUBTRACTION: return "SUB";
	case OP_TREE_NODE_TYPE_MULTIPLICATION: return "MUL";
	case OP_TREE_NODE_TYPE_DIVISION: return "DIV";
	case OP_TREE_NODE_TYPE_REMAINDER: return "REM";
	case OP_TREE_NODE_TYPE_LOGICAL_OR: return "LOGICAL OR";
	case OP_TREE_NODE_TYPE_LOGICAL_AND: return "LOGICAL AND";
	case OP_TREE_NODE_TYPE_EQUAL: return "EQUAL";
	case OP_TREE_NODE_TYPE_NOT_EQUAL: return "NOT EQUAL";
	case OP_TREE_NODE_TYPE_LESS: return "LESS";
	case OP_TREE_NODE_TYPE_GREATER: return "GREATER";
	case OP_TREE_NODE_TYPE_LESS_EQUAL: return "LESS OR EQUAL";
	case OP_TREE_NODE_TYPE_GREATER_EQUAL: return "GREATER OR EQUAL";
	case OP_TREE_NODE_TYPE_BIT_XOR: return "BITWISE XOR";
	case OP_TREE_NODE_TYPE_BIT_OR: return "BITWISE OR";
	case OP_TREE_NODE_TYPE_BIT_AND: return "BITWISE AND";
	case OP_TREE_NODE_TYPE_UNARY_MINUS: return "UNARY MINUS";
	case OP_TREE_NODE_TYPE_UNARY_LOGICAL_NOT: return "LOGICAL NOT";
	case OP_TREE_NODE_TYPE_UNARY_BIT_NOT: return "BITWISE NOT";
	case OP_TREE_NODE_TYPE_VALUE_PLACE: return "VALUE PLACE";
	case OP_TREE_NODE_TYPE_VALUE_INT: return "INT";
	case OP_TREE_NODE_TYPE_VALUE_FLOAT: return "FLOAT";
	case OP_TREE_NODE_TYPE_VALUE_STRING: return "STRING";
	}
}

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

	opTreeNode->id = opTreeNodeCounter;
	++opTreeNodeCounter;

	const char *elemText = (const char*)exprElemNode->getText(exprElemNode)->chars;
	if (strcmp(elemText, "==") == 0)			/* Equal */
		opTreeNode->type = OP_TREE_NODE_TYPE_EQUAL;
	else if (elemText[0] == '=')						/* Assignment */
		opTreeNode->type = OP_TREE_NODE_TYPE_WRITE;
	else if (elemText[0] == '+')						/* Addition */
		opTreeNode->type = OP_TREE_NODE_TYPE_ADDITION;
	else if (elemText[0] == '-') {
		if (exprElemNode->getChildCount(exprElemNode) == 2) /* Subtraction */
			opTreeNode->type = OP_TREE_NODE_TYPE_SUBTRACTION;
		else												/* Unary Minus */
			opTreeNode->type = OP_TREE_NODE_TYPE_UNARY_MINUS;
	}
	else if (elemText[0] == '*')					/* Multiplication */
		opTreeNode->type = OP_TREE_NODE_TYPE_MULTIPLICATION;
	else if (elemText[0] == '/')					/* Division */
		opTreeNode->type = OP_TREE_NODE_TYPE_DIVISION;
	else if (elemText[0] == '%')					/* Remainder */
		opTreeNode->type = OP_TREE_NODE_TYPE_REMAINDER;
	else if (strcmp(elemText, "&&") == 0)	/* Logical AND */
		opTreeNode->type = OP_TREE_NODE_TYPE_LOGICAL_AND;
	else if (strcmp(elemText, "||") == 0)	/* Logical OR */
		opTreeNode->type = OP_TREE_NODE_TYPE_LOGICAL_OR;
	else if (strcmp(elemText, "!=") == 0)	/* Not equal */
		opTreeNode->type = OP_TREE_NODE_TYPE_NOT_EQUAL;
	else if (strcmp(elemText, "<=") == 0)	/* Less or Equal */
		opTreeNode->type = OP_TREE_NODE_TYPE_LESS_EQUAL;
	else if (strcmp(elemText, ">=") == 0)	/* Greater or Equal */
		opTreeNode->type = OP_TREE_NODE_TYPE_GREATER_EQUAL;
	else if (elemText[0] == '<')						/* Less */
		opTreeNode->type = OP_TREE_NODE_TYPE_LESS;
	else if (elemText[0] == '>')						/* Greater */
		opTreeNode->type = OP_TREE_NODE_TYPE_GREATER;
	else if (elemText[0] == '|')						/* Bitwise OR */
		opTreeNode->type = OP_TREE_NODE_TYPE_BIT_OR;
	else if (elemText[0] == '^')						/* Bitwise XOR */
		opTreeNode->type = OP_TREE_NODE_TYPE_BIT_XOR;
	else if (elemText[0] == '&')						/* Bitwise AND */
		opTreeNode->type = OP_TREE_NODE_TYPE_BIT_AND;
	else if (elemText[0] == '!')						/* Logical NOT */
		opTreeNode->type = OP_TREE_NODE_TYPE_UNARY_LOGICAL_NOT;
	else if (elemText[0] == '~')						/* Bitwise NOT */
		opTreeNode->type = OP_TREE_NODE_TYPE_UNARY_BIT_NOT;

	switch (opTreeNode->type) {
		case OP_TREE_NODE_TYPE_WRITE:
		case OP_TREE_NODE_TYPE_ADDITION:
		case OP_TREE_NODE_TYPE_SUBTRACTION:
		case OP_TREE_NODE_TYPE_MULTIPLICATION:
		case OP_TREE_NODE_TYPE_DIVISION:
		case OP_TREE_NODE_TYPE_REMAINDER:
		case OP_TREE_NODE_TYPE_LOGICAL_AND:
		case OP_TREE_NODE_TYPE_LOGICAL_OR:
		case OP_TREE_NODE_TYPE_EQUAL:
		case OP_TREE_NODE_TYPE_NOT_EQUAL:
		case OP_TREE_NODE_TYPE_LESS:
		case OP_TREE_NODE_TYPE_GREATER:
		case OP_TREE_NODE_TYPE_LESS_EQUAL:
		case OP_TREE_NODE_TYPE_GREATER_EQUAL:
		case OP_TREE_NODE_TYPE_BIT_OR:
		case OP_TREE_NODE_TYPE_BIT_XOR:
		case OP_TREE_NODE_TYPE_BIT_AND:
		{
			opTreeNode->numberOfNext = 2;
			
			OpTreeNode* operand1 = (OpTreeNode*)malloc(sizeof(OpTreeNode));
			opTreeNode->next[0] = operand1;
			parseOperationTree(operand1, exprElemNode->getChild(exprElemNode, 0));

			OpTreeNode* operand2 = (OpTreeNode*)malloc(sizeof(OpTreeNode));
			opTreeNode->next[1] = operand2;
			parseOperationTree(operand2, exprElemNode->getChild(exprElemNode, 1));
			break;
		}
		case OP_TREE_NODE_TYPE_UNARY_MINUS:
		case OP_TREE_NODE_TYPE_UNARY_LOGICAL_NOT:
		case OP_TREE_NODE_TYPE_UNARY_BIT_NOT:
		{
			opTreeNode->numberOfNext = 1;

			OpTreeNode* operand = (OpTreeNode*)malloc(sizeof(OpTreeNode));
			opTreeNode->next[0] = operand;
			parseOperationTree(operand, exprElemNode->getChild(exprElemNode, 0));
			break;
		}
		default: {															/* Handle not operations node */
			if (atoi(elemText) != 0) {			/* It's integer */
				opTreeNode->type = OP_TREE_NODE_TYPE_VALUE_INT;
				opTreeNode->data.number = atoi(elemText);
			}
			else if (atof(elemText) != 0.0) {	/* It's float */
				opTreeNode->type = OP_TREE_NODE_TYPE_VALUE_FLOAT;
				opTreeNode->data.real = atof(elemText);
			}
			else if (elemText[0] == "\"") {			/* It's string (starts with " symbol) */
				opTreeNode->type = OP_TREE_NODE_TYPE_VALUE_STRING;
				opTreeNode->data.string = elemText;
			}
			else {									/* Identifier */
				opTreeNode->type = OP_TREE_NODE_TYPE_VALUE_PLACE;
				opTreeNode->data.identifier = elemText;
			}
			opTreeNode->numberOfNext = 0;
		}
	}
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
	
	curNode->next[0] = exprCfgNode;
	curNode->numberOfNext = 1;
	curNode->id = cfgNodeCounter;

	++cfgNodeCounter;
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
				curNode = parseExpressionStatement(curNode, baseStatementNode);
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

	curNode->next[0] = endNode;
	curNode->numberOfNext = 1;
	curNode->id = cfgNodeCounter++;

	endNode->id = cfgNodeCounter;

	cfgSubroutine->cfgStart = startNode;
	return cfgSubroutine;
}

char* getOpTreeNodeDesc(OpTreeNode *node)
{
	const int opTreeNodeMaxDigits = 10;

	char* nodeType = getDescByOpTreeNodeType(node->type);

	char value[30];
	if (node->type == OP_TREE_NODE_TYPE_VALUE_INT) {
		snprintf(value, 30, "%d", node->data.number);
	} else if (node->type == OP_TREE_NODE_TYPE_VALUE_FLOAT) {
		snprintf(value, 30, "%f", node->data.real);
	} else if (node->type == OP_TREE_NODE_TYPE_VALUE_STRING) {
		snprintf(value, 30, "%s", node->data.string);
	} else if (node->type == OP_TREE_NODE_TYPE_VALUE_PLACE) {
		snprintf(value, 30, "%s", node->data.identifier);
	} else {
		value[0] = '\0';
	}

	char* desc = (char*)malloc(strlen(value) + strlen(nodeType) + 2 + opTreeNodeMaxDigits);
	if (!desc) {
		printf("Failed to allocate memory for description string of cfg node\n");
		return NULL;
	}
	desc[0] = '\0';

	strcat(desc, nodeType);
	strcat(desc, " ");
	if (value[0] != '\0') {		/* If there is some value */
		strcat(desc, value);
	}

	char opTreeNodeCounterStr[10];
	snprintf(opTreeNodeCounterStr, 10, "%d", node->id);
	strcat(desc, " ");
	strcat(desc, opTreeNodeCounterStr);
	
	return desc;
}

void printOpTree(FILE *opTreeFile, OpTreeNode *opTree)
{
	char *curDesc = getOpTreeNodeDesc(opTree);

	for (int i = 0; i < opTree->numberOfNext; ++i) {
		OpTreeNode *next = opTree->next[i];
		char *nextDesc = getOpTreeNodeDesc(next);
		fprintf(opTreeFile, "\t\"%s\" -> \"%s\";\n", curDesc, nextDesc);
		free(nextDesc);

		printOpTree(opTreeFile, next);
	}
	free(curDesc);
}

char* getCfgNodeDesc(CfgNode* node)
{
	const int cfgNodeMaxDigits = 10;

	char opTreeAddress[20];
	snprintf(opTreeAddress, 20, "%p", node->opTree);

	char* nodeType = getDescByNodeType(node->type);

	char* desc = (char*)malloc(strlen(opTreeAddress) + strlen(nodeType) + 2 + cfgNodeMaxDigits);
	if (!desc) {
		printf("Failed to allocate memory for description string of cfg node\n");
		return NULL;
	}
	desc[0] = '\0';

	strcat(desc, nodeType);
	strcat(desc, " ");
	if (node->type != NODE_TYPE_BREAK_STAT && node->type != NODE_TYPE_EMPTY
		&& node->type != NODE_TYPE_START && node->type != NODE_TYPE_END) {
		strcat(desc, opTreeAddress);
	}

	char cfgNodeCounterStr[10];
	snprintf(cfgNodeCounterStr, 10, "%d", node->id);
	strcat(desc, " ");
	strcat(desc, cfgNodeCounterStr);

	return desc;
}

void printCfgNode(FILE *cfgFile, CfgNode* curNode)
{
	if (curNode->type == NODE_TYPE_END)
		return;		/* Reached the end */

	if (curNode->type == NODE_TYPE_EMPTY || curNode->type == NODE_TYPE_BREAK_STAT
		|| curNode->type == NODE_TYPE_START) {
		;		/* No operation tree for such nodes */
	} else {
		/* Create op tree file with the name of address of op tree for the cur node */
		char opTreeAddress[25];
		snprintf(opTreeAddress, 25, "%p.dot", curNode->opTree);
		FILE* opTreeFile = fopen(opTreeAddress, "w");
		if (!opTreeFile) {
			printf("Failed to open %s.dot file\n", opTreeAddress);
			return NULL;
		}
		fprintf(opTreeFile, "digraph OpTree {\n");

		printOpTree(opTreeFile, curNode->opTree);

		fprintf(opTreeFile, "}\n");
		fclose(opTreeFile);
	}

	char* curDesc = getCfgNodeDesc(curNode);

	for (int i = 0; i < curNode->numberOfNext; ++i) {
		CfgNode* next = curNode->next[i];
		char* nextDesc = getCfgNodeDesc(next);
		fprintf(cfgFile, "\t\"%s\" -> \"%s\";\n", curDesc, nextDesc);
		free(nextDesc);

		printCfgNode(cfgFile, next);
	}
	free(curDesc);
}

void printCfgSubroutine(CfgSubroutine* curSubroutine)
{
	FILE* cfgFile = fopen("cfg.dot", "w");
	if (!cfgFile) {
		printf("Failed to open cfg.dot file\n");
		return NULL;
	}

	fprintf(cfgFile, "digraph Cfg {\n");

	printCfgNode(cfgFile, curSubroutine->cfgStart);

	fprintf(cfgFile, "}\n");
	fclose(cfgFile);
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

	printCfgSubroutine(cfgFile.headSubroutine);

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

