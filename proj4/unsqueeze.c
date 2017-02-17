/**
   @file unsqueeze.c
   @author Xiaohui Z Ellis (xzheng6)

   The unsqueeze program is the decompressor.
   This program implements the main function of the unsqueeze program.
   It will be responsible for handling the command-line arguments,
   using the bits component to read codes from the input file,
   using the codes component to convert them back to characters,
   and writing those to the output file.
 */

#include "bits.h"
#include <stdlib.h>

/** Exit code for invalid input. */
#define EXIT_UNSUCCESS 1

/** The number of command-line arguments. */
#define NUM_ARG 3

int main(int argc, char *argv[])
{
  if (argc != NUM_ARG) {
    fprintf(stderr, "usage: unsqueeze <infile> <outfile>\n");
    exit(EXIT_UNSUCCESS);
  }
  FILE *infp = fopen(argv[1], "rb");
  if (!infp) {
    fprintf(stderr, "%s: No such file or directory\n", argv[1]);
    exit(EXIT_UNSUCCESS);
  }
  FILE *outfp = fopen(argv[2], "wb");
  if (!outfp) {
    fclose(infp);
    fprintf(stderr, "%s: No such file or directory\n", argv[2]);
    exit(EXIT_UNSUCCESS);
  }
  BitBuffer *buffer = (BitBuffer *) malloc(sizeof(BitBuffer));
  buffer->bits = 0;
  buffer->bcount = 0;
  char ch;
  int code = read5Bits(buffer, infp);
  if (code == -1) {
    fclose(infp);
    fclose(outfp);
    free(buffer);
    return EXIT_SUCCESS;
  }
  if (code != 1 && code != 0) {
    fclose(infp);
    fclose(outfp);
    free(buffer);
    fprintf(stderr, "Invalid compressed format\n");
    exit(EXIT_UNSUCCESS);
  }
  if (code == 1) {
    while ((code = read5Bits(buffer, infp)) != -1) {
      if (code != COMMON)
        ch = codeToSym(code);
      else {
        code = read8Bits(buffer, infp);
        if (code == -1)
          break;
        ch = code;
      }
      fprintf(outfp, "%c", ch);
    }
  }
  else {
    while ((code = read8Bits(buffer, infp)) != -1) {
      ch = code;
      fprintf(outfp, "%c", ch);
    }
  }
  fclose(infp);
  fclose(outfp);
  free(buffer);
  return EXIT_SUCCESS;
}
