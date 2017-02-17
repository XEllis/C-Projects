#include <stdbool.h>

// This trick will let us define the length of a winning run,
// when we compile the program, if we want to.
#ifndef RUNLEN
/** Number of markers in a row required for a win. */
#define RUNLEN 4
#endif

/** Indicate game status: one player gets number of markers in a row required for a win. */
#define WON 1

/** Indicate game status: the board fills up without any player winning. */
#define FULL 2

/** Indicate game status: other situations. */
#define OTHERS 0

/** Remainder of column number divided by 10 would be low-order digit for the column. */
#define DIGITS 10

/** The maximum number of characters players are expected to enter as their move. */
#define MOVE 2

/**
   Print the given board (of the given rows/cols size) to standard output.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
 */
void printBoard(int rows, int cols, char board[rows][cols]);

/**
   Set the board to its initial state, filled with spaces.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
 */
void clearBoard(int rows, int cols, char board[rows][cols]);

/**
   Checks to see if the game is over.
   If one of the players has won, it returns 1.
   If the game board is full but nobody won, it returns 2.
   Otherwise, it returns 0.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
   @return the game status.
 */
int gameStatus(int rows, int cols, char board[rows][cols]);

/**
   Return true if there's a winning sequence of markers starting at the given board location,
   startRow, startCol location, either a sequence of X characters or O characters.
   The dRow and dCol parameters indicate what direction to look,
   with dRow giving change-in-row for each step and dCol giving the change-in-column.
   For example, a dRow of -1 and a dCol of 1 would look for a sequence of markers,
   starting from the given start location and diagonally up to the right.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
   @param startRow row start position to look for a win.
   @param startCol column start position to look for a win.
   @param dRow direction to move vertically looking for a win.
   @param dCol direction to move horizontally looking for a win.
   @return true if there's a win in the given board location.
 */
bool winner(int rows, int cols, char board[rows][cols],
            int startRow, int startCol, int dRow, int dCol);

/**
   Prompt the users for their next move.
   It reads a column number from the player.
   If the column is between 1 and the width of the board, and it's not already full,
   this program will make the requested move (adding an X or O to the board) and return.
   If the move is invalid (e.g., not a number, out of range or a column that's already full),
   the program will print "Invalid move" to standard output and prompt the user again for a move.
   If the program reaches the end-of-file on standard input,
   it will terminate the program without returning.
   @param player character X or O, indicating which player has the next move.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
 */
void makeMove(char player, int rows, int cols, char board[rows][cols]);
