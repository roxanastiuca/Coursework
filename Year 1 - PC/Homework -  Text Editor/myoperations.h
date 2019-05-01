#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "myutils.h"

#define MAX 1000
#define MAXCOMMANDS 10
#define MAXCHARINCOMMANDS 20
#define MAXINDENT 8

int Wrap(char result[MAX][MAX], int *result_line_count, char *command);
int Center(char result[MAX][MAX], int *result_line_count, char *command);
int AlignLeft(char result[MAX][MAX], int *result_line_count, char *command);
int AlignRight(char result[MAX][MAX], int *result_line_count, char *command);
int Justify(char text[MAX][MAX], int *text_line_count, char *command);
int Paragraphs(char result[MAX][MAX], int *result_line_count, char *command);
int Lists(char result[MAX][MAX], int *result_line_count, char *command);
int OrderedLists(char result[MAX][MAX], int *result_line_count, char *command);