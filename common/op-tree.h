#ifndef MY_LANG_OP_TREE_H
#define MY_LANG_OP_TREE_H

typedef enum {
	OP_TREE_NODE_TYPE_WRITE,
	OP_TREE_NODE_TYPE_READ,
	OP_TREE_NODE_TYPE_LOGICAL_OR,
	OP_TREE_NODE_TYPE_LOGICAL_AND,
	OP_TREE_NODE_TYPE_EQUAL,
	OP_TREE_NODE_TYPE_NOT_EQUAL,
	OP_TREE_NODE_TYPE_LESS,
	OP_TREE_NODE_TYPE_GREATER,
	OP_TREE_NODE_TYPE_LESS_EQUAL,
	OP_TREE_NODE_TYPE_GREATER_EQUAL,
	OP_TREE_NODE_TYPE_ADDITION,
	OP_TREE_NODE_TYPE_SUBTRACTION,
	OP_TREE_NODE_TYPE_MULTIPLICATION,
	OP_TREE_NODE_TYPE_DIVISION,
	OP_TREE_NODE_TYPE_REMAINDER,
	OP_TREE_NODE_TYPE_BIT_OR,
	OP_TREE_NODE_TYPE_BIT_XOR,
	OP_TREE_NODE_TYPE_BIT_AND,
	OP_TREE_NODE_TYPE_FUNCTION_CALL,
	OP_TREE_NODE_TYPE_ARRAY_ACCESS,
	OP_TREE_NODE_TYPE_INDEX,
	OP_TREE_NODE_TYPE_VALUE_PLACE,
	OP_TREE_NODE_TYPE_VALUE
} OpTreeNodeType;

typedef struct OpTreeNode {
	OpTreeNodeType type;
	struct OpTreeNode** next;	// there can be many next nodes
	int numberOfNext;
	union {
		int number;
		float real;
		char* string;
		char* identifier;
	} data;
} OpTreeNode;

typedef struct OpTree {
	OpTreeNode* start;
} OpTree;

#endif	// MY_LANG_OP_TREE_H