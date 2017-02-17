/** Unit tests for loading and representing a document.
    I'm not using assert because I want to do things like
    count the number of successful tests. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "document.h"

/** Number of tests attempted. */
static int attempted = 0;

/** Number of tests passed. */
static int passed = 0;

/** Report on a test that passed / failed */
void test( bool cond, int codeLine, char *msg )
{
  if ( cond ) {
    printf( "File %s(%d), test %d PASSED (%s)\n", __FILE__,
            codeLine, attempted, msg );
    passed += 1;
  } else {
    printf( "File %s(%d), test %d **FAILED** (%s)\n", __FILE__,
            codeLine, attempted, msg );
  }
  attempted += 1;
}

/** Make sure line lno in the given document looks just like the given
    target string, and that other fields fields make sense. */
void checkLine( Document *doc, int lno, char *target, int codeLine )
{
  test( doc->lines[ lno ]->len == strlen( target ), codeLine,
        "Line len field is correct." );
  test( doc->lines[ lno ]->cap > doc->lines[ lno ]->len, codeLine,
        "Line has sufficient capacity." );
  test( strcmp( doc->lines[ lno ]->text, target ) == 0, codeLine,
        "Line contains the right text." );
}

/** Copy src file to dest.  I'd rather do this in the shell, but I'd
    like these tests to be self-contained. */
void copyFile( char *src, char *dest )
{
  FILE *fps = fopen( src, "r" );
  if ( !fps )
    perror( src ), exit( EXIT_FAILURE );

  FILE *fpd = fopen( dest, "w" );
  if ( !fpd )
    perror( dest ), exit( EXIT_FAILURE );

  int ch;
  while ( ( ch = fgetc( fps ) ) != EOF )
    fputc( ch, fpd );

  fclose( fps );
  fclose( fpd );
}

int main()
{
  // Get a clean copy of the test file.
  copyFile( "original-2.txt", "test.txt" );
  
  // Load it as a document.
  Document *doc = loadDocument( "test.txt" );

  // Check document size and spot-check some lines.
  test( doc->len == 14, __LINE__, "Document has the right number of lines" );
  test( doc->cap >= doc->len, __LINE__, "Document sufficient line capacity" );

  checkLine( doc, 2, "", __LINE__ );
  checkLine( doc, 4, "It will be good for testing cursor movement.", __LINE__ );
  checkLine( doc, 13, "left.", __LINE__ );

  test( doc->cRow == 0 && doc->cCol == 0, __LINE__, "Initial cursor position" );
  
  // Move around in a little square.
  test( moveCursor( doc, CursorRight ), __LINE__, "Cursor right" );
  test( doc->cRow == 0 && doc->cCol == 1, __LINE__, "Updated cursor position" );

  test( moveCursor( doc, CursorDown ), __LINE__, "Cursor down" );
  test( doc->cRow == 1 && doc->cCol == 1, __LINE__, "Updated cursor position" );

  test( moveCursor( doc, CursorLeft ), __LINE__, "Cursor left" );
  test( doc->cRow == 1 && doc->cCol == 0, __LINE__, "Updated cursor position" );

  test( moveCursor( doc, CursorUp ), __LINE__, "Cursor up" );
  test( doc->cRow == 0 && doc->cCol == 0, __LINE__, "Updated cursor position" );

  // Try to move too far up and left.
  test( ! moveCursor( doc, CursorUp ), __LINE__, "Invalid cursor up" );
  test( doc->cRow == 0 && doc->cCol == 0, __LINE__, "Unchanged cursor position" );

  test( ! moveCursor( doc, CursorLeft ), __LINE__, "Invalid cursor up" );
  test( doc->cRow == 0 && doc->cCol == 0, __LINE__, "Unchanged cursor position" );

  // Scoot down to the end of a short line.
  for ( int i = 0; i < 9; i++ )
    moveCursor( doc, CursorDown );
  moveCursor( doc, CursorRight );
  test( doc->cRow == 9 && doc->cCol == 1, __LINE__, "Updated cursor position" );

  test( ! moveCursor( doc, CursorRight ), __LINE__, "Invalid cursor right" );
  test( doc->cRow == 9 && doc->cCol == 1, __LINE__, "Unchanged cursor position" );
 
  // Scoot down to the bottom line
  for ( int i = 0; i < 4; i++ )
    moveCursor( doc, CursorDown );
  test( doc->cRow == 13 && doc->cCol == 1, __LINE__, "Updated cursor position" );

  test( ! moveCursor( doc, CursorDown ), __LINE__, "Invalid cursor down" );
  test( doc->cRow == 13 && doc->cCol == 1, __LINE__, "Unchanged cursor position" );

  // Move to the end of this longer line.
  for ( int i = 0; i < 4; i++ )
    moveCursor( doc, CursorRight );
  test( doc->cRow == 13 && doc->cCol == 5, __LINE__, "Updated cursor position" );

  // Move up, to the end of a shorter line.
  test( moveCursor( doc, CursorUp ), __LINE__, "Cursor up to a shorter line" );
  test( doc->cRow == 12 && doc->cCol == 3, __LINE__, "At the end of a shorter line" );

  // Move up to the end of a longer line.
  for ( int i = 0; i < 2; i++ )
    moveCursor( doc, CursorUp );
  for ( int i = 0; i < 37; i++ )
    moveCursor( doc, CursorRight );
  test( doc->cRow == 10 && doc->cCol == 39, __LINE__, "Updated cursor position" );

  // Move down to a shorter line.
  test( moveCursor( doc, CursorDown ), __LINE__, "Cursor down to a shorter line" );
  test( doc->cRow == 11 && doc->cCol == 2, __LINE__, "Updated cursor position" );

  freeDocument( doc );

  printf( "Passed %d tests (of %d attempted)\n", passed, attempted );

  // Exit successfully if we passed all tests.
  exit( passed == attempted ? EXIT_SUCCESS : EXIT_FAILURE );
}
