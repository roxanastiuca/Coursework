Homework - Text Editor

The purpose of this homework was creating a text editor, which receives 3 arguments: the name of the input file, the name of the output file and a string, that contains the operations that should be applied to the text.

These are the possible operations and the way to call them:
- wrap text: "W max_line_length";
- center text: "C [start_line [end_line]]";
- allign left: "L [start_line [end_line]]";
- allign right: "R [start_line [end_line]]";
- justify: "J [start_line [end_line]]";
- indent paragraphs: "P indent_length [start_line [end_line]]";
- format lines in list: "I list_type special_character [start_line [end_line]]", where list_type can be 'n'(numbered list), 'a/A'(alphabetic list with lowercase/uppercase characters) or 'b'(bullet list);
- format lines in ordered list: "O list_type special_character ordering [start_line [end_line]]", where ordering is either 'a' for normal alphabetic list or 'z' for reversed alphabetic list.

There are four main files (and, excluding main, their headers):
- main.c which contains the frame-code given by the teacher;
- mycode.c which contains the main function where the commands are processed;
- myoperations.c which contains the functions for each possible operation;
- myutils.c which contains additional functions used by the main ones.
