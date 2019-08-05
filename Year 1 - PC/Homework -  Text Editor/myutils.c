#include "myutils.h"

/**
 * Description: Erase leading whitespace from string.
 * Input: The string.
 * Output: N\A.
 **/
void TrimLeadingWhitespace(char *string) {
	char *p = string;
	int lenght = strlen(p);

	while(*p && isspace(*p)) {
		p++;
		lenght--;
	}

	memmove(string, p, lenght + 1);
}

/**
 * Description: Erase trailing whitespace from string.
 * Input: The string.
 * Output: N\A.
 **/
void TrimTrailingWhitespace(char *string) {
	char *p = string;
	int lenght = strlen(p);

	while(isspace(p[lenght-1]))
		p[--lenght] = 0;

	memmove(string, p, lenght + 1);
}

/**
 * Description: Find the first number in a string and remember in variable j
 the position in string where the number ends.
 * Input: The string, The address of variable j (position where the
 number ends).
 * Output: The number found or -1 if no number was found in the string.
 **/
int FindNumber(char *command, int *j){
	command += *j;
	while((*command) == ' ') {
		command++;
		(*j)++;
	}

	int number = 0;
	int found_number = 0;
	while((*command) <= '9' && (*command) >= '0') {
		number = number * 10 + ((*command) - '0');
		command++;
		(*j)++;

		found_number = 1;
	}

	if(found_number)
		return number;
	else
		return -1;
}

/**
 * Description: Finds the starting and ending line for the operation
 and checks if the operation is not valid.
 * Input: The command string, the addresses for the starting and ending line.
 * Output: 0 for valid command, 1 for invalid command.
 **/
int GetStartAndEnd(char *command, int *start_line, int *end_line) {
	int j = 0;

	*start_line = FindNumber(command, &j);
	*end_line = FindNumber(command, &j);

	if(*(command + j) != '\0' ||
	  (*start_line > *end_line && *end_line != -1))
		return 1;

	return 0;
}

/**
 * Description: Finds the maximum number of characters in a line.
 * Input: The text, the address for the number of lines in text.
 * Output: The length of the longest line.
 **/
int GetMaxLine(char text[MAX][MAX], int *text_line_count) { 
	int i, max_line_lenght = 0;

	for(i = 0; i < *text_line_count; i++) {
		int lenght = strlen(text[i]);
		if(lenght > max_line_lenght)
			max_line_lenght = lenght;
	}

	return max_line_lenght;
}

/**
 * Description: The next three functions are for formatting the lines in a list
 in 3 possible ways.
 * Input: The text, the starting and ending line, the special character. The
 function "AlphabeticList" receives additionally the parameter list_type
 which may be 'a' or 'A'.
 * Output: N\A.
 **/
void NumberedList(
	char text[MAX][MAX],
	int start_line,
	int end_line,
	char special_character
	) {

	int index = 1, index_lenght = 1;
	int i;

	for(i = start_line; i <= end_line; i++, index++) {
		if(index % 10 == 0)
			index_lenght++;

		if(text[i][0] == '\n') {
			text[i][index_lenght + 1] = '\n';
		} else {
			char aux[MAX];
			strcpy(aux, text[i]);
			memset(text[i], ' ', 2 + index_lenght);
			strcpy(text[i] + 2 + index_lenght, aux);
		}
		
		int j, x = index;

		for(j = index_lenght - 1; j >= 0; j--) {
			text[i][j] = ((x % 10) + '0');
			x /= 10;
		}

		text[i][index_lenght] = special_character;	
	}
}

void AlphabeticList(
	char text[MAX][MAX],
	int start_line,
	int end_line,
	char list_type,
	char special_character
	) {

	char index = list_type;

	int i;

	for(i = start_line; i <= end_line; i++, index++) {
		if(text[i][0] == '\n') {
			text[i][0] = index;
			text[i][1] = special_character;
			text[i][2] = '\n';
		} else {
			char aux[MAX];
			strcpy(aux, text[i]);
			memset(text[i], ' ', 3);
			strcpy(text[i] + 3, aux);
			text[i][0] = index;
			text[i][1] = special_character;
		}
	}
}

void BulletList(
	char text[MAX][MAX],
	int start_line,
	int end_line,
	char special_character
	) {

	int i;

	for(i = start_line; i <= end_line; i++) {
		if(text[i][0] == '\n') {
			text[i][0] = special_character;
			text[i][1] = '\n';
		} else {
			char aux[MAX];
			strcpy(aux, text[i]);
			memset(text[i], ' ', 2);
			strcpy(text[i] + 2, aux);
			text[i][0] = special_character;
		}
	}
}

/**
 * Description: Function used as parameter for qsort from stdlib library.
 * Input: The address of the strings which need to be compared.
 * Output: Negative number if *ia<*ib, 0 if the strings are identical,
 positive number if *ia>*ib.
 **/
int CompareStrings(const void *a, const void *b) {
	const char **ia = (const char **)a;
	const char **ib = (const char **)b;
	return strcmp(*ia, *ib);


}

/**
 * Description: Sorting the lines between start_line and end_line.
 * Input: The text, the starting and ending line, the type of sorting
 ('a' for ascending sort, 'z' for descending sort).
 * Output: N\A.
 **/
void SortList(
	char text[MAX][MAX],
	int start_line,
	int end_line,
	char ordering
	) {
	
	char *aux[MAX];
	int aux_line_count = end_line - start_line + 1;
	int i;

	for(i = 0; i < aux_line_count; i++)
		aux[i] = calloc(MAX, sizeof(char *));

	for(i = 0; i < aux_line_count; i++) {
		strcpy(aux[i], text[start_line + i]);
	}

	qsort(aux, aux_line_count, sizeof(char *), CompareStrings);


	if(ordering == 'a') {
		for(i = 0; i < aux_line_count; i++) {
			strcpy(text[start_line + i], aux[i]);
		}
	} else {
		for(i = 0; i < aux_line_count; i++) {
			strcpy(text[start_line + i], aux[aux_line_count - i - 1]);
		}
	}

	for(i = 0; i < aux_line_count; i++)
		free(aux[i]);
}
