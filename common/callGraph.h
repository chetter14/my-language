#ifndef MY_LANG_CALL_GRAPH_H
#define MY_LANG_CALL_GRAPH_H

typedef struct CallerCalleePair {
	char* caller;
	char* callee;
	struct CallerCalleePair* next;
} CallerCalleePair;

typedef struct {
	int numberOfCalls;
	CallerCalleePair *head;
} CallGraph;

void addCall(const char* caller, const char* callee);
void closeCallGraphFile();

#endif	// MY_LANG_CALL_GRAPH_H