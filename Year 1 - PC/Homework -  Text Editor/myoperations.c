#include "myoperations.h"

/**
 * Description: The operation for wrapping the text's lines in a given
 number of characters (max_line_length).
 * Input: The text, its number of lines, the command (which starts with 'W').
 * Ouput: 0 for valid operation, 1 for "Invalid operation!", 2 for
 "Cannot wrap".
 **/
int Wrap(char text[MAX][MAX], int *text_line_count, char *command) {
	int max_line_lenght, j = 1;
	max_line_lenght = FindNumber(command, &j);

	if(*(command + j) != '\0' || max_line_lenght == -1)
		return 1;

	char aux[MAX][MAX] = {{0}};
	int aux_line_count = 0, char_in_line = 0, i;

	for(i = 0; i < (*text_line_count); i++) {
		
		if(text[i][0] == '\n' ){
			aux_line_count += 2;
			int leading_whitespace = 0;
			while(text[i+1][leading_whitespace] == ' ')
				leading_whitespace++;

			while(leading_whitespace > max_line_lenght) {
				aux_line_count++;
				leading_whitespace -= (max_line_lenght + 1);
			}

			if(leading_whitespace > 0) {
				memset(aux[aux_line_count], ' ', leading_whitespace - 1);
				char_in_line = leading_whitespace - 1;
			} else {
				char_in_line = 0;
			}

			continue;
		}

		if(i == 0) {
			int leading_whitespace = 0;
			while(text[i][leading_whitespace] == ' ')
				leading_whitespace++;

			while(leading_whitespace > max_line_lenght) {
				aux_line_count++;
				leading_whitespace -= (max_line_lenght + 1);
			}

			if(leading_whitespace > 0) {
				memset(aux[aux_line_count], ' ', leading_whitespace - 1);
				char_in_line = leading_whitespace - 1;
			}
		}

		char *word;
		word = strtok(text[i], " \n");

		int char_in_word;
		
		while(word != NULL) {
			
			char_in_word = strlen(word);
			if(char_in_word > max_line_lenght)
				return 2;

			if(char_in_line == 0) {

				strcpy(aux[aux_line_count], word);
				char_in_line = char_in_word;

			} else {

				if(char_in_line + char_in_word + 1 <= max_line_lenght) {

					strcat(aux[aux_line_count], " ");
					strcat(aux[aux_line_count], word);
					char_in_line += char_in_word + 1;

				} else {

					aux_line_count++;
					strcpy(aux[aux_line_count], word);
					char_in_line = char_in_word;

				}
			}

			word = strtok(NULL, " \n");
		}
	}
	
	aux_line_count++;

	for(i = 0; i < aux_line_count; i++)
		strcat(aux[i], "\n");

	for(i = 0; i < aux_line_count; i++)
		strcpy(text[i], aux[i]);
	
	*text_line_count = aux_line_count;

	return 0;

}

/**
 * Description: The operation for centering the text's lines.
 * Input: The text, its number of lines, the command.
 * Output: 0 for valid operation, 1 for invalid operation.
 **/
int Center(char text[MAX][MAX], int *text_line_count, char *command) {
	int start_line, end_line;
	int error = GetStartAndEnd(command + 1, &start_line, &end_line);
	
	if(error)
		return error;

	if(start_line == -1)
		start_line = 0;
	if(end_line == -1 || end_line > *text_line_count)
		end_line = *text_line_count - 1;

	int max_line_lenght = GetMaxLine(text, text_line_count);

	int i;

	for(i = start_line; i <= end_line; i++) {
		int lenght = strlen(text[i]);

		if(lenght < max_line_lenght && text[i][0] != '\n') {
			char aux[MAX];
			strcpy(aux,text[i]);
			int middle = (max_line_lenght - lenght) / 2;
			
			if((max_line_lenght - lenght) % 2 == 0) {
				memset(text[i], ' ', middle);
				strcpy(text[i] + middle, aux);
			} else {
				memset(text[i], ' ', middle + 1);
				strcpy(text[i] + middle + 1, aux);
			}
		}
	}

	return 0;
}

/**
 * Description: The operation for alligning the text to the left.
 * Input: The text, its number of lines, the command.
 * Output: 0 for valid command, 1 for invalid command.
 **/
int AlignLeft(char text[MAX][MAX], int *text_line_count, char *command) {
	int start_line, end_line;
	int error = GetStartAndEnd(command + 1, &start_line, &end_line);
	
	if(error)
		return error;

	if(start_line == -1)
		start_line = 0;
	if(end_line == -1 || end_line > *text_line_count)
		end_line = *text_line_count - 1;

	for(int i = start_line; i <= end_line; i++) {
		if(text[i][0] != '\n')
			TrimLeadingWhitespace(text[i]);
	}

	return 0;
}

/**
 * Description: The operation for alligning the text to the right.
 * Input: The text, its number of lines, the command.
 * Output: 0 for valid operation, 1 for invalid operation.
 **/
int AlignRight(char text[MAX][MAX], int *text_line_count, char *command) {
	int start_line, end_line;
	int error = GetStartAndEnd(command + 1, &start_line, &end_line);
	if(error)
		return error;

	if(start_line == -1)
		start_line = 0;
	if(end_line == -1 || end_line > *text_line_count)
		end_line = *text_line_count - 1;

	int max_line_lenght = GetMaxLine(text, text_line_count);

	int i;

	for(i = start_line; i <= end_line; i++) {
		int lenght = strlen(text[i]);
		if(lenght < max_line_lenght && text[i][0] != '\n') {
			char aux[MAX];
			strcpy(aux, text[i]);
			memset(text[i], ' ', max_line_lenght - lenght);
			strcpy(text[i] + (max_line_lenght - lenght), aux);
		}
	}

	return 0;
}

/**
 * Description: The operation for justifying the lines of a text (balancing
 the words in a line so that each line has the same number of characters).
 * Input: The text, its number of lines, the command.
 * Output: 0 for valid operation, 1 for invalid operation.
 **/
int Justify(char text[MAX][MAX], int *text_line_count, char *command) {
	int start_line, end_line;
	int error = GetStartAndEnd(command + 1, &start_line, &end_line);
	if(error)
		return error;

	if(start_line == -1)
		start_line = 0;
	if(end_line == -1 || end_line > *text_line_count)
		end_line = *text_line_count - 1;

	int max_line_lenght = GetMaxLine(text, text_line_count);

	int i;

	for(i = start_line; i <= end_line; i++) {
		if(text[i][0] != '\n')
			TrimLeadingWhitespace(text[i]);
		
		int lenght = strlen(text[i]);
	
		/**
		 The lines at the ending of the paragraph, the empty lines (which
		 contain only the endline character) or the lines that already have
		 max_line_length length remain unmodified.
		 **/
		if((lenght < max_line_lenght) && (text[i+1][0] != '\n') &&
			(text[i][0] != '\n') && (i != *text_line_count - 1)) {
			
			/** Calculate the number of spaces in line: **/
			int spaces_count = 0;
			int poz = 0;

			while(poz < lenght) {
				poz += strcspn(text[i] + poz, " ");
				while(*(text[i] + poz) == ' ') {
					poz++;
					spaces_count++;
				}
			}

			if(spaces_count > 0) {
				int spaces_necessary, spaces_left;

				spaces_necessary = (max_line_lenght - lenght) / spaces_count;
				spaces_left = (max_line_lenght - lenght) % spaces_count;
			
				poz = strcspn(text[i], " ");

				while(poz < lenght) {
					while(*(text[i] + poz) == ' ')
						poz++;

					char aux[MAX];
					strcpy(aux, text[i] + poz);
					
					if(spaces_left) {
						memset(text[i] + poz, ' ', spaces_necessary + 1);
						poz += (spaces_necessary + 1);
						lenght += (spaces_necessary + 1);
						strcpy(text[i] + poz, aux);
						spaces_left--;
					} else {
						memset(text[i] + poz, ' ', spaces_necessary);
						poz += spaces_necessary;
						lenght += spaces_necessary;
						strcpy(text[i] + poz, aux);
					}

					poz += strcspn(text[i] + poz, " ");
				}
			}
		}
	}

	return 0;
}

/**
 * Description: The operation for indenting the paragraphs.
 * Input: The text, its number of lines, the command.
 * Output: 0 for valid operation, 1 for invalid operation.
 **/
int Paragraphs(char text[MAX][MAX], int *text_line_count, char *command) {
	int j = 1;
	int indent_lenght = FindNumber(command, &j);
	if(indent_lenght > MAXINDENT || indent_lenght == -1)
		return 1;

	int start_line, end_line;
	int error = GetStartAndEnd(command + j, &start_line, &end_line);

	if(error)
		return error;

	if(start_line == -1)
		start_line = 0;
	if(end_line == -1 || end_line > *text_line_count)
		end_line = *text_line_count - 1;

	int i;

	for(i = start_line; i <= end_line; i++) {
		if(i == 0 || (text[i-1][0] == '\n')) {
			TrimLeadingWhitespace(text[i]);

			char aux[MAX];
			strcpy(aux, text[i]);
			memset(text[i], ' ', strlen(text[i]));
			strcpy(text[i] + indent_lenght, aux);
		}
	}

	return 0;
}

/**
 * Description: The operation for formatting the lines as a list.
 * Input: The text, its number of lines, the command.
 * Output: 0 for valid operation, 1 for invalid operation.
 **/
int Lists(char text[MAX][MAX], int *text_line_count, char *command) {
	char list_type, special_character;
	list_type = *(command + 2);
	special_character = *(command + 4);

	/** Check the operation is valid: **/
	if(list_type != 'n' && list_type != 'a' &&
		list_type != 'A' && list_type != 'b')
		return 1;

	int start_line, end_line;
	int error = GetStartAndEnd(command + 5, &start_line, &end_line);

	if(error)
		return error;

	if(start_line == -1)
		start_line = 0;
	if(end_line == -1 || end_line > *text_line_count)
		end_line = *text_line_count - 1;

	int i;

	for(i = start_line; i <= end_line; i++) {
		if(text[i][0] != '\n') {
			TrimLeadingWhitespace(text[i]);
		}
	}
	
	switch(list_type) {
		case 'n':
			NumberedList(text, start_line, end_line, special_character);
			break;
		case 'a':
			AlphabeticList(text, start_line, end_line, list_type,
				special_character);
			break;
		case 'A':
			AlphabeticList(text, start_line, end_line, list_type,
				special_character);
			break;
		case 'b':
			BulletList(text, start_line, end_line, special_character);
			break;
		default:
			return 1;
	}
	
	return 0;
}

/**
 * Description: The operation for arranging the lines in a list.
 * Input: The list, its number of lines, the command.
 * Output: 0 for valid operation, 1 for invalid operation.
 **/
int OrderedLists(char text[MAX][MAX], int *text_line_count, char *command) {
	char list_type, special_character, ordering;
	list_type = *(command + 2);
	special_character = *(command + 4);
	ordering = *(command + 6);

	/** Check the operation is valid: **/
	if(list_type != 'n' && list_type != 'a' &&
		list_type != 'A' && list_type != 'b')
		return 1;
	if(ordering != 'a' && ordering != 'z')
		return 1;

	int start_line, end_line;
	int error = GetStartAndEnd(command + 7, &start_line, &end_line);
	
	if(error)
		return error;

	if(start_line == -1)
		start_line = 0;
	if(end_line == -1 || end_line > *text_line_count)
		end_line = *text_line_count - 1;

	if(start_line < end_line)
		SortList(text, start_line, end_line, ordering);

	int i;

	for(i = start_line; i <= end_line; i++)
		TrimLeadingWhitespace(text[i]);

	switch(list_type) {
		case 'n':
			NumberedList(text, start_line, end_line, special_character);
			break;
		case 'a':
			AlphabeticList(text, start_line, end_line, list_type,
				special_character);
			break;
		case 'A':
			AlphabeticList(text, start_line, end_line, list_type,
				special_character);
			break;
		case 'b':
			BulletList(text, start_line, end_line, special_character);
			break;
		default:
			return 1;
	}

	return 0;

}