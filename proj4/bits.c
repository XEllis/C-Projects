/**
   @file bits.c
   @author Xiaohui Z Ellis (xzheng6)

   This program define functions for writing out bit sequences to and from a file.
 */

#include "bits.h"

/**
   Write the 5-bit code stored in the code parameter.
   Temporarily store bits in the given buffer until we have 8 of them,
   and can write them to the file.
   @param code to write out, a value betteen 0 and 31.
   @param pointer to storage for bits that can't be written to the file yet.
   When this function is called,
   this buffer may already contain some bits left-over from a previous write.
   @param fp file we're writing to, opened for writing in binary mode.
 */
void write5Bits(int code, BitBuffer *buffer, FILE *fp)
{
  if (buffer->bcount <= BITS_PER_BYTE - FIVE_BITS) {
    buffer->bits = buffer->bits << FIVE_BITS;
    buffer->bits |= code;
    buffer->bcount += FIVE_BITS;
    if (buffer->bcount == BITS_PER_BYTE) {
      fprintf(fp, "%c", buffer->bits);
      buffer->bits = 0;
      buffer->bcount = 0;
    }
  }
  else {
    int to8 = BITS_PER_BYTE - buffer->bcount;
    int left = FIVE_BITS - to8;
    buffer->bits = buffer->bits << to8;
    int tmp = code >> left;
    buffer->bits |= tmp;
    fprintf(fp, "%c", buffer->bits);
    buffer->bits = code << (BITS_PER_BYTE - left);
    buffer->bits = buffer->bits >> (BITS_PER_BYTE - left);
    buffer->bcount = left;
  }
}

/**
   Just like write5Bits(), but writing 8 bits at a time.
   @param code to write out, a value betteen 0 and 255.
   @param pointer to storage for bits that can't be written to the file yet.
   @param fp file we're writing to.
 */
void write8Bits(int code, BitBuffer *buffer, FILE *fp)
{
  int to8 = BITS_PER_BYTE - buffer->bcount;
  int left = BITS_PER_BYTE - to8;
  buffer->bits = buffer->bits << to8;
  int tmp = code >> left;
  buffer->bits |= tmp;
  fprintf(fp, "%c", buffer->bits);
  buffer->bits = code << (BITS_PER_BYTE - left);
  buffer->bits = buffer->bits >> (BITS_PER_BYTE - left);
  buffer->bcount = left;
}

/**
   If there are any bits buffered in buffer, write them out to the given file,
   in the high-order bit positions of a byte, leaving zeros in the low-order bits.
   @param buffer pointer to storage for unwritten bits left over from a previous write.
   @param fp file these bits are to be written to, opened for writing.
 */
void flushBits(BitBuffer *buffer, FILE *fp)
{
  if (buffer->bcount == 0)
    return;
  if (buffer->bcount > BITS_PER_BYTE - FIVE_BITS)
    buffer->bits = buffer->bits << (BITS_PER_BYTE - buffer->bcount);
  else {
    buffer->bits = buffer->bits << FIVE_BITS;
    buffer->bits |= COMMON;
    buffer->bits = buffer->bits << (BITS_PER_BYTE - FIVE_BITS - buffer->bcount);
  }
  fprintf(fp, "%c", buffer->bits);
  buffer->bits = 0;
  buffer->bcount = 0;
}

/**
   Read and return the next 5-bit code from the given file.
   The given buffer may contain some bits left over from the last read,
   and if this read has any left-over bits, it should leave them in that buffer.
   @param buffer pointer to storage for left-over from one read call to the next.
   @param fp file bits are being read from, opened for reading in binary.
   @return value of the 5-bit code read in, or -1 if we reach the end-of-file,
   before getting all 5 bits.
 */
int read5Bits(BitBuffer *buffer, FILE *fp)
{
  if (buffer->bcount >= FIVE_BITS) {
    int left = buffer->bcount - FIVE_BITS;
    int code = buffer->bits >> left;
    buffer->bits = buffer->bits << (BITS_PER_BYTE - left);
    buffer->bits = buffer->bits >> (BITS_PER_BYTE - left);
    buffer->bcount = left;
    return code;
  }
  int ch = fgetc(fp);
  if (ch == EOF)
    return -1;
  int tmp = buffer->bits;
  tmp = tmp << BITS_PER_BYTE;
  tmp |= ch;
  int left = buffer->bcount + BITS_PER_BYTE - FIVE_BITS;
  int code = tmp >> left;
  buffer->bits = tmp << (BITS_PER_BYTE - left);
  buffer->bits = buffer->bits >> (BITS_PER_BYTE - left);
  buffer->bcount = left;
  return code;
}

/**
   Just like read5bits() but reading 8 bits at a time.
   @param buffer pointer to storage for left-over from one read call to the next.
   @param fp file bits are being read from, opened for reading in binary.
   @return value of the 8-bit code read in, or -1 if we can't read 8 bits.
 */
int read8Bits(BitBuffer *buffer, FILE *fp)
{
  if (buffer->bcount == BITS_PER_BYTE) {
    int code = buffer->bits;
    buffer->bits = 0;
    buffer->bcount = 0;
    return code;
  }
  int ch = fgetc(fp);
  if (ch == EOF)
    return -1;
  int tmp = buffer->bits;
  tmp = tmp << BITS_PER_BYTE;
  tmp |= ch;
  int left = buffer->bcount;
  int code = tmp >> left;
  buffer->bits = tmp << (BITS_PER_BYTE - left);
  buffer->bits = buffer->bits >> (BITS_PER_BYTE - left);
  buffer->bcount = left;
  return code;
}
