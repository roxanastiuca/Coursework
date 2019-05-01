#include "mycode.h"

/**
 * Description: This is the main function, where commands are processed and
 where other functions are called.
 * Input: The text, its number of lines, the string of commands.
 * Output: 0 for no errors, 1 for "Invalid operation!", 2 for "Cannot wrap".
 **/
int Modify(char text[MAX][MAX], int *text_line_count, char *commands) {
	int i;

	/** Erase trailing whitespace from the original text: **/
	for(i = 0; i < *text_line_count; i++) {
		TrimTrailingWhitespace(text[i]);
		strcat(text[i], "\n");
	}
	
	char list_commands[MAXCOMMANDS][MAXCHARINCOMMANDS];
	i = 0;

	/** Extract the commands from the string and add them to an array: **/
	char *command;
	command = strtok(commands, ",\n");
	while(command != NULL) {
		if(i >= MAXCOMMANDS) {
			printf("Too many operations! Only the first 10 will be applied.");
			printf("\n");
			break;
		}
		if(strlen(command) > MAXCHARINCOMMANDS)
			return 1;

		strcpy(list_commands[i], command);
		TrimLeadingWhitespace(list_commands[i]);
		TrimTrailingWhitespace(list_commands[i]);

		i++;

		command = strtok(NULL, ",\n");
	}

	int commands_count = i;
	
	int error = 0;

	for(i = 0; i < commands_count; i++) {

		char action = list_commands[i][0];
		/** Some commands may be given in lowercase, so I change them
		to uppercase: **/
		if(action >= 'a' && action <= 'z')
			action += ('A' - 'a');
		
		switch(action) {
			case 'W':
				error = Wrap(text, text_line_count, list_commands[i]);
				break;
			case 'C':
				error = Center(text, text_line_count, list_commands[i]);
				break;
			case 'L':
				error = AlignLeft(text, text_line_count, list_commands[i]);
				break;
			case 'R':
				error = AlignRight(text, text_line_count, list_commands[i]);
				break;
			case 'J':
				error = Justify(text, text_line_count, list_commands[i]);
				break;
			case 'P':
				error = Paragraphs(text, text_line_count, list_commands[i]);
				break;
			case 'I':
				error = Lists(text, text_line_count, list_commands[i]);
				break;
			case 'O':
				error = OrderedLists(text, text_line_count, list_commands[i]);
				break;
			default:
				error = 1;
		}

		if(error)
			return error;

	}
	
	return 0;
}