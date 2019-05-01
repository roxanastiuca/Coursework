#include <stdio.h>
#include <string.h>
#include "mycode.h"
#include "myoperations.h"

#define MAX 1000

/* This file contains mostly the frame-code given by the teacher. */
int main(int argc, char *argv[]) {
  char buf[MAX],               // buffer used for reading from the file
      original[MAX][MAX],     // original text, line by line
      result[MAX][MAX];       // text obtained after applying operations
  int original_line_count = 0,  // number of lines in the input file
      result_line_count = 0,    // number of lines in the output file
      i;

  if (argc != 4) {  // invalid number of arguments
    fprintf(stderr,
            "Usage: %s operations_string input_file output_file\n",
            argv[0]);
    return -1;
  }

  // Open input file for reading
  FILE *input_file = fopen(argv[2], "r");

  if (input_file == NULL) {
    fprintf(stderr, "File \"%s\" not found\n", argv[2]);
    return -2;
  }

  // Read data in file line by line
  while (fgets(buf, 1000, input_file)) {
    strcpy(original[original_line_count], buf);
    original_line_count++;
  }

  fclose(input_file);

  /** Here begins my contribution to this code: **/
  char text[MAX][MAX];
  int text_line_count;

  for(i = 0; i < original_line_count; i++)
    strcpy(text[i], original[i]);
  text_line_count = original_line_count;

  int error = Modify(text, &text_line_count, argv[1]);

  if(error) {
    switch(error) {
      case 1:
        printf("Invalid operation!\n");
        break;
      case 2:
        printf("Cannot wrap!\n");
        break;
    }
    
    for(i = 0; i < original_line_count; i++)
      strcpy(result[i], original[i]);
    result_line_count = original_line_count;

  } else {

    for(i = 0; i < text_line_count; i++)
      strcpy(result[i], text[i]);
    result_line_count = text_line_count;
  }
  /** Here my code ends. **/

  // Open output file for writing
  FILE *output_file = fopen(argv[3], "w");

  if (output_file == NULL) {
    fprintf(stderr, "Could not open or create file \"%s\"\n", argv[3]);
    return -2;
  }

  // Write result in output file
  for (i = 0; i < result_line_count; i++) {
    fputs(result[i], output_file);
  }

  fclose(output_file);
}
