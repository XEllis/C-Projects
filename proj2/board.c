/**
   @file board.c
   @author Xiaohui Z Ellis (xzheng6)

   This program defines a number of helper functions for the connect program.
 */

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
   Print the given board (of the given rows/cols size) to standard output.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
 */
void printBoard(int rows, int cols, char board[rows][cols])
{
  printf("\n");
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printf("|%c", board[i][j]);
    }
    printf("|\n");
  }
  for (int j = 0; j < cols; j++) {
    printf("+-");
  }
  printf("+\n");
  for (int j = 0; j < cols; j++) {
    printf(" %d", (j+1)%DIGITS);
  }
  printf("\n");
}

/**
   Set the board to its initial state, filled with spaces.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
 */
void clearBoard(int rows, int cols, char board[rows][cols])
{
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      board[i][j]=' ';
    }
  }
}

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
int gameStatus(int rows, int cols, char board[rows][cols])
{
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (board[i][j] == 'X' || board[i][j] == 'O') {
        for (int m = -1; m <= 1; m++) {
          for (int n = -1; n <= 1; n++) {
            if ((m != 0 || n != 0) && winner(rows, cols, board, i, j, m, n)) {
              return WON;
            }
          }
        }
      }
    }
  }
  int count = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (board[i][j] == ' ') {
        count++;
      }
    }
  }
  if (count == 0) {
    return FULL;
  }
  return OTHERS;
}

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
            int startRow, int startCol, int dRow, int dCol)
{
  // Number of X and O symbols in this sequence of locations.
  int xcount = 0, ocount = 0;
  // Walk down the sequence of board spaces.
  for (int k = 0; k < RUNLEN; k++) {
    // Figure out its row and column index and make sure it's on the board.
    int r = startRow + k * dRow;
    int c = startCol + k * dCol;
    if (r < 0 || r >= rows || c < 0 || c >= cols) {
      return false;
    }
    // Count an X or an O if it's one of those.
    if (board[r][c] == 'X') {
      xcount++;
    }
    else if (board[r][c] == 'O') {
      ocount++;
    }
  }
  // We have a winner if it's all Xs or Os.
  return xcount == RUNLEN || ocount == RUNLEN;
}

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
void makeMove(char player, int rows, int cols, char board[rows][cols])
{
  printf("%c move> ", player);
  char move[MOVE+1] = "";
  int move_col = 0;
  while (scanf("%3s", move) == 1) {
    if (move[MOVE]) {
      printf("Invalid move\n");
      int ch = getchar();
      while (ch != '\n') {
        ch = getchar();
      }
    }
    else if (sscanf(move, "%d", &move_col) != 1) {
      printf("Invalid move\n");
      int ch = getchar();
      while (ch != '\n') {
        ch = getchar();
      }
    }
    else if (strlen(move) == MOVE && (move[MOVE-1] < '0' || move[MOVE-1] > '9')) {
      printf("Invalid move\n");
      int ch = getchar();
      while (ch != '\n') {
        ch = getchar();
      }
    }
    else if (move_col < 1 || move_col > cols) {
      printf("Invalid move\n");
      int ch = getchar();
      while (ch != '\n') {
        ch = getchar();
      }
    }
    else if (board[0][move_col-1] != ' ') {
      printf("Invalid move\n");
      int ch = getchar();
      while (ch != '\n') {
        ch = getchar();
      }
    }
    else {
      for (int i = rows-1; i >= 0; i--) {
        if (board[i][move_col-1] == ' ') {
          board[i][move_col-1] = player;
          break;
        }
      }
      return;
    }
    for (int i = 0; i < MOVE+1; i++) {
      move[i] = '\0';
    }
    move_col = 0;
    printf("%c move> ", player);
  }
  exit(EXIT_SUCCESS);
}
