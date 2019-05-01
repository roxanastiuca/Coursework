#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "myoperations.h"
#include "myutils.h"

#define MAX 1000
#define MAXCOMMANDS 10
#define MAXCHARINCOMMANDS 20
#define MAXINDENT 8

int Modify(char result[MAX][MAX], int *result_line_count, char *commands);