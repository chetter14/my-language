#ifndef MY_LANG_CFG_H
#define MY_LANG_CFG_H

#include "op-tree.h"

typedef struct CfgNode {
	CfgNode** next;			// can be more than one possible next nodes
	OpTree* opTree;			// operations tree
} CfgNode;

typedef struct Cfg {
	CfgNode* start;
} Cfg;

typedef struct CfgSubroutine {
	char* name;
	Cfg cfg;
	char* sourceFile;
	// should parameters and return type be specified ?
} CfgSubroutine;

#endif	// MY_LANG_CFG_H