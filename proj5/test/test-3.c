/** Unit tests for edits to the document, saving the updated document
    and undoing the edits. */

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
  copyFile( "original-3.txt", "test.txt" );
  
  // Load it as a document.
  Document *doc = loadDocument( "test.txt" );

  // Make sure the original file looks like we expect.
  checkLine( doc, 0, "This is a test", __LINE__ );
  checkLine( doc, 1, "file we're going", __LINE__ );
  checkLine( doc, 2, "to use for editing.", __LINE__ );

  // Insert a character at the start and end of a line.
  Edit *ed1 = makeInsert( doc, 'x' );
  ed1->apply( ed1, doc );
  checkLine( doc, 0, "xThis is a test", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 1, __LINE__, "Updated cursor position" );

  for ( int i = 0; i < 14; i++ )
    moveCursor( doc, CursorRight );

  Edit *ed2 = makeInsert( doc, '-' );
  ed2->apply( ed2, doc );
  checkLine( doc, 0, "xThis is a test-", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 16, __LINE__, "Updated cursor position" );

  // Break the line in the middle
  for ( int i = 0; i < 8; i++ )
    moveCursor( doc, CursorLeft );
  Edit *ed3 = makeInsert( doc, '\n' );
  ed3->apply( ed3, doc );
  checkLine( doc, 0, "xThis is", __LINE__ );
  checkLine( doc, 1, " a test-", __LINE__ );
  test( doc->cRow == 1 && doc->cCol == 0, __LINE__, "Updated cursor position" );

  // Can't backspace from the start of the file.
  moveCursor( doc, CursorUp );
  Edit *ed4 = makeDelete( doc );
  test( ! ed4, __LINE__, "Invalid backspace edit" );

  // Delete characters at the start and end of a line.
  moveCursor( doc, CursorDown );
  moveCursor( doc, CursorRight );
  ed4 = makeDelete( doc );
  ed4->apply( ed4, doc );
  checkLine( doc, 1, "a test-", __LINE__ );
  test( doc->cRow == 1 && doc->cCol == 0, __LINE__, "Updated cursor position" );

  moveCursor( doc, CursorDown );
  for ( int i = 0; i < 16; i++ )
    moveCursor( doc, CursorRight );
  Edit *ed5 = makeDelete( doc );
  ed5->apply( ed5, doc );
  checkLine( doc, 2, "file we're goin", __LINE__ );
  test( doc->cRow == 2 && doc->cCol == 15, __LINE__, "Updated cursor position" );

  // Delete the linebreak between two lines.
  for ( int i = 0; i < 15; i++ )
    moveCursor( doc, CursorLeft );
  moveCursor( doc, CursorDown );

  Edit *ed6 = makeDelete( doc );
  ed6->apply( ed6, doc );
  checkLine( doc, 2, "file we're gointo use for editing.", __LINE__ );
  test( doc->cRow == 2 && doc->cCol == 15, __LINE__, "Updated cursor position" );

  // Save the document and make sure it looks like it's supposed to.
  test( saveDocument( doc, "output.txt" ), __LINE__, "Save successful" );

  test( matchFiles( "output.txt", "expected-3.txt" ), __LINE__,
        "Save file matches expected" );

  // Undo all these edits in stack order.
  ed6->undo( ed6, doc );
  checkLine( doc, 2, "file we're goin", __LINE__ );
  checkLine( doc, 3, "to use for editing.", __LINE__ );
  test( doc->cRow == 3 && doc->cCol == 0, __LINE__, "Updated cursor position" );
  ed6->destroy( ed6 );
  
  ed5->undo( ed5, doc );
  checkLine( doc, 2, "file we're going", __LINE__ );
  test( doc->cRow == 2 && doc->cCol == 16, __LINE__, "Updated cursor position" );
  ed5->destroy( ed5 );
  
  ed4->undo( ed4, doc );
  checkLine( doc, 1, " a test-", __LINE__ );
  test( doc->cRow == 1 && doc->cCol == 1, __LINE__, "Updated cursor position" );
  ed4->destroy( ed4 );
  
  ed3->undo( ed3, doc );
  checkLine( doc, 0, "xThis is a test-", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 8, __LINE__, "Updated cursor position" );
  ed3->destroy( ed3 );
  
  ed2->undo( ed2, doc );
  checkLine( doc, 0, "xThis is a test", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 15, __LINE__, "Updated cursor position" );
  ed2->destroy( ed2 );
  
  ed1->undo( ed1, doc );
  checkLine( doc, 0, "This is a test", __LINE__ );
  test( doc->cRow == 0 && doc->cCol == 0, __LINE__, "Updated cursor position" );
  ed1->destroy( ed1 );
  
  freeDocument( doc );

  printf( "Passed %d tests (of %d attempted)\n", passed, attempted );

  // Exit successfully if we passed all tests.
  exit( passed == attempted ? EXIT_SUCCESS : EXIT_FAILURE );
}
