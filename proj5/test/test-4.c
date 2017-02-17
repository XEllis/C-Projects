/** Unit tests for the command history, with a stack of commands that can
    be undone and a stack that can be redone. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "document.h"
#include "edit.h"

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

/** Return true if the two given files are identical, Implemented here
    so the tests will be self-contained. */
bool matchFiles( char *file1, char *file2 )
{
  bool match = true;

  FILE *fp1 = fopen( file1, "r" );
  if ( !fp1 )
    perror( file1 ), exit( EXIT_FAILURE );

  FILE *fp2 = fopen( file2, "r" );
  if ( !fp2 )
    perror( file2 ), exit( EXIT_FAILURE );

  int ch;
  while ( match && ( ch = fgetc( fp1 ) ) != EOF )
    if ( ch != fgetc( fp2 ) )
      match = false;
      
  if ( match && fgetc( fp2 ) != EOF )
    match = false;

  fclose( fp1 );
  fclose( fp2 );

  return match;
}

int main()
{
  // Get a clean copy of the test file.
  copyFile( "original-4.txt", "test.txt" );
  
  // Load it as a document.
  Document *doc = loadDocument( "test.txt" );

  // Create a new command history.
  History hist;
  initHistory( &hist );
  
  // Same edits as test 3, but saving them to the history this time.

  // Insert a character at the start and end of a line.
  Edit *ed = makeInsert( doc, 'x' );
  applyEdit( &hist, doc, ed );

  checkLine( doc, 0, "xThis is a test", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 1, __LINE__, "Updated cursor position" );

  for ( int i = 0; i < 14; i++ )
    moveCursor( doc, CursorRight );

  ed = makeInsert( doc, '-' );
  applyEdit( &hist, doc, ed );
  checkLine( doc, 0, "xThis is a test-", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 16, __LINE__, "Updated cursor position" );

  // Break the line in the middle
  for ( int i = 0; i < 8; i++ )
    moveCursor( doc, CursorLeft );
  ed = makeInsert( doc, '\n' );
  applyEdit( &hist, doc, ed );
  checkLine( doc, 0, "xThis is", __LINE__ );
  checkLine( doc, 1, " a test-", __LINE__ );
  test( doc->cRow == 1 && doc->cCol == 0, __LINE__, "Updated cursor position" );

  // Delete characters at the start and end of a line.
  moveCursor( doc, CursorRight );
  ed = makeDelete( doc );
  applyEdit( &hist, doc, ed );
  checkLine( doc, 1, "a test-", __LINE__ );
  test( doc->cRow == 1 && doc->cCol == 0, __LINE__, "Updated cursor position" );

  moveCursor( doc, CursorDown );
  for ( int i = 0; i < 16; i++ )
    moveCursor( doc, CursorRight );
  ed = makeDelete( doc );
  applyEdit( &hist, doc, ed );
  checkLine( doc, 2, "file we're goin", __LINE__ );
  test( doc->cRow == 2 && doc->cCol == 15, __LINE__, "Updated cursor position" );

  // Delete the linebreak between two lines.
  for ( int i = 0; i < 15; i++ )
    moveCursor( doc, CursorLeft );
  moveCursor( doc, CursorDown );

  ed = makeDelete( doc );
  applyEdit( &hist, doc, ed );
  checkLine( doc, 2, "file we're gointo use for editing.", __LINE__ );
  test( doc->cRow == 2 && doc->cCol == 15, __LINE__, "Updated cursor position" );

  // Undo these edits via the history.
  test( undoEdit( &hist, doc ), __LINE__, "Successful undo" );
  checkLine( doc, 2, "file we're goin", __LINE__ );
  checkLine( doc, 3, "to use for editing.", __LINE__ );
  test( doc->cRow == 3 && doc->cCol == 0, __LINE__, "Updated cursor position" );
  
  test( undoEdit( &hist, doc ), __LINE__, "Successful undo" );
  checkLine( doc, 2, "file we're going", __LINE__ );
  test( doc->cRow == 2 && doc->cCol == 16, __LINE__, "Updated cursor position" );
  
  test( undoEdit( &hist, doc ), __LINE__, "Successful undo" );
  checkLine( doc, 1, " a test-", __LINE__ );
  test( doc->cRow == 1 && doc->cCol == 1, __LINE__, "Updated cursor position" );
  
  test( undoEdit( &hist, doc ), __LINE__, "Successful undo" );
  checkLine( doc, 0, "xThis is a test-", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 8, __LINE__, "Updated cursor position" );
  
  test( undoEdit( &hist, doc ), __LINE__, "Successful undo" );
  checkLine( doc, 0, "xThis is a test", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 15, __LINE__, "Updated cursor position" );
  
  test( undoEdit( &hist, doc ), __LINE__, "Successful undo" );
  checkLine( doc, 0, "This is a test", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 0, __LINE__, "Updated cursor position" );
  
  // Save the document and make sure it looks like it's supposed to.  Should
  // look just like the original.
  test( saveDocument( doc, "output.txt" ), __LINE__, "Save successful" );

  test( matchFiles( "output.txt", "expected-4.txt" ), __LINE__,
        "Save file matches expected" );

  // Redo all the edits we just undid.
  test( redoEdit( &hist, doc ), __LINE__, "Successful redo" );
  checkLine( doc, 0, "xThis is a test", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 1, __LINE__, "Updated cursor position" );
  
  test( redoEdit( &hist, doc ), __LINE__, "Successful redo" );
  checkLine( doc, 0, "xThis is a test-", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 16, __LINE__, "Updated cursor position" );

  test( redoEdit( &hist, doc ), __LINE__, "Successful redo" );
  checkLine( doc, 0, "xThis is", __LINE__ );
  checkLine( doc, 1, " a test-", __LINE__ );
  test( doc->cRow == 1 && doc->cCol == 0, __LINE__, "Updated cursor position" );

  test( redoEdit( &hist, doc ), __LINE__, "Successful redo" );
  checkLine( doc, 1, "a test-", __LINE__ );
  test( doc->cRow == 1 && doc->cCol == 0, __LINE__, "Updated cursor position" );

  test( redoEdit( &hist, doc ), __LINE__, "Successful redo" );
  checkLine( doc, 2, "file we're goin", __LINE__ );
  test( doc->cRow == 2 && doc->cCol == 15, __LINE__, "Updated cursor position" );

  test( redoEdit( &hist, doc ), __LINE__, "Successful redo" );
  checkLine( doc, 2, "file we're gointo use for editing.", __LINE__ );
  test( doc->cRow == 2 && doc->cCol == 15, __LINE__, "Updated cursor position" );

  // Redo stack should be empty.
  test( ! redoEdit( &hist, doc ), __LINE__, "Empty redo stack." );

  // Add a new blank line to the end of the document.
  for ( int i = 0; i < 19; i++ )
    moveCursor( doc, CursorRight );
  ed = makeInsert( doc, '\n' );
  applyEdit( &hist, doc, ed );
  checkLine( doc, 3, "", __LINE__ );
  test( doc->cRow == 3 && doc->cCol == 0, __LINE__, "Updated cursor position" );

  // Insert enough characters to exceed the capacity of the undo stack.
  for ( int i = 0; i < HIST_SIZE + 1; i++ ) {
    ed = makeInsert( doc, 'x' );
    applyEdit( &hist, doc, ed );
  }
  checkLine( doc, 3, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", __LINE__ );
  test( doc->cRow == 3 && doc->cCol == 31, __LINE__, "Updated cursor position" );

  // Undo all the operations in the undo stack.
  for ( int i = 0; i < HIST_SIZE - 1; i++ )
    undoEdit( &hist, doc );
  test( undoEdit( &hist, doc ), __LINE__, "Last undo on undo stack" );

  // No more undos.
  test( ! undoEdit( &hist, doc ), __LINE__, "Empty undo stack" );
  checkLine( doc, 3, "x", __LINE__ );
  test( doc->cRow == 3 && doc->cCol == 1, __LINE__, "Updated cursor position" );

  // Redo some of these undone edits.
  for ( int i = 0; i < 8; i++ )
    redoEdit( &hist, doc );
  test( redoEdit( &hist, doc ), __LINE__, "Successful redo" );
  checkLine( doc, 3, "xxxxxxxxxx", __LINE__ );
  test( doc->cRow == 3 && doc->cCol == 10, __LINE__, "Updated cursor position" );

  // Make a new edit invalidates the redo stack.
  ed = makeInsert( doc, 'y' );
  applyEdit( &hist, doc, ed );
  
  test( ! redoEdit( &hist, doc ), __LINE__, "Redo stack should be empty." );

  clearHistory( &hist );
  freeDocument( doc );

  printf( "Passed %d tests (of %d attempted)\n", passed, attempted );

  // Exit successfully if we passed all tests.
  exit( passed == attempted ? EXIT_SUCCESS : EXIT_FAILURE );
}
