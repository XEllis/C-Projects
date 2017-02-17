/**
   @file cross.c
   @author Xiaohui Z Ellis (xzheng6)

   This program reads a list of dictionary words,
   then helps users guess answers by showing users words of a particular length,
   that match characters they've already figured out.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/** Each word in the file containing the word list may contain at most 20 letters. */
#define LETTERS 20

/** The file containing the word list may contain at most 100000 words. */
#define WORDS 100000

/** A two-dimensional array of characters for storing the word list. */
char words[WORDS+1][LETTERS+1];

/** An integer representing the number of words on the word list. */
int wordCount;

/** Exit code for invalid input. */
#define EXIT_UNSUCCESS 1

/**
   Read the word list from the file with the given name,
   store it in the array words, and set wordCount.
   @param filename a pointer to a character.
 */
void readWords(char const *filename)
{
  FILE *fptr = fopen(filename, "r");
  if (!fptr) {
    fprintf(stderr, "Can't open word file\n");
    exit(EXIT_UNSUCCESS);
  }
  while (fscanf(fptr, "%s", words[wordCount]) == 1) {
    if (words[wordCount][LETTERS]) {
      fprintf(stderr, "Invalid word file\n");
      exit(EXIT_UNSUCCESS);
    }
    for (int i = 0; words[wordCount][i]; i++) {
      if (words[wordCount][i] < 'a' || words[wordCount][i] > 'z') {
        fprintf(stderr, "Invalid word file\n");
        exit(EXIT_UNSUCCESS);
      }
    }
    wordCount++;
    if (wordCount > WORDS) {
      fprintf(stderr, "Invalid word file\n");
      exit(EXIT_UNSUCCESS);
    }
  }
  fclose(fptr);
}

/**
   Prompt the user for a pattern and stores it in the given array pat.
   Detect and ignore invalid patterns and re-prompt the user until it gets a valid pattern.
   Return true if the user (eventually) enters a valid pattern,
   and false if it reaches end-of-file before getting a valid pattern.
   @param pat a pointer to a character.
   @return true if the user (eventually) enters a valid pattern,
   and false if it reaches end-of-file before getting a valid pattern.
 */
bool getPattern(char *pat)
{
  printf("pattern> ");
  for (int i = 0; i < LETTERS+1; i++) {
    pat[i] = '\0';
  }
  while (scanf("%21s", pat) == 1) {
    bool temp = false;
    if (pat[LETTERS]) {
      printf("Invalid pattern\n");
      int ch = getchar();
      while (ch != '\n') {
        ch = getchar();
      }
      temp = true;
    }
    else {
      for (int i = 0; pat[i]; i++) {
        if (pat[i] > 'z' || (pat[i] < 'a' && pat[i] != '?')) {
          printf("Invalid pattern\n");
          int ch = getchar();
          while (ch != '\n') {
            ch = getchar();
          }
          temp = true;
          break;
        }
      }
    }
    if (temp) {
      for (int i = 0; i < LETTERS+1; i++) {
        pat[i] = '\0';
      }
      printf("pattern> ");
    }
    else {
      return true;
    }
  }
  return false;
}

/**
   Given a word and a pattern, this function returns true,
   if the given word matches the given pattern.
   @param word a pointer to a character.
   @param pat a pointer to a character.
   @return true if the given word matches the given pattern.
 */
bool matches(char const *word, char const *pat)
{
  if (strlen(word) != strlen(pat)) {
    return false;
  }
  for (int i = 0; pat[i]; i++) {
    if (pat[i] != '?' && word[i] != pat[i]) {
      return false;
    }
  }
  return true;
}

/**
   Starting point for the program,
   it takes one command­-line argument, the name of a file containing the word list.
   The word list is a list of dictionary words this program is going to match against.
   The program will repeatedly prompt the user for patterns and report matches.
   It will terminate successfully when it reaches the end­-of-­file on standard input.
   @param argc the number of command-line arguments.
   @param argv command-line arguments.
   @return exit status for the program.
 */
int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "usage: cross <word-file>\n");
    exit(EXIT_UNSUCCESS);
  }
  readWords(argv[1]);
  char pat[LETTERS+1];
  while (getPattern(pat)) {
    for (int i = 0; i < wordCount; i++) {
      if (matches(words[i], pat)) {
        printf("%s\n", words[i]);
      }
    }
  }
  return EXIT_SUCCESS;
}
