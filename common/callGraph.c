#include "callGraph.h"
#include <stdio.h>
#include <stdlib.h>

static FILE* outputFile = NULL;
static CallGraph callGraph;

void addCall(const char* caller, const char* callee)
{
	if (outputFile == NULL) {		/* The first call to addCall() function */
		int errorCode = fopen_s(&outputFile, "CallGraph.txt", "w");
		if (outputFile == NULL) {	/* Failed to open the file */
			printf("ERROR! Failed to open CallGraph.txt file!\n");
			return;
		}
		callGraph.numberOfCalls = 0;
		callGraph.head = NULL;
	}

	CallerCalleePair* pair = (CallerCalleePair*)calloc(1, sizeof(CallerCalleePair));
	pair->caller = caller;
	pair->callee = callee;
	fprintf(outputFile, "%s -> %s\n", caller, callee);
	pair->next = NULL;

	if (callGraph.head == NULL) {	/* The first caller-callee pair */
		callGraph.head = pair;
	} else {
		pair->next = callGraph.head;
		callGraph.head = pair;
	}

	++callGraph.numberOfCalls;
}

void closeCallGraphFile()
{
	fclose(outputFile);
}