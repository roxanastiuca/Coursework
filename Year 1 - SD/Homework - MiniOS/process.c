#include "functions_lib.h"

/**
 * Description: Determine the smallest unused PID.
 * Input: The system's information.
 * Output: The smallest unused PID/ -1 for memory error.
 **/
int GetMinPID(TSystem *sys) {
	int min = sys->max_pid + 1;

	void *aux_q = InitQueue(sizeof(TProcess));
	if(!aux_q)
		return -1; /* failed memory allocation */

	/* Parsing through the finished processes, we determine the smallest
	PID of a process that is not already used by an active process. */
	TList elem = ExtrQ(sys->finished);
	while(elem) {
		if(((TProcess *) elem->info)->pid < min &&
			!FindInList(sys->proc, ((TProcess *) elem->info)->pid)) {

			min = ((TProcess *) elem->info)->pid;
		}

		IntrQ(aux_q, elem);
		elem = ExtrQ(sys->finished);
	}

	/* Move back the elements from the auxiliary queue: */
	elem = ExtrQ(aux_q);
	while(elem) {
		IntrQ(sys->finished, elem);
		elem = ExtrQ(aux_q);
	}

	free(aux_q);
	return min;
}

/**
 * Description: The defragmentation of the operating system's memory.
 Give processes new memory locations.
 * Input: The address of the list for active processes.
 * Output: N\A.
 **/
void Defragmentation(AList aL) {
	TList list = *aL;
	/* Reset list as empty: */
	*aL = NULL;
	TList p = NULL, ant = NULL;

	/* Parse through list: */
	while(list) {
		TList aux = list;
		list = list->next;

		p = *aL;
		ant = NULL;

		/* Insert aux in *aL in ascending order according to PID: */
		for(; p != NULL; ant = p, p = p->next)
			if(((TProcess *)p->info)->pid > ((TProcess *)aux->info)->pid)
				break;
		
		aux->next = p;
		if(ant == NULL)
			*aL = aux;
		else
			ant->next = aux;
	}

	/* Update mem_start for processes: */
	long long prev_mem = 0;

	for(p = *aL; p != NULL; p = p->next) {
		TProcess *proc = (TProcess *)p->info;

		proc->mem_start = prev_mem;
		prev_mem += proc->mem_size;
	}
}

/**
 * Description: Get the start memory location for a new process and insert
 it in the list of active processes.
 * Input: The address for the list of active processes; the memory size and
 the PID of the new process.
 * Output: The start memory location/ -1 if there is not enough memory in the
 operating system for the process/ -1 for failed memory allocation.
 **/
long long GetMemStart(AList aL, long long mem_size, int pid) {
	if(mem_size > MAX_MEM)
		return -1;

	long long prev_mem = 0;

	TList p = *aL, ant = NULL;

	/* Parse through the active processes: */
	for(; p != NULL; ant = p, p = p->next) {
		TProcess *process = (TProcess *) p->info;

		/* Check if the new process fits before the current one: */
		if(prev_mem + mem_size <= process->mem_start)
			break;

		prev_mem = process->mem_start + process->mem_size;
	}

	/* Retain a process's information: */
	TProcess new_proc;
	new_proc.pid = pid;
	new_proc.mem_size = mem_size;
	new_proc.mem_start = prev_mem;

	if(p == NULL) {
		if(prev_mem + mem_size > MAX_MEM) {
			/* There is no large enough continuous memory zone for the new
			process, so a defragmentation takes place: */
			Defragmentation(aL);

			/* Get to the end of the active processes list to append the new
			process there: */
			for(p = *aL; p != NULL && p->next != NULL; p = p->next);
			prev_mem = ((TProcess *)p->info)->mem_start +
						((TProcess *)p->info)->mem_size;

			if(prev_mem + mem_size < MAX_MEM) {
				/* Insert new_node after p: */
				TList new_node = AssignNode(sizeof(TProcess), &new_proc);
				if(!new_node)
					return -2; /* failed memory allocation */

				p->next = new_node;
				return prev_mem;
			} else {
				return -1; /* there is not enough memory for the new process */
			}

		} else {
			/* Insert new_node before p: */
			TList new_node = AssignNode(sizeof(TProcess), &new_proc);
			if(!new_node)
				return -2; /* failed memory allocation */

			new_node->next = NULL;
			if(ant == NULL)
				*aL = new_node;
			else
				ant->next = new_node;

			return prev_mem;
		}
	} else {
		/* Insert new_node before p: */
		TList new_node = AssignNode(sizeof(TProcess), &new_proc);
		if(!new_node)
			return -2; /* failed memory allocation */

		new_node->next = p;
		if(ant == NULL)
			*aL = new_node;
		else
			ant->next = new_node;

		return prev_mem;
	}
}

/**
 * Description: Compare two processes by the priority queue's criteria.
 * Input: The two processes.
 * Output: <0 if a should be before b, >= if a should be after b.
 **/
int cmpWaitingProcess(void *a, void *b) {
	TProcess *x = (TProcess *)a;
	TProcess *y = (TProcess *)b;

	if(x->priority == y->priority) {
		if(x->remaining_time == y->remaining_time)
			return (x->pid - y->pid);

		return (x->remaining_time - y->remaining_time);
	}

	return (y->priority - x->priority);
}

/**
 * Description: Find a process that is running or waiting.
 * Input: The system's information, the process's PID, its status.
 * Output: The address of the process.
 **/
TProcess *FindProcess(TSystem *sys, int pid, char *status) {
	/** Check if the process is running: **/
	if(sys->running && ((TProcess *) sys->running->info)->pid == pid) {
		*status = 'r';
		return (TProcess *)sys->running->info;
	}

	void *aux_q = InitQueue(sizeof(TProcess));
	if(!aux_q) {
		*status = '0'; /* failed memory allocation */
		return NULL;

	}

	TProcess *p = NULL;

	/** Check if the process is waiting: **/
	TList elem = ExtrQ(sys->waiting);
	while(elem) {
		if(((TProcess *) elem->info)->pid == pid) {
			*status = 'w';
			p = (TProcess *) elem->info;
		}

		IntrQ(aux_q, elem);
		elem = ExtrQ(sys->waiting);
	}

	/* Move back the elements from the auxiliary queue: */
	elem = ExtrQ(aux_q);
	while(elem) {
		IntrQ(sys->waiting, elem);
		elem = ExtrQ(aux_q);
	}

	free(aux_q);
	return p;
}