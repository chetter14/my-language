#include "cfg.h"
#include <antlr3.h>
#include <stdlib.h>
#include <string.h>

static int cfgNodeCounter = 1;
static int opTreeNodeCounter = 1;

void addChild(CfgNode* curNode, CfgNode* child)
{
	curNode->next[curNode->numberOfNext] = child;
	curNode->numberOfNext++;
}

void assignId(CfgNode* curNode)
{
	curNode->id = cfgNodeCounter++;
}

char* getDescByNodeType(CfgNodeType type)
{
	switch (type) {
	case NODE_TYPE_IF_STAT: return "IF-ELSE COND";
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
	case OP_TREE_NODE_TYPE_ARRAY_ACCESS: return "ARRAY ACCESS";
	case OP_TREE_NODE_TYPE_ARRAY_INDEX: return "ARRAY INDEX";
	case OP_TREE_NODE_TYPE_FUNCTION_CALL: return "FUNC CALL";
	case OP_TREE_NODE_TYPE_FUNC_CALL_ARGS: return "FUNC ARGS";
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
	case OP_TREE_NODE_TYPE_VALUE_BOOL: return "BOOL";
	case OP_TREE_NODE_TYPE_VALUE_CHAR: return "CHAR";
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
	Parameter* parameter = (Parameter*)calloc(1, sizeof(Parameter));
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
	if (strcmp(elemText, "ArrayAccess") == 0)	/* Array access */
		opTreeNode->type = OP_TREE_NODE_TYPE_ARRAY_ACCESS;
	else if (strcmp(elemText, "Index") == 0)		/* Index (or indices) to access array */
		opTreeNode->type = OP_TREE_NODE_TYPE_ARRAY_INDEX;
	else if (strcmp(elemText, "FunctionCall") == 0)	/* Function call */
		opTreeNode->type = OP_TREE_NODE_TYPE_FUNCTION_CALL;
	else if (strcmp(elemText, "Args") == 0)			/* Arguments for function call */
		opTreeNode->type = OP_TREE_NODE_TYPE_FUNC_CALL_ARGS;
	else if (strcmp(elemText, "==") == 0)			/* Equal */
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
	else
		opTreeNode->type = OP_TREE_NODE_TYPE_VALUE;

	switch (opTreeNode->type) {
		case OP_TREE_NODE_TYPE_ARRAY_ACCESS:
		{
			int childCount = exprElemNode->getChildCount(exprElemNode);

			opTreeNode->numberOfNext = childCount;
			opTreeNode->next = (OpTreeNode**)calloc(opTreeNode->numberOfNext, sizeof(OpTreeNode*));

			/* Value place that is our array */
			OpTreeNode* valuePlace = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
			opTreeNode->next[0] = valuePlace;
			parseOperationTree(valuePlace, exprElemNode->getChild(exprElemNode, 0));

			/* Array indices we want to access */
			for (int i = 1; i < childCount; ++i)
			{
				OpTreeNode* index = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
				opTreeNode->next[i] = index;
				parseOperationTree(index, exprElemNode->getChild(exprElemNode, i));
			}
			break;
		}
		case OP_TREE_NODE_TYPE_ARRAY_INDEX:
		{
			/* There can be 1 or 2 children (indices) */
			int childCount = exprElemNode->getChildCount(exprElemNode);
			
			opTreeNode->numberOfNext = childCount;
			opTreeNode->next = (OpTreeNode**)calloc(opTreeNode->numberOfNext, sizeof(OpTreeNode*));

			/* Indices that are expressions */
			for (int i = 0; i < childCount; ++i)
			{
				OpTreeNode* expr = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
				opTreeNode->next[i] = expr;
				parseOperationTree(expr, exprElemNode->getChild(exprElemNode, i));
			}
			break;
		}
		case OP_TREE_NODE_TYPE_FUNC_CALL_ARGS:
		{
			/* There can be 0 or more children (arguments) */
			int childCount = exprElemNode->getChildCount(exprElemNode);

			opTreeNode->numberOfNext = childCount;
			if (childCount != 0)
			{
				opTreeNode->next = (OpTreeNode**)calloc(opTreeNode->numberOfNext, sizeof(OpTreeNode*));

				for (int i = 0; i < childCount; ++i)
				{
					OpTreeNode* argument = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
					opTreeNode->next[i] = argument;
					parseOperationTree(argument, exprElemNode->getChild(exprElemNode, i));
				}
			}
			break;
		}
		case OP_TREE_NODE_TYPE_FUNCTION_CALL:
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
			
			/* Allocate memory for the array of next operation tree nodes */
			opTreeNode->next = (OpTreeNode**)calloc(opTreeNode->numberOfNext, sizeof(OpTreeNode*));

			OpTreeNode* operand1 = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
			opTreeNode->next[0] = operand1;
			parseOperationTree(operand1, exprElemNode->getChild(exprElemNode, 0));

			OpTreeNode* operand2 = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
			opTreeNode->next[1] = operand2;
			parseOperationTree(operand2, exprElemNode->getChild(exprElemNode, 1));
			break;
		}
		case OP_TREE_NODE_TYPE_UNARY_MINUS:
		case OP_TREE_NODE_TYPE_UNARY_LOGICAL_NOT:
		case OP_TREE_NODE_TYPE_UNARY_BIT_NOT:
		{
			opTreeNode->numberOfNext = 1;

			/* Allocate memory for the array of next operation tree nodes */
			opTreeNode->next = (OpTreeNode**)calloc(opTreeNode->numberOfNext, sizeof(OpTreeNode*));

			OpTreeNode* operand = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
			opTreeNode->next[0] = operand;
			parseOperationTree(operand, exprElemNode->getChild(exprElemNode, 0));
			break;
		}
		default: {															/* Handle not operations node */
			if (strcmp(elemText, "true") == 0 || strcmp(elemText, "false") == 0) {	/* It's BOOLEAN */
				opTreeNode->type = OP_TREE_NODE_TYPE_VALUE_BOOL;
				if (strcmp(elemText, "true") == 0)
					opTreeNode->data.number = 1;				/* Equivalent to true */
				else
					opTreeNode->data.number = 0;				/* Equivalent to false */
			}
			else if (elemText[0] == '\'') {								/* It's CHAR */
				opTreeNode->type = OP_TREE_NODE_TYPE_VALUE_CHAR;
				opTreeNode->data.number = elemText[1];		/* Made of just one character */
			}
			else if (elemText[0] == '\"') {								/* It's STRING */
				int charsLen = strlen(elemText) - 2;	/* Subtract the first and last double quotes */
				char* string = (char*)calloc(charsLen + 1, sizeof(char));

				strncpy(string, &elemText[1], charsLen);
				string[charsLen] = '\0';					/* Set the end of string */
				
				opTreeNode->type = OP_TREE_NODE_TYPE_VALUE_STRING;
				opTreeNode->data.string = string;
			}
			else if (atoi(elemText) != 0 || strcmp(elemText, "0") == 0) {	/* It's INTEGER */
				opTreeNode->type = OP_TREE_NODE_TYPE_VALUE_INT;
				opTreeNode->data.number = atoi(elemText);
			}
			else {														/* IDENTIFIER */
				opTreeNode->type = OP_TREE_NODE_TYPE_VALUE_PLACE;
				opTreeNode->data.identifier = elemText;
			}
			/* !!! Add handling of BIT and HEX literals !!! */
			opTreeNode->numberOfNext = 0;
		}
	}
}

CfgNode* parseStatements(CfgNode* curNode, pANTLR3_BASE_TREE baseNode, int startIndex, int numberOfStatements, CfgNode *outsideLoopNode);

CfgNode *parseExpressionStatement(CfgNode* curNode, pANTLR3_BASE_TREE exprStatNode)
{
	pANTLR3_BASE_TREE exprElemNode = exprStatNode->getChild(exprStatNode, 0);
	CfgNode *exprCfgNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!exprCfgNode) {
		printf("Failed to allocate memory for expression cfg node object\n");
		return NULL;
	}
	exprCfgNode->type = NODE_TYPE_EXPR_STAT;

	OpTreeNode *opTree = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
	parseOperationTree(opTree, exprElemNode);
	exprCfgNode->opTree = opTree;
	
	addChild(curNode, exprCfgNode);
	assignId(curNode);

	return exprCfgNode;
}

CfgNode* parseIfStatement(CfgNode* curNode, pANTLR3_BASE_TREE ifStatNode, CfgNode *outsideLoopNode)
{
	/* Parse condition node and its operation tree */
	pANTLR3_BASE_TREE conditionNode = ifStatNode->getChild(ifStatNode, 0);
	CfgNode* condCfgNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!condCfgNode) {
		printf("Failed to allocate memory for if statement cfg node object\n");
		return NULL;
	}
	condCfgNode->type = NODE_TYPE_IF_STAT;

	OpTreeNode* condOpTree = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
	parseOperationTree(condOpTree, conditionNode->getChild(conditionNode, 0));
	condCfgNode->opTree = condOpTree;

	addChild(curNode, condCfgNode);
	assignId(curNode);
	
	/* Parse "then" clause of if statement */
	pANTLR3_BASE_TREE thenNode = ifStatNode->getChild(ifStatNode, 1);
	CfgNode* thenCfgNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!thenCfgNode) {
		printf("Failed to allocate memory for \"then\" clause of if statement cfg node object \n");
		return NULL;
	}
	thenCfgNode->type = NODE_TYPE_EMPTY;

	addChild(condCfgNode, thenCfgNode);
	assignId(condCfgNode);

	CfgNode* lastThenCfgNode = parseStatements(thenCfgNode, thenNode, 0, thenNode->getChildCount(thenNode), outsideLoopNode);

	/* Create an empty node that is the end of "then" and "else" clauses */
	CfgNode* endIfNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!endIfNode) {
		printf("Failed to allocate memory for the cfg node object of the end of 'if statement'\n");
		return NULL;
	}
	endIfNode->type = NODE_TYPE_EMPTY;
	
	/* Parse "else" clause if it exists */
	CfgNode *lastElseCfgNode = NULL;
	if (ifStatNode->getChildCount(ifStatNode) == 3) {
		pANTLR3_BASE_TREE elseNode = ifStatNode->getChild(ifStatNode, 2);
		CfgNode* elseCfgNode = (CfgNode*)calloc(1, sizeof(CfgNode));
		if (!elseCfgNode) {
			printf("Failed to allocate memory for \"else\" clause of if statement cfg node object \n");
			return NULL;
		}
		elseCfgNode->type = NODE_TYPE_EMPTY;

		addChild(condCfgNode, elseCfgNode);

		lastElseCfgNode = parseStatements(elseCfgNode, elseNode, 0, elseNode->getChildCount(elseNode), outsideLoopNode);
	} else {				/* No "else block" -> link to the end node as well */
		addChild(condCfgNode, endIfNode);
	}

	addChild(lastThenCfgNode, endIfNode);
	assignId(lastThenCfgNode);

	if (lastElseCfgNode) {
		addChild(lastElseCfgNode, endIfNode);
		assignId(lastElseCfgNode);
	}

	return endIfNode;
}

CfgNode* parseWhileStatement(CfgNode* curNode, pANTLR3_BASE_TREE whileStatNode)
{
	/* Parse condition node and its operation tree */
	pANTLR3_BASE_TREE conditionNode = whileStatNode->getChild(whileStatNode, 0);
	CfgNode* condCfgNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!condCfgNode) {
		printf("Failed to allocate memory for while statement condition cfg node object\n");
		return NULL;
	}
	condCfgNode->type = NODE_TYPE_WHILE_STAT;

	OpTreeNode* condOpTree = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
	parseOperationTree(condOpTree, conditionNode->getChild(conditionNode, 0));
	condCfgNode->opTree = condOpTree;

	addChild(curNode, condCfgNode);
	assignId(curNode);

	/* Create an empty node that is the end of while statement */
	CfgNode* endWhileNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!endWhileNode) {
		printf("Failed to allocate memory for the cfg node object of the end of 'while statement'\n");
		return NULL;
	}
	endWhileNode->type = NODE_TYPE_EMPTY;

	/* Outside node of the while loop */
	CfgNode* outsideWhileNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!outsideWhileNode) {
		printf("Failed to allocate memory for the cfg node object of the outside node of 'while statement'\n");
		return NULL;
	}
	outsideWhileNode->type = NODE_TYPE_EMPTY;

	/* Parse statements to repeat */
	CfgNode* lastWhileBodyNode = NULL;
	if (whileStatNode->getChildCount(whileStatNode) == 2) {		/* If there is body for while loop */
		pANTLR3_BASE_TREE bodyNode = whileStatNode->getChild(whileStatNode, 1);
		lastWhileBodyNode = parseStatements(condCfgNode, bodyNode, 0, bodyNode->getChildCount(bodyNode), outsideWhileNode);
	}

	if (lastWhileBodyNode) {
		addChild(lastWhileBodyNode, endWhileNode);
		assignId(lastWhileBodyNode);
	} else {						/* No body for the while loop */
		addChild(condCfgNode, endWhileNode);
		assignId(condCfgNode);
	}

	/* Link the endWhileNode to the condition node */
	addChild(endWhileNode, condCfgNode);
	assignId(endWhileNode);

	/* Link condition node to the outside node */
	addChild(condCfgNode, outsideWhileNode);
	if (!lastWhileBodyNode) {
		assignId(condCfgNode);
	}

	return outsideWhileNode;
}

CfgNode* parseBlockStatement(CfgNode* curNode, pANTLR3_BASE_TREE blockStatNode, CfgNode *outsideLoopNode)
{
	return parseStatements(curNode, blockStatNode, 0, blockStatNode->getChildCount(blockStatNode), outsideLoopNode);
}

CfgNode* parseBreakStatement(CfgNode* curNode, pANTLR3_BASE_TREE breakStatNode, CfgNode *outsideLoopNode)
{
	/* Create break cfg node */
	CfgNode* breakCfgNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!breakCfgNode) {
		printf("Failed to allocate memory for break statement cfg node object\n");
		return NULL;
	}
	breakCfgNode->type = NODE_TYPE_BREAK_STAT;

	if (outsideLoopNode == NULL) {
		/* !!! Add extra handling !!! */
		printf("ERROR! Break statement is called outside of loop!\n");
		return NULL;
	}

	/* Link current cfg to the break node */
	addChild(curNode, breakCfgNode);
	assignId(curNode);

	/* Create a stub cfg node */
	CfgNode* stubNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!stubNode) {
		printf("Failed to allocate memory for stub cfg node object after break statement\n");
		return NULL;
	}
	stubNode->type = NODE_TYPE_EMPTY;
	
	/* Link break node to the empty node */
	addChild(breakCfgNode, stubNode);
	assignId(breakCfgNode);

	/* Link break to the outside loop node */
	addChild(breakCfgNode, outsideLoopNode);

	return stubNode;
}

CfgNode* parseSimpleStatement(CfgNode* curNode, pANTLR3_BASE_TREE simpleStatNode, CfgNode *outsideLoopNode)
{
	const char* baseStatementString = (const char*)simpleStatNode->getText(simpleStatNode)->chars;
	if (strcmp("ExpressionStatement", baseStatementString) == 0) {
		curNode = parseExpressionStatement(curNode, simpleStatNode);
	} else if (strcmp("IfStatement", baseStatementString) == 0) {
		curNode = parseIfStatement(curNode, simpleStatNode, outsideLoopNode);
	} else if (strcmp("WhileStatement", baseStatementString) == 0) {
		curNode = parseWhileStatement(curNode, simpleStatNode);
	} else if (strcmp("BlockStatement", baseStatementString) == 0) {
		curNode = parseBlockStatement(curNode, simpleStatNode, outsideLoopNode);
	} else if (strcmp("BreakStatement", baseStatementString) == 0) {
		curNode = parseBreakStatement(curNode, simpleStatNode, outsideLoopNode);
	}
	return curNode;
}

CfgNode* parseRepeatStatement(CfgNode* curNode, pANTLR3_BASE_TREE statementNode, pANTLR3_BASE_TREE repeatCondNode)
{
	/* Create an empty node that is the start of repeat statement */
	CfgNode* startRepeatNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!startRepeatNode) {
		printf("Failed to allocate memory for the cfg node object of the start of 'repeat statement'\n");
		return NULL;
	}
	startRepeatNode->type = NODE_TYPE_EMPTY;

	addChild(curNode, startRepeatNode);
	assignId(curNode);

	/* Create an empty node that is going to be after/outside of repeat statement */
	CfgNode* outsideRepeatNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!outsideRepeatNode) {
		printf("Failed to allocate memory for the cfg node object of the outside node of 'repeat statement'\n");
		return NULL;
	}
	outsideRepeatNode->type = NODE_TYPE_EMPTY;

	/* Parse statements in repeat body */
	CfgNode* endRepeatNode = parseSimpleStatement(startRepeatNode, statementNode, outsideRepeatNode);

	/* Parse condition node and its operation tree */
	CfgNode* condCfgNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!condCfgNode) {
		printf("Failed to allocate memory for repeat statement condition cfg node object\n");
		return NULL;
	}
	condCfgNode->type = NODE_TYPE_REPEAT_STAT;

	OpTreeNode* condOpTree = (OpTreeNode*)calloc(1, sizeof(OpTreeNode));
	parseOperationTree(condOpTree, repeatCondNode->getChild(repeatCondNode, 0));
	condCfgNode->opTree = condOpTree;

	/* End of repeat body link to condition node */
	addChild(endRepeatNode, condCfgNode);
	assignId(endRepeatNode);

	/* Condition node link to the start of repeat body */
	addChild(condCfgNode, startRepeatNode);
	assignId(condCfgNode);

	/* Link condition node to the outside node */
	addChild(condCfgNode, outsideRepeatNode);

	return outsideRepeatNode;
}

CfgNode* parseStatements(CfgNode *curNode, pANTLR3_BASE_TREE baseNode, int startIndex, int numberOfStatements, CfgNode *outsideLoopNode)
{
	for (int i = startIndex; i < startIndex + numberOfStatements; ++i) {
		pANTLR3_BASE_TREE statementNode = baseNode->getChild(baseNode, i);
		if (statementNode->getChildCount(statementNode) == 2) {		/* repeat-until loop */
			pANTLR3_BASE_TREE repeatStatNode = statementNode->getChild(statementNode, 0);
			pANTLR3_BASE_TREE repeatCondNode = statementNode->getChild(statementNode, 1);
			curNode = parseRepeatStatement(curNode, repeatStatNode, repeatCondNode);
		} else {
			pANTLR3_BASE_TREE baseStatementNode = statementNode->getChild(statementNode, 0);
			curNode = parseSimpleStatement(curNode, baseStatementNode, outsideLoopNode);
		}
	}
	return curNode;
}

CfgSubroutine *getCfgSubroutine(pANTLR3_BASE_TREE functionNode)
{
	CfgSubroutine* cfgSubroutine = (CfgSubroutine*)calloc(1, sizeof(CfgSubroutine));
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
	CfgNode* startNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!startNode) {
		printf("Failed to allocate memory for cfg node object\n");
		return NULL;
	}
	startNode->type = NODE_TYPE_START;
		
	CfgNode* curNode = parseStatements(startNode, functionNode, 1, functionNode->getChildCount(functionNode) - 1, NULL);

	CfgNode *endNode = (CfgNode*)calloc(1, sizeof(CfgNode));
	if (!endNode) {
		printf("Failed to allocate memory for cfg node object\n");
		return NULL;
	}
	endNode->type = NODE_TYPE_END;

	addChild(curNode, endNode);
	assignId(curNode);

	assignId(endNode);

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
	} else if (node->type == OP_TREE_NODE_TYPE_VALUE_BOOL) {
		snprintf(value, 30, "%d", node->data.number);
	} else if (node->type == OP_TREE_NODE_TYPE_VALUE_CHAR) {
		snprintf(value, 30, "%c", node->data.number);
	} else if (node->type == OP_TREE_NODE_TYPE_VALUE_STRING) {
		snprintf(value, 30, "%s", node->data.string);
	} else if (node->type == OP_TREE_NODE_TYPE_VALUE_PLACE) {
		snprintf(value, 30, "%s", node->data.identifier);
	} else {
		value[0] = '\0';
	}

	char* desc = (char*)calloc(1, strlen(value) + strlen(nodeType) + 2 + opTreeNodeMaxDigits);
	if (!desc) {
		printf("Failed to allocate memory for description string of cfg node\n");
		return NULL;
	}
	desc[0] = '\0';

	strcat(desc, nodeType);
	strcat(desc, " ");
	if (value[0] != '\0') {		/* If there is some value */
		strcat(desc, value);
		strcat(desc, " ");
	}

	char opTreeNodeCounterStr[10];
	snprintf(opTreeNodeCounterStr, 10, "%d", node->id);
	strcat(desc, opTreeNodeCounterStr);
	
	return desc;
}

void printOpTree(FILE *opTreeFile, OpTreeNode *opTree)
{
	char *curDesc = getOpTreeNodeDesc(opTree);

	for (int i = 0; i < opTree->numberOfNext; ++i) {
		OpTreeNode *next = opTree->next[i];
		char *nextDesc = getOpTreeNodeDesc(next);
		fprintf(opTreeFile, "\t\"%s\" -> \"%s\" [label=\"%d\"];\n", curDesc, nextDesc, i);
		free(nextDesc);

		printOpTree(opTreeFile, next);
	}
	free(curDesc);
}

char* getCfgNodeDesc(CfgNode* node)
{
	const int cfgNodeMaxDigits = 10;

	char* nodeType = getDescByNodeType(node->type);

	char* desc = (char*)calloc(1, strlen(nodeType) + 2 + cfgNodeMaxDigits);
	if (!desc) {
		printf("Failed to allocate memory for description string of cfg node\n");
		return NULL;
	}
	desc[0] = '\0';

	strcat(desc, nodeType);
	strcat(desc, " ");

	char cfgNodeCounterStr[10];
	snprintf(cfgNodeCounterStr, 10, "%d", node->id);
	strcat(desc, cfgNodeCounterStr);

	return desc;
}

void printCfgNode(FILE *cfgFile, CfgNode* curNode)
{
	if (curNode->isVisited) return;		/* Already were here */
	curNode->isVisited = true;

	if (curNode->type == NODE_TYPE_END)
		return;		/* Reached the end */

	char* curDesc = getCfgNodeDesc(curNode);

	if (curNode->type == NODE_TYPE_EMPTY || curNode->type == NODE_TYPE_BREAK_STAT
		|| curNode->type == NODE_TYPE_START) {
		;		/* No operation tree for such nodes */
	} else {
		/* Print out the link from cfg node to operation tree */
		char* curOpTreeDesc = getOpTreeNodeDesc(curNode->opTree);
		fprintf(cfgFile, "\t\"%s\"[style=filled, fillcolor=yellow];\n", curDesc);
		fprintf(cfgFile, "\t\"%s\" -> \"%s\";\n", curDesc, curOpTreeDesc);
		free(curOpTreeDesc);

		printOpTree(cfgFile, curNode->opTree);
	}

	for (int i = 0; i < curNode->numberOfNext; ++i) {
		CfgNode* next = curNode->next[i];
		char* nextDesc = getCfgNodeDesc(next);
		fprintf(cfgFile, "\t\"%s\"[style=filled, fillcolor=yellow];\n", curDesc);
		fprintf(cfgFile, "\t\"%s\"[style=filled, fillcolor=yellow];\n", nextDesc);
		fprintf(cfgFile, "\t\"%s\" -> \"%s\" [label=\"%d\"];\n", curDesc, nextDesc, i);
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
	printf("Printed out the cfg of subroutine - %s\n", cfgFile.headSubroutine->functionName);

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

