/**
   @file codes.h
   @author Xiaohui Z Ellis (xzheng6)

   Header file for the codes.c component, with functions to help map between symbols,
   and the codes used to represent them.
 */


/** The number of common symbols. */
#define COMMON 31

/**
   Given the ASCII code for a character,
   this function returns the 5-bit code used to represent it.
   If there's no 5-bit code to represent the given character,
   this function returns the escape code, 31.
   @param ch the given character.
   @return the 5-bit code to represent the character or the escape code.
 */
int symToCode(unsigned char ch);

/**
   Given a 5-bit code, this function returns the ASCII character it represents.
   This function is only defined for codes from 0 up to 30, inclusive.
   @param code the given 5-bit code.
   @return the ASCII character the 5-bit code represents.
 */
int codeToSym(int code);
