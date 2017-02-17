#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "document.h"
#include "edit.h"

/** Macro to compute min. */
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/** Macro to compute max. */
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/** Print a usage message and exit unsuccessfully.
*/
static void usage()
{
  fprintf( stderr, "usage: sled <file>\n" );
  exit( EXIT_FAILURE );
}

/** Starting point for the program, expects a filename as the only command-line
    argument.
    @param argc Number of command-line arguments.
    @param argv List of command-line arguments.
*/
int main( int argc, char *argv[] )
{
  // Load the document listed on the command line.
  if ( argc != 2 )
    usage();
  Document *doc = loadDocument( argv[ 1 ] );

  // Make a history, for undo/redo.
  History hist;
  initHistory( &hist );

  // Initialize ncurses.
  initscr();
  clear();
  noecho();
  cbreak();
  keypad( stdscr, true );

  // Position of the upper-left corner of the view.
  int viewRow = 0, viewCol = 0;

  bool running = true;
  while ( running ) {
    // Get the size of the window and make sure the cursor is in view.
    int rows, cols;
    getmaxyx( stdscr, rows, cols );

    viewRow = MIN( viewRow, doc->cRow );
    viewRow = MAX( viewRow, doc->cRow - rows + 1 );

    viewCol = MIN( viewCol, doc->cCol );
    viewCol = MAX( viewCol, doc->cCol - cols + 1 );

    // Paint the current state of the document.
    for ( int i = viewRow; i < viewRow + rows && i < doc->len; i++ )
      mvprintw( i - viewRow, 0, "%-*s", cols, doc->lines[ i ]->text + viewCol );
    // Clear the first row past the end of the document, if it exists.
    if ( doc->len - viewRow < rows )
      mvprintw( doc->len - viewRow, 0, "%*s", cols, "" );
    // Show the cursor where it's supposed to be.
    move( doc->cRow - viewRow, doc->cCol - viewCol );
    refresh();
    
    int ch = getch();
    char *kname = keyname( ch );

    // Respond to key events we understand.
    if ( ( ch >= ' ' && ch <= '~' ) || ch == '\n' ) {
      // It's a symbol to insert.
      Edit *ed = makeInsert( doc, ch );
      applyEdit( &hist, doc, ed );
    } else if ( strcmp( kname, "KEY_BACKSPACE" ) == 0 ) {
      // Remove the previous symbol.
      Edit *ed = makeDelete( doc );
      if ( ed )
        applyEdit( &hist, doc, ed );
      else
        beep();
    } else if ( strcmp( kname, "KEY_UP" ) == 0 ) {
      if ( ! moveCursor( doc, CursorUp ) )
        beep();
    } else if ( strcmp( kname, "KEY_DOWN" ) == 0 ) {
      if ( ! moveCursor( doc, CursorDown ) )
        beep();
    } else if ( strcmp( kname, "KEY_LEFT" ) == 0 ) {
      if ( ! moveCursor( doc, CursorLeft ) )
        beep();
    } else if ( strcmp( kname, "KEY_RIGHT" ) == 0 ) {
      if ( ! moveCursor( doc, CursorRight ) )
        beep();
    } else if ( strcmp( kname, "^S" ) == 0 ) {
      // Save a copy of the document.
      if ( ! saveDocument( doc, argv[ 1 ] ) )
        beep();
    } else if ( strcmp( kname, "^U" ) == 0 ) {
      // Try to undo the previous edit
      if ( ! undoEdit( &hist, doc ) )
        beep();
    } else if ( strcmp( kname, "^R" ) == 0 ) {
      // Try to redo the previous edit
      if ( ! redoEdit( &hist, doc ) )
        beep();
    } else if ( strcmp( kname, "^X" ) == 0 ) {
      // Exit when we get to the top of the loop.
      running = false;
    }
  }

  // Shut down curses and free the document.
  endwin();
  freeDocument( doc );

  exit( EXIT_SUCCESS );
}
