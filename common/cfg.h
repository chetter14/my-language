#ifndef MY_LANG_CFG_H
#define MY_LANG_CFG_H

#include "op-tree.h"
#include "ast.h"

/*
file1:
	main() -> cfg1
	test() -> cfg2

file2:
	main() -> cfg3
	test() -> cfg4

callgraph (CG):		( iterate over all the CFGs and build CallGraph )
	f1 -> f2	( function f1() calls function f2() )
	(if there is a call of function that is not in call graph - throw error)

Every basic block in CFG has an operation tree root

Every function has:
	file name (where this function is from)
	function name
	return type
	parameters
	cfg

CFG has an entry block (START block) and end block (END)

There are different types of statements. Each statement is distinct in cfg (in my case)
	and each statement has its operation tree
*/

typedef enum {
	NODE_TYPE_IF_STAT,
	NODE_TYPE_WHILE_STAT,
	NODE_TYPE_REPEAT_STAT,
	NODE_TYPE_EXPR_STAT,
	NODE_TYPE_BREAK_STAT,
	NODE_TYPE_START,			// as START
	NODE_TYPE_END,				// as END
	NODE_TYPE_EMPTY				// used as a helper node for cfg building
} CfgNodeType;

typedef struct CfgNode {
	CfgNodeType type;
	struct CfgNode* next[2];		// there can be maximum 2 next nodes (true/false for conditions)
	int numberOfNext;
	OpTreeNode* opTree;					// operations tree
	int id;
} CfgNode;

// BuiltInType:
typedef enum BuiltInType {
	BUILT_IN_TYPE_BOOL,
	BUILT_IN_TYPE_BYTE,
	BUILT_IN_TYPE_INT,
	BUILT_IN_TYPE_UINT,
	BUILT_IN_TYPE_LONG,
	BUILT_IN_TYPE_ULONG,
	BUILT_IN_TYPE_CHAR,
	BUILT_IN_TYPE_STRING,
} BuiltInType;

// ArrayType:
typedef enum {
	ARRAY_TYPE_BUILT_IN_TYPE,
	ARRAY_TYPE_IDENTIFIER
} ArrayTypeTag;

typedef struct {
	ArrayTypeTag tag;
	union {
		BuiltInType builtInType;
		char* identifier;
	};
	int dimensions[5];		// assume 5 is the maximum number of possible dimensions
	int numberOfDimensions;
} ArrayType;

// TypeRef:
typedef enum {
	TYPE_REF_NO_TYPE,
	TYPE_REF_BUILT_IN_TYPE,
	TYPE_REF_IDENTIFIER,
	TYPE_REF_ARRAY_TYPE
} TypeRefTag;

typedef struct {
	TypeRefTag tag;
	union {
		BuiltInType builtInType;
		char* identifier;
		ArrayType arrayType;
	};
} TypeRef;

// Parameter:
typedef struct Parameter {
	char* identifier;
	TypeRef typeRef;
	struct Parameter* next;
} Parameter;

// ParameterList:
typedef struct {
	Parameter* headParameter;
	int numberOfParams;
} ParameterList;

// Subroutine:
typedef struct CfgSubroutine {
	char* functionName;
	ParameterList parameterList;
	TypeRef returnType;
	CfgNode* cfgStart;
	struct CfgSubroutine* next;
} CfgSubroutine;

// File:
typedef struct CfgFile {
	char* fileName;
	CfgSubroutine* headSubroutine;
	int numberOfSubroutines;
} CfgFile;

CfgFile getCfg(char* sourceFile, pANTLR3_BASE_TREE sourceAST, char* errorMessage);

#endif	// MY_LANG_CFG_H