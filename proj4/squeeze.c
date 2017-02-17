/**
   @file squeeze.c
   @author Xiaohui Z Ellis (xzheng6)

   The squeeze program is the compressor.
   This program works by encoding a file using 5-bit codes,
   for the most common symbols.
   Other, less common symbols end up needing 13 bits to encode.
   This program implements the main function of the squeeze program.
   It is responsible for handling the command-line arguments,
   reading bytes from the input file,
   using the codes component to convert them to codes,
   and using the bits component to write them out to the output file.
 */

#include "bits.h"
#include <stdlib.h>
#include <string.h>

/** Exit code for invalid input. */
#define EXIT_UNSUCCESS 1

/** The number of command-line arguments. */
#define NUM_ARG 3

/** The number of bytes to read at one time. */
#define BYTES 1024

int main(int argc, char *argv[])
{
  if (argc != NUM_ARG) {
    if (strcmp(argv[0], "./squeeze") == 0)
      fprintf(stderr, "usage: squeeze <infile> <outfile>\n");
    if (strcmp(argv[0], "./esqueeze") == 0)
      fprintf(stderr, "usage: esqueeze <infile> <outfile>\n");
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
  buffer->bits = 1;
  buffer->bcount = FIVE_BITS;
  int ch;
  int code;
  while ((ch = fgetc(infp)) != EOF) {
    code = symToCode(ch);
    write5Bits(code, buffer, outfp);
    if (code == COMMON) {
      write8Bits(ch, buffer, outfp);
    }
  }
  flushBits(buffer, outfp);
  fclose(infp);
  fclose(outfp);
  free(buffer);
  if (strcmp(argv[0], "./squeeze") == 0)
    return EXIT_SUCCESS;
  if (strcmp(argv[0], "./esqueeze") == 0) {
    FILE *fp1 = fopen(argv[1], "rb");
    FILE *fp2 = fopen(argv[2], "rb");
    char *tmp1 = (char *) malloc(BYTES);
    char *tmp2 = (char *) malloc(BYTES);
    int count1 = 0;
    int count2 = 0;
    int len1 = 0;
    int len2 = 0;
    while ((len1 = fread(tmp1, 1, BYTES, fp1)) != 0)
      count1 += len1;
    while ((len2 = fread(tmp2, 1, BYTES, fp2)) != 0)
      count2 += len2;
    fclose(fp1);
    fclose(fp2);
    free(tmp1);
    free(tmp2);
    if (count2 <= count1)
      return EXIT_SUCCESS;
    else {
      FILE *infp = fopen(argv[1], "rb");
      FILE *outfp = fopen(argv[2], "wb");
      BitBuffer *buffer = (BitBuffer *) malloc(sizeof(BitBuffer));
      buffer->bits = 0;
      buffer->bcount = FIVE_BITS;
      int ch;
      int code;
      while ((ch = fgetc(infp)) != EOF) {
        code = ch;
        write8Bits(code, buffer, outfp);
      }
      flushBits(buffer, outfp);
      fclose(infp);
      fclose(outfp);
      free(buffer);
      return EXIT_SUCCESS;
    }
  }
}
