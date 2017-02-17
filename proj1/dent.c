/**
   @file dent.c
   @author Xiaohui Z Ellis (xzheng6)

   This program reads text from standard input,
   and prints out correctly indented code based on the nesting of curly brackets.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/** Two-space indenting. */
#define INDENT 2

/** Exit code for invalid input. */
#define EXIT_UNSUCCESS 100

/** Level of indentation. */
int depth = 0;

/**
   Print out spaces to properly indent the start of a line to an indentation depth of d.
   @param d the indentation depth.
 */
void indent(int d)
{
  for (int i = 0; i < INDENT*d; i++) {
    putchar(' ');
  }
}

/**
   Return true if the given ch is either a space or a tab character.
   @param ch the character to check.
   @return true if the given ch is either a space or a tab character.
 */
bool isASpace(char ch)
{
  return ch == ' ' || ch == '\t';
}

/**
   Starting point for the program,
   it reads text from standard input until it reaches the end-of-file.
   Discard spaces at the start of the line until it reaches a non-whitespace character.
   Prints out spaces to properly indent the line
   and prints the rest of the line exactly like it appears in the input.
   Handles two special cases, lines starting with a closing curly bracket
   and lines containing only whitespaces.
   Handles the contents of double-quoted string literally.
   Handles invalid inputs where the input doesn't have matching opening and closing curly brackets.
   @return exit status for the program.
 */
int main()
{
  int ch = getchar();

  // Handle the input line-by-line.
  while (ch != EOF) {

    // Discard spaces at the start of the line until it reaches a non-whitespace character.
    while (isASpace(ch)) {
      ch = getchar();
    }

    // Handle non-blank/blank line.
    if (ch != '\n') {
      // Handle line starting with }.
      if (ch == '}') {
        depth--;
        // Handle the invalid input
        // where the input has more closing curly brackets than open curly brackets.
        if (depth < 0) {
          printf("Unmatched brackets\n");
          exit(EXIT_UNSUCCESS);
        }
      }
      indent(depth);
      if (ch == '{') {
        depth++;
      }
      // Handles the contents of double-quoted string literally.
      if (ch == '"') {
        putchar(ch);
        ch = getchar();
        while (ch != '"' && ch != EOF) {
          putchar(ch);
          ch = getchar();
        }
        if (ch == EOF) {
          return EXIT_SUCCESS;
        }
      }
      putchar(ch);
      ch = getchar();
      while (ch != '\n') {
        if (ch == '{') {
          depth++;
        }
        if (ch == '}') {
          depth--;
          // Handle the invalid input
          // where the input has more closing curly brackets than open curly brackets.
          if (depth < 0) {
            printf("Unmatched brackets\n");
            exit(EXIT_UNSUCCESS);
          }
        }
        // Handles the contents of double-quoted string literally.
        if (ch == '"') {
          putchar(ch);
          ch = getchar();
          while (ch != '"' && ch != EOF) {
            putchar(ch);
            ch = getchar();
          }
          if (ch == EOF) {
            return EXIT_SUCCESS;
          }
        }
        putchar(ch);
        ch = getchar();
      }
    }

    // Put the last character('\n') at the end of the line
    // and read the first character of the next line.
    putchar(ch);
    ch = getchar();
  }

  // Handle the invalid input
  // where the input has more opening curly brackets than closing curly brackets.
  if (depth > 0) {
    printf("Unmatched brackets\n");
    exit(EXIT_UNSUCCESS);
  }

  return EXIT_SUCCESS;
}
