#include "struct_lib.h"

/* operations.c */
int AddProcess(FILE *fin, FILE *fout, TSystem *sys);
int GetProcessStatus(FILE *fin, FILE *fout, TSystem *sys);
int PushProcessStack(FILE *fin, FILE *fout, TSystem *sys);
int PopProcessStack(FILE *fin, FILE *fout, TSystem *sys);
int PrintProcessStack(FILE *fin, FILE *fout, TSystem *sys);
int PrintWaitingQ(FILE *fout, void *queue);
int PrintFinishedQ(FILE *fout, void *queue);
int Run(FILE *fin, TSystem *sys);
int FinishAll(FILE *fout, TSystem *sys);

/* q_s_list.c */
void *InitQueue(size_t size_elem);
void *InitStack(size_t size_elem);
TSystem *InitSystem(size_t size_elem);
TList AssignNode(size_t size_elem, void *elem);
void IntrQ(void *queue, TList new);
TList ExtrQ(void *queue);
int Push(void *stack, size_t size_elem, void *elem);
int Pop(void *stack, void *elem);
int orderedIntrQ(void *queue, TList new, TFctCmp cmpFct);
void freeProcess(void *p);
void DestroyStack(void *s);
void DestroyList(AList aL, TFctFree fctFree);
void DestroyQueue(void *q);
void DestroySystem(TSystem *sys);
int EmptySystem(TSystem *sys);
void RemoveProc(TSystem *sys, int pid);
int FindInList(TList list, int val);

/* process.c */
int GetMinPID(TSystem *sys);
void Defragmentation(AList aL);
long long GetMemStart(AList aL, long long mem_size, int pid);
int cmpWaitingProcess(void *a, void *b);
TProcess *FindProcess(TSystem *sys, int pid, char *status);