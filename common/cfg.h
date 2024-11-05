#ifndef MY_LANG_CFG_H
#define MY_LANG_CFG_H

#include "op-tree.h"

typedef struct CfgNode {
	CfgNode* next;			// can be more than one possible next nodes
	OpTree* opTree;			// operations tree
} CfgNode;

typedef struct CfgSubroutine {
	char* name;
	CfgNode* start;
	char* sourceFile;
} CfgSubroutine;

typedef struct Cfg {
	CfgSubroutine* subroutines;
} Cfg;

#endif	// MY_LANG_CFG_H