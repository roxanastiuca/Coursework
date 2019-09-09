#include "functions_lib.h"

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "No files given.\n");
		return -1;
	}
	
	FILE *fin = fopen(argv[1], "rt");
	FILE *fout = fopen(argv[2], "wt");

	if((fin == NULL) || (fout == NULL)) {
		fprintf(stderr, "Cannot open files.\n");
		return -1;
	}

	TSystem *sys = InitSystem(sizeof(TProcess));
	if(!sys) {
		fprintf(stderr, "Cannot create system.\n");
		return -1;
	}

	/* Read the time quantum in which a process can run continuously: */
	fscanf(fin, "%lld", &(sys->max_time));

	char operation[10];
	int error = 0;

	while(fscanf(fin, "%s", operation) == 1) {
		if(strcmp(operation, "add") == 0) {
			error = AddProcess(fin, fout, sys);
		} else if(strcmp(operation, "print") == 0) {
			char aux_op[10];
			fscanf(fin, "%s", aux_op);

			if(strcmp(aux_op, "waiting") == 0) {
				error = PrintWaitingQ(fout, sys->waiting);
			} else if(strcmp(aux_op, "finished") == 0) {
				error = PrintFinishedQ(fout, sys->finished);
			} else if(strcmp(aux_op, "stack") == 0) {
				error = PrintProcessStack(fin, fout, sys);
			}
		} else if(strcmp(operation, "push") == 0) {
			error = PushProcessStack(fin, fout, sys);
		} else if(strcmp(operation, "pop") == 0) {
			error = PopProcessStack(fin, fout, sys);
		} else if(strcmp(operation, "get") == 0) {
			error = GetProcessStatus(fin, fout, sys);
		} else if(strcmp(operation, "run") == 0) {
			error = Run(fin, sys);
		} else if(strcmp(operation, "finish") == 0) {
			error = FinishAll(fout, sys);
		}

		if(error) {
			fprintf(stderr, "Insufficient memory.\n");
			/* Stop program execution, free all memory, close files and
			return the error -1 for insufficient memory: */
			break;
		}
	}

	/* Free all memory used by system: */
	DestroySystem(sys);

	/* Close files: */
	fclose(fin);
	fclose(fout);

	return error;
}