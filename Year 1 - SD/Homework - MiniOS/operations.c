#include "functions_lib.h"

/**
 * Description: The operation for adding a process in the system.
 * Input: The input and output files; the system's informations.
 * Output: 0 for no error/ -1 for memory error.
 **/
int AddProcess(FILE *fin, FILE *fout, TSystem *sys) {
	TProcess p;

	fscanf(fin, "%lld %lld %d", &p.mem_size, &p.exec_time, &p.priority);

	p.pid = GetMinPID(sys);
	if(p.pid == -1)
		return -1; /* failed memory allocation */


	p.mem_start = GetMemStart(&(sys->proc), p.mem_size, p.pid);
	if(p.mem_start == -1) {
		fprintf(fout, "Cannot reserve memory for PID %d.\n", p.pid);
		return 0;
	} else if(p.mem_start == -2) {
		return -1; /* failed memory allocation */
	}

	p.remaining_time = p.exec_time;
	/* cicle"time is the remaining time out of a quantum for one process
	so its value will either be max_time of system or exec_time of process
	is that is smaller than the given quantum. */
	if(p.remaining_time < sys->max_time)
		p.cicle_time = p.remaining_time;
	else
		p.cicle_time = sys->max_time;
	p.remaining_mem = p.mem_size;
	p.stack = InitStack(4);
	if(!p.stack) {
		return -1; /* failed memory allocation */
	}

	TList new = AssignNode(sizeof(TProcess), &p);
	if(!new) {
		return -1; /* failed memory allocation */
	}

	/* The new process is put directly in the running state if there are no
	other processes in the system. */
	if(sys->running == NULL) {
		sys->running = new;
	} else {
		if(orderedIntrQ(sys->waiting, new, cmpWaitingProcess))
			return -1; /* failed memory allocation */
	}

	fprintf(fout, "Process created successfully: PID: %d, ", p.pid);
	fprintf(fout, "Memory starts at 0x%llx.\n", p.mem_start);

	/* Update max_pid, which holds the value for the higher PID of a process
	existing in the system: */
	if(p.pid > sys->max_pid)
		sys->max_pid = p.pid;

	return 0;
}

/**
 * Description: The operation for determining the state of a process.
 * Input: The input and output files; the system's information.
 * Output: 0 for no error/ -1 for memory error.
 **/
int GetProcessStatus(FILE *fin, FILE *fout, TSystem *sys) {
	int pid;
	fscanf(fin, "%d", &pid);

	int found = 0;

	/* Check if process is running: */
	if(sys->running && ((TProcess *) sys->running->info)->pid == pid) {
		fprintf(fout, "Process %d is running (remaining_time: %lld).\n",
			pid, ((TProcess *)sys->running->info)->remaining_time);
		found = 1;
	}

	/* Auxiliary queue, needed for parsing the qaiting and finished queue: */
	void *aux_q = InitQueue(sizeof(TProcess));
	if(!aux_q) {
		return -1; /* failed memory allocation */
	}

	/* Search if process is waiting: */
	TList elem = ExtrQ(sys->waiting);
	while(elem) {
		if(((TProcess *)elem->info)->pid == pid) {
			fprintf(fout, "Process %d is waiting (remaining_time: %lld).\n",
				pid, ((TProcess *)elem->info)->remaining_time);
			found = 1;
		}

		IntrQ(aux_q, elem);
		elem = ExtrQ(sys->waiting);
	}

	/* Move back in the waiting queue: */
	elem = ExtrQ(aux_q);
	while(elem) {
		IntrQ(sys->waiting, elem);
		elem = ExtrQ(aux_q);
	}

	/* Search if process is finished: */
	elem = ExtrQ(sys->finished);
	while(elem) {
		if(((TProcess *)elem->info)->pid == pid) {
			fprintf(fout, "Process %d is finished.\n", pid);
			found = 1;
		}
		IntrQ(aux_q, elem);
		elem = ExtrQ(sys->finished);
	}

	/* Move back in the finished queue: */
	elem = ExtrQ(aux_q);
	while(elem) {
		IntrQ(sys->finished, elem);
		elem = ExtrQ(aux_q);
	}

	free(aux_q);

	if(!found) {
		fprintf(fout, "Process %d not found.\n", pid);
	}

	return 0;
}

/**
 * Description: The operation for stacking the data of a process.
 * Input: The input and output files; the system's information.
 * Output: 0 for no error/ -1 for memory error.
 **/
int PushProcessStack(FILE *fin, FILE *fout, TSystem *sys) {
	int pid, data;
	/* Retain in the status variable if the process was found, it doesn't
	exist or if there was a failed memory allocation: */
	char status = 'f';

	fscanf(fin, "%d %d", &pid, &data);

	TProcess *process = FindProcess(sys, pid, &status);
	if(process == NULL || status == 'f') {
		if(status == '0') {
			return -1; /* failed memory allocation */
		} else {
			fprintf(fout, "PID %d not found.\n", pid);
			return 0;
		}
	}

	if(process->remaining_mem < 4) {
		fprintf(fout, "Stack overflow PID %d.\n", pid);
		return 0;
	}

	Push(process->stack, 4, &data);
	process->remaining_mem -= 4;

	return 0;
}

/**
 * Description: The operation for deleting the data in a process's stack.
 * Input: The input and output files; the system's information.
 * Output: 0 for no error/ -1 for memory error.
 **/
int PopProcessStack(FILE *fin, FILE *fout, TSystem *sys) {
	int pid;
	/* Retain in the status variable if the process was found, it doesn't
	exist or if there was a failed memory allocation: */
	char status = 'f';

	fscanf(fin, "%d", &pid);

	TProcess *process = FindProcess(sys, pid, &status);
	if(process == NULL || status == 'f') {
		if(status == '0') {
			return -1; /* failed memory allocation */
		} else {
			fprintf(fout, "PID %d not found.\n", pid);
			return 0;
		}
	}

	int data;
	if(Pop(process->stack, &data) == -1) {
		fprintf(fout, "Empty stack PID %d.\n", pid);
	} else {
		process->remaining_mem += 4;
	}

	return 0;
}

/**
 * Description: The operation for printing a process's stack.
 * Input: The input and output files; the system's information.
 * Output: 0 for no error/ -1 for memory error.
 **/
int PrintProcessStack(FILE *fin, FILE *fout, TSystem *sys) {
	int pid;
	/* Retain in the status variable if the process was found, it doesn't
	exist or if there was a failed memory allocation: */
	char status = 'f';

	fscanf(fin, "%d", &pid);

	TProcess *process = FindProcess(sys, pid, &status);
	if(process == NULL || status == 'f') {
		if(status == '0') {
			return -1;
		} else {
			fprintf(fout, "PID %d not found.\n", pid);
			return 0;
		}
	}

	if(((TStack *)process->stack)->top == NULL) {
		fprintf(fout, "Empty stack PID %d.\n", pid);
		return 0;
	}

	/* Auxiliary stack needed for parsing through the process's stack: */
	void *aux_s = InitStack(4);
	if(!aux_s) {
		return -1;
	}

	int data;
	while(Pop(process->stack, &data) == 0) {
		Push(aux_s, 4, &data);
	}

	fprintf(fout, "Stack of PID %d: ", pid);

	Pop(aux_s, &data);
	while(1) {
		Push(process->stack, 4, &data);

		fprintf(fout, "%d", data);
		if(!Pop(aux_s, &data)) {
			fprintf(fout, " ");
		} else {
			fprintf(fout, ".\n");
			break;
		}
	}

	free(aux_s);
	return 0;
}

/**
 * Description: The operation for printing the waiting priority queue.
 * Input: The output file; the queue.
 * Output: 0 for no error/ -1 for memory error.
 **/
int PrintWaitingQ(FILE *fout, void *queue) {
	void *aux_q = InitQueue(sizeof(TProcess));
	if(!aux_q) {
		return -1; /* failed memory allocation */
	}

	fprintf(fout, "Waiting queue:\n[");

	TList elem = ExtrQ(queue);
	while(elem) {
		TProcess p = *((TProcess *) elem->info);

		fprintf(fout, "(%d: priority = %d, remaining_time = %lld)",
						p.pid, p.priority, p.remaining_time);

		IntrQ(aux_q, elem);
		elem = ExtrQ(queue);

		if(elem)
			fprintf(fout, ",\n");
	}
	fprintf(fout, "]\n");

	/* Move back the elements from the auxiliary queue: */
	elem = ExtrQ(aux_q);
	while(elem) {
		IntrQ(queue, elem);
		elem = ExtrQ(aux_q);
	}

	free(aux_q);
	return 0;
}

/**
 * Description: The operation for printing the finished queue.
 * Input: The output file; the queue.
 * Output: 0 for no error/ -1 for memory error.
 **/
int PrintFinishedQ(FILE *fout, void *queue) {
	void *aux_q = InitQueue(sizeof(TProcess));
	if(!aux_q) {
		return -1; /* failed memory allocation */
	}

	fprintf(fout, "Finished queue:\n[");

	TList elem = ExtrQ(queue);
	while(elem) {
		TProcess p = *((TProcess *) elem->info);

		fprintf(fout, "(%d: priority = %d, executed_time = %lld)",
						p.pid, p.priority, p.exec_time);

		IntrQ(aux_q, elem);
		elem = ExtrQ(queue);

		if(elem)
			fprintf(fout, ",\n");
	}
	fprintf(fout, "]\n");

	/* Move back the elements from the auxiliary queue: */
	elem = ExtrQ(aux_q);
	while(elem) {
		IntrQ(queue, elem);
		elem = ExtrQ(aux_q);
	}

	free(aux_q);
	return 0;
}

/**
 * Description: The operation for simulating the running of the processes for
 a given time.
 * Input: The input file; the system's information.
 * Output: o for no error/ -1 for memory error.
 **/
int Run(FILE *fin, TSystem *sys) {
	long long time;
	fscanf(fin, "%lld", &time);

	/* Running stops when all processes are finished or the time expired. */
	while((time > 0) && (!EmptySystem(sys))) {
		/* The current process is the one already running: */
		TProcess *curr_proc = (TProcess *)sys->running->info;

		/* If there are no other processes, the only one afected is the current
		one: */
		if(((TQueue *)sys->waiting)->start == NULL) {
			if(time >= curr_proc->remaining_time) {
				TList aux = sys->running;
				sys->running = NULL;
				aux->next = NULL;
				IntrQ(sys->finished, aux);
				RemoveProc(sys, curr_proc->pid);
			} else {
				curr_proc->remaining_time -= time;

				time -= curr_proc->cicle_time;
				curr_proc->cicle_time = sys->max_time - time % sys->max_time;

				if(curr_proc->remaining_time < curr_proc->cicle_time)
					curr_proc->cicle_time = curr_proc->remaining_time;
			}

			time = 0;
		} else {
			if(time < curr_proc->cicle_time) {
				curr_proc->remaining_time -= time;

				curr_proc->cicle_time = sys->max_time - time;
				if(curr_proc->remaining_time < curr_proc->cicle_time)
					curr_proc->cicle_time = curr_proc->remaining_time;

				time = 0;
			} else {
				curr_proc->remaining_time -= curr_proc->cicle_time;
				time -= curr_proc->cicle_time;

				if(sys->max_time < curr_proc->remaining_time)
					curr_proc->cicle_time = sys->max_time;
				else
					curr_proc->cicle_time = curr_proc->remaining_time;

				TList aux = sys->running;
				sys->running = ExtrQ(sys->waiting);
				if(sys->running)
					sys->running->next = NULL;

				if(curr_proc->remaining_time > 0) {
					orderedIntrQ(sys->waiting, aux, cmpWaitingProcess);
				} else {
					IntrQ(sys->finished, aux);
					RemoveProc(sys, curr_proc->pid);
				}
			}
		}
	}

	return 0;
}

/**
 * Description: The operating for finishing the execution of all processes.
 * Input: The output file; the system's memory.
 * Output: 0 for no error/ -1 for memory error.
 **/
int FinishAll(FILE *fout, TSystem *sys) {
	if(EmptySystem(sys)) {
		fprintf(fout, "Total time: 0\n");
		return 0;
	}

	long long max_time = 0;

	if(sys->running)
		max_time += ((TProcess *)sys->running->info)->remaining_time;

	void *aux_q = InitQueue(sizeof(TProcess));
	if(!aux_q) {
		return -1;
	}

	TList elem = ExtrQ(sys->waiting);
	while(elem) {
		max_time += ((TProcess *)elem->info)->remaining_time;
		IntrQ(aux_q, elem);
		elem = ExtrQ(sys->waiting);
	}

	/* There is no need to move back the elements from the auxiliary queue
	since it is just going to be deallocated: */
	DestroyQueue(aux_q);

	fprintf(fout, "Total time: %lld\n", max_time);

	return 0;
}