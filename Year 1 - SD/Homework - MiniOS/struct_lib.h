#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MEM 3145728
#define INF 2000000000

typedef int (*TFctCmp)(void *, void*); /* comparison function */
typedef void (*TFctFree)(void *); /*memory deallocation function */

typedef struct node {
	void *info;
	struct node *next;
} TNode, *TList, **AList;

typedef struct {
	size_t size_elem;
	TList start, end;
} TQueue;

typedef struct {
	size_t size_elem;
	TList top;
} TStack;

typedef struct {
	int pid, priority;

	long long exec_time, remaining_time;
	long long cicle_time;

	long long mem_size, remaining_mem;
	long long mem_start;

	void *stack;
} TProcess;

typedef struct {
	void *waiting, *finished;
	TList running;

	TList proc;

	int max_pid;
	long long max_time;
} TSystem;