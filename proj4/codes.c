/**
   @file codes.c
   @author Xiaohui Z Ellis (xzheng6)

   This program defines functions to help map between symbols and the codes,
   used to represent them.
 */

#include "codes.h"

/**
   The character array is to help map between symbols and the codes used to represent them.
   The array shows the symbol, and the array index shows the code used to represent it.
 */
static char symTbl[COMMON] = {'e', 't', 'a', 'i', '"', 'n', ' ', 's', 'o', 'l',
                              'r', 'd', 'c', '>', '<', '/', 'p', 'm', '-', 'u',
                              '.', 'h', 'f', '_', '=', 'g', ':', 'b', '0', 'y',
                              '\n'};

/**
   Given the ASCII code for a character,
   this function returns the 5-bit code used to represent it.
   If there's no 5-bit code to represent the given character,
   this function returns the escape code, 31.
   @param ch the given character.
   @return the 5-bit code to represent the character or the escape code.
 */
int symToCode(unsigned char ch)
{
  for (int i = 0; i < COMMON; i++) {
    if (ch == symTbl[i])
      return i;
  }
  return COMMON;
}

/**
   Given a 5-bit code, this function returns the ASCII character it represents.
   This function is only defined for codes from 0 up to 30, inclusive.
   @param code the given 5-bit code.
   @return the ASCII character the 5-bit code represents.
 */
int codeToSym(int code)
{
  return symTbl[code];
}
