#include "functions_lib.h"

/** Initialize new queue: **/
void *InitQueue(size_t size_elem) {
	TQueue *q = (TQueue *) malloc(sizeof(TQueue));
	if(q) {
		q->size_elem = size_elem;
		q->start = q->end = NULL;
	}

	return (void *)q;
}

/** Initialize new stack: **/
void *InitStack(size_t size_elem) {
	TStack *s = (TStack *) malloc(sizeof(TStack));
	if(s) {
		s->size_elem = size_elem;
		s->top = NULL;
	}

	return (void *)s;
}

/** Initialize new system: **/
TSystem *InitSystem(size_t size_elem) {
	TSystem *sys = (TSystem *) malloc(sizeof(TSystem));
	if(!sys)
		return NULL;

	sys->waiting = sys->finished = sys->running = NULL;

	sys->waiting = InitQueue(size_elem);
	if(!sys->waiting) {
		free(sys);
		return NULL; /* failed memory allocation */
	}

	sys->finished = InitQueue(size_elem);
	if(!sys->finished) {
		free(sys->waiting);
		free(sys);
		return NULL; /* failed memory allocation */
	}

	sys->proc = NULL; /* empty list */
	sys->max_pid = 0;

	return sys;
}

/** Create a new list node: **/
TList AssignNode(size_t size_elem, void *elem) {
	TList aux = (TList) malloc(sizeof(TNode));
	if(aux) {
		aux->info = malloc(size_elem);
		if(!aux->info) {
			free(aux);
			return NULL; /* failed allocation */
		}

		memcpy(aux->info, elem, size_elem);
		aux->next = NULL;
	}

	return aux;
}

/** Insert a node in a queue. (the node has already been created and it
contains the information): **/
void IntrQ(void *queue, TList new) {
	TQueue *q = (TQueue *)queue;

	if(q->start == NULL) {
		q->start = q->end = new;
	} else {
		(q->end)->next = new;
		q->end = new;
	}
}

/** Extract a node from a queue: **/
TList ExtrQ(void *queue) {
	TQueue *q = (TQueue *)queue;
	TList aux = q->start;

	if(q->start == NULL)
		return NULL; /* empty queue */

	q->start = aux->next;
	if(q->start == NULL)
		q->end = NULL;

	aux->next = NULL;
	return aux;
}

/** The push operation for a stack, given the information's size **/
int Push(void *stack, size_t size_elem, void *elem) {
	TList aux = AssignNode(size_elem, elem);
	if(!aux)
		return -1; /* failed memory allocation */

	TStack *s = (TStack *) stack;

	aux->next = s->top;
	s->top = aux;

	return 0; /* no error */
}

/** The pop operation for a stack: **/
int Pop(void *stack, void *elem) {
	TStack *s = (TStack *) stack;

	if(s->top == NULL)
		return -1; /* empty stack */

	memcpy(elem, s->top->info, s->size_elem);
	TList aux = s->top;
	s->top = aux->next;

	free(aux->info);
	free(aux);

	return 0;
}

/** Insert in a priority queue: **/
int orderedIntrQ(void *queue, TList new, TFctCmp cmpFct) {
	void *aux_q = InitQueue(sizeof(TProcess));
	if(!aux_q) {
		return -1; /* failed memory allocation */
	}

	int ok = 1;
	TList elem = ExtrQ(queue);
	while(elem) {
		if(ok == 1 && cmpFct(elem->info, new->info) >= 0) {
			IntrQ(aux_q, new);
			ok = 0;
		}

		IntrQ(aux_q, elem);
		elem = ExtrQ(queue);
	}

	/* If the end of the queue was reached and the new elements wasn't added,
	then add it to the end: */
	if(ok == 1) {
		IntrQ(aux_q, new);
	}

	/* Move back from auxiliary queue: */
	elem = ExtrQ(aux_q);
	while(elem) {
		IntrQ(queue, elem);
		elem = ExtrQ(aux_q);
	}

	free(aux_q);
	return 0;
}

/** A TFctFree function, used as parameter for DestroyList. It frees a
process's memory: **/
void freeProcess(void *p) {
	TProcess *proc = (TProcess *)p;

	if(proc->stack)
		DestroyStack(proc->stack);

	free(p);
}

/** Free stack memory: **/
void DestroyStack(void *s) {
	int el;
	while(!Pop(s, &el));

	free(s);
}

/** Free list memory, using fctFree for freeing a node's information: **/
void DestroyList(AList aL, TFctFree fctFree) {
	while(*aL) {
		TList aux = *aL;
		*aL = (*aL)->next;

		fctFree(aux->info);
		free(aux);
	}
}

/** Free queue memory: **/
void DestroyQueue(void *q) {
	TList elem = ExtrQ(q);

	while(elem) {
		freeProcess(elem->info);
		free(elem);

		elem = ExtrQ(q);
	}

	free(q);
}

/** Free system memory: */
void DestroySystem(TSystem *sys) {
	if(sys->waiting)
		DestroyQueue(sys->waiting);
	if(sys->finished)
		DestroyQueue(sys->finished);
	if(sys->running)
		DestroyList(&(sys->running),freeProcess);
	if(sys->proc)
		DestroyList(&(sys->proc), free);

	free(sys);
}

/** Check if a system doesn't have any active processes.
-1 if so/ 0 if it has active processes. **/
int EmptySystem(TSystem *sys) {
	if(sys->running != NULL)
		return 0;

	if(((TQueue *)sys->waiting)->start != NULL)
		return 0;

	return -1;
}

/** Remove a process from the active processes list (finding it by its PID) **/
void RemoveProc(TSystem *sys, int pid) {
	TList p = sys->proc, ant = NULL;

	for(; p != NULL; ant = p, p = p->next)
		if(((TProcess *)p->info)->pid == pid)
			break;

	if(ant == NULL)
		sys->proc = p->next;
	else
		ant->next = p->next;

	free(p->info);
	free(p);
}

/** Check if a process is active: 1 if so/ 0 if not. **/
int FindInList(TList list, int val) {
	for(; list != NULL; list = list->next) {
		if(((TProcess *) list->info)->pid == val) {
			return 1;
		}
	}

	return 0;
}