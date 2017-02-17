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
  copyFile( "original-1.txt", "test.txt" );
  
  // Load it as a document.
  Document *doc = loadDocument( "test.txt" );

  test( doc->len == 5, __LINE__, "Document has the right number of lines" );
  test( doc->cap >= doc->len, __LINE__, "Document sufficient line capacity" );

  checkLine( doc, 0, "This is a", __LINE__ );
  checkLine( doc, 1, "short text", __LINE__  );
  checkLine( doc, 2, "file, for", __LINE__  );
  checkLine( doc, 3, "the first", __LINE__  );
  checkLine( doc, 4, "unit test.", __LINE__  );

  printf( "Passed %d tests (of %d attempted)\n", passed, attempted );

  freeDocument( doc );
  
  // Exit successfully if we passed all tests.
  exit( passed == attempted ? EXIT_SUCCESS : EXIT_FAILURE );
}
