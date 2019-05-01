#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX 1000
#define MAXCOMMANDS 10
#define MAXCHARINCOMMANDS 20
#define MAXINDENT 8

void TrimLeadingWhitespace(char *string);
void TrimTrailingWhitespace(char *string);

int FindNumber(char *command, int *j);
int GetStartAndEnd(char *command, int *start_line, int *end_line);
int GetMaxLine(char result[MAX][MAX], int *result_line_count);

void NumberedList(
	char result[MAX][MAX],
	int start_line,
	int end_line,
	char special_character
	);
void AlphabeticList(
	char result[MAX][MAX],
	int start_line,
	int end_line,
	char list_type,
	char special_character
	);
void BulletList(
	char result[MAX][MAX],
	int start_line,
	int end_line,
	char special_character
	);

int CompareStrings(const void *a, const void *b);
void SortList(
	char result[MAX][MAX],
	int start_line,
	int end_line,
	char ordering
	);