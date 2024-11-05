#include "cfg.h"
#include <antlr3.h>


Cfg getCfg(char** sourceFiles, pANTLR3_BASE_TREE* sourceASTs, char* errorMessage) {
	
	// File contains only one function

	/* 
	Algorithm:
	- Iterate over source files:
		(create cfg for subroutines)
		1) Remember the function name in global structure
		2) Step through statements, split into cfg nodes (made of operations trees - expressions),
			link "expression" nodes to one another
		3) If any error occurs, keep it in "errorMessage"
	- Link cfg of subroutines to one another
	- Return control flow graph for all files
	*/

}