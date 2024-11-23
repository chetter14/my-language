#ifndef MY_LANG_OP_TREE_H
#define MY_LANG_OP_TREE_H

typedef struct ExprNode {
	struct ExprNode* next;
	// add operands and operator
} ExprNode;

typedef struct OpTree {
	ExprNode* start;
} OpTree;

#endif	// MY_LANG_OP_TREE_H