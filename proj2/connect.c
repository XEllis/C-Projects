/**
   @file connect.c
   @author Xiaohui Z Ellis (xzheng6)

   This program lets you play a generalized game of connect four.
   Two players (or one player and computer) each get to drop markers (X or O),
   into the top of a chosen column in a two­-dimensional game board.
   The marker drops down the column until it rests at the bottom of the column,
   or on top of a marker that's already been placed.
   Gameplay continues until a player manages to get four of their markers in a row,
   or the board fills up with neither player winning,
   or the program reaches the end-of-file on standard input.
 */

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/** The maximum allowable board size is always 16 larger than the constant RUNLEN. */
#define LARGER 16

/** Exit code for invalid input. */
#define EXIT_UNSUCCESS 1

/**
   This function is define for the auto-play feature. It is similar to makeMove.
   It also updates the latest move for player X.
   Prompt the users for their next move during autoplay for the O player.
   It reads a column number from the player.
   If the column is between 1 and the width of the board, and it's not already full,
   this program will make the requested move (adding an X to the board) and return.
   If the move is invalid (e.g., not a number, out of range or a column that's already full),
   the program will print "Invalid move" to standard output and prompt the user again for a move.
   If the program reaches the end-of-file on standard input,
   it will terminate the program without returning.
   @param player character X, indicating player X has the next move.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
   @param ptr_xi a pointer to the latest row index for player X.
   @param ptr_xj a pointer to the latest column index for player X.
 */
void makeXMove(char player, int rows, int cols, char board[rows][cols], int *ptr_xi, int *ptr_xj)
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
          *ptr_xi = i;
          *ptr_xj = move_col-1;
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

/**
   This function is defined for the auto-play feature. It is similar to winner.
   Return true if there's a potential threat of markers starting at the given board location,
   startRow, startCol location, either a sequence of X characters or O characters.
   The dRow and dCol parameters indicate what direction to look,
   with dRow giving change-in-row for each step and dCol giving the change-in-column.
   For example, a dRow of -1 and a dCol of 1 would look for a sequence of markers,
   starting from the given start location and diagonally up to the right.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
   @param startRow row start position to look for a threat.
   @param startCol column start position to look for a threat.
   @param dRow direction to move vertically looking for a threat.
   @param dCol direction to move horizontally looking for a threat.
   @return true if there's a threat in the given board location.
 */
bool threat(int rows, int cols, char board[rows][cols],
            int startRow, int startCol, int dRow, int dCol)
{
  // Number of X and O symbols in this sequence of locations.
  int xcount = 0, ocount = 0;
  int blank = 0;
  // Walk down the sequence of board spaces.
  for (int k = 0; k < RUNLEN; k++) {
    // Figure out its row and column index and make sure it's on the board.
    int r = startRow + k * dRow;
    int c = startCol + k * dCol;
    if (r < 0 || r >= rows || c < 0 || c >= cols) {
      return false;
    }
    if (k < RUNLEN-2) {
      // Count an X or an O if it's one of those.
      if (board[r][c] == 'X') {
        xcount++;
      }
      else if (board[r][c] == 'O') {
        ocount++;
      }
    }
    else {
      if (board[r][c] == ' ') {
        blank++;
      }
    }
  }
  // We have a winner if it's all Xs or Os.
  return xcount+blank == RUNLEN || ocount+blank == RUNLEN;
}

/**
   This function is defined for the auto-play feature.
   Return true if array notcols has a number that is equal to xj+1.
   @param xj column index.
   @param cols number of columns the board has.
   @return true if array notcols has a number that is equal to xj+1.
 */
bool helper(int xj, int cols, int notcols[cols])
{
  for (int j = 0; j < cols; j++) {
    if (xj+1 == notcols[j]) {
      return true;
    }
  }
  return false;
}

/**
   This function is defined for the auto-play feature.
   @param player character O, indicating computer has the next move.
   @param rows number of rows the board has.
   @param cols number of columns the board has.
   @param board the game board.
   @param xi the latest row index for player X.
   @param xj the latest column index for player X.
 */
void makeAutoMove(char player, int rows, int cols, char board[rows][cols], int xi, int xj)
{
  int notcount = 0;
  int bnotcount = 0;
  int notcols[cols];
  int bnotcols[cols];
  for (int k = 0; k < cols; k++) {
    notcols[k] = 0;
    bnotcols[k] = 0;
  }
  // If there is a winning move, the computer will take it.
  for (int j = 0; j < cols; j++) {
    for (int i = rows-1; i >= 0; i--) {
      if (board[i][j] == ' ') {
        board[i][j] = player;
        if (gameStatus(rows, cols, board) == WON) {
          printf("Computer Move %d\n", j+1);
          return;
        }
        board[i][j] = ' ';
        break;
      }
    }
  }
  // If the opponent is going to win, the computer will block it.
  for (int j = 0; j < cols; j++) {
    for (int i = rows-1; i >= 0; i--) {
      if (board[i][j] == ' ') {
        board[i][j] = 'X';
        if (gameStatus(rows, cols, board) == WON) {
          board[i][j] = player;
          printf("Computer Move %d\n", j+1);
          return;
        }
        board[i][j] = ' ';
        break;
      }
    }
  }
  // If the opponent is going to have threat, the computer will prevent it.
  for (int j = 0; j < cols; j++) {
    for (int i = rows-1; i >= 0; i--) {
      if (board[i][j] == ' ') {
        board[i][j] = 'X';
        int twothreat = 0;
        int threatrow = 0;
        int threatcol = j;
        for (int n = 0; n < cols; n++) {
          for (int m = rows-1; m >= 0; m--) {
            if (board[m][n] == ' ') {
              board[m][n] = 'X';
              if (gameStatus(rows, cols, board) == WON) {
                twothreat++;
                if (threatcol == j) {
                  threatrow = m;
                  threatcol = n;
                }
              }
              board[m][n] = ' ';
              break;
            }
          }
        }
        if (twothreat >= 2) {
          board[i][j] = ' ';
          board[threatrow][threatcol] = player;
          printf("Computer Move %d\n", threatcol+1);
          return;
        }
        if (i != 0) {
          board[i][j] = player;
          board[i-1][j] = 'X';
          if (gameStatus(rows, cols, board) == WON) {
            notcols[notcount] = j+1;
            notcount++;
          }
          else {
            int twothreat = 0;
            for (int n = 0; n < cols; n++) {
              for (int m = rows-1; m >= 0; m--) {
                if (board[m][n] == ' ') {
                  board[m][n] = 'X';
                  if (gameStatus(rows, cols, board) == WON) {
                    twothreat++;
                  }
                  board[m][n] = ' ';
                  break;
                }
              }
            }
            if (twothreat >= 2) {
              notcols[notcount] = j+1;
              notcount++;
            }
          }
          board[i][j] = 'X';
          board[i-1][j] = player;
          if (gameStatus(rows, cols, board) == WON) {
            bnotcols[bnotcount] = j+1;
            bnotcount++;
          }
          board[i-1][j] = ' ';
        }
        board[i][j] = ' ';
        break;
      }
    }
  }
  if (threat(rows, cols, board, xi, xj, 0, 1)) {
    if (xj != 0 && !helper(xj-1, cols, notcols) &&
        !helper(xj-1, cols, bnotcols) && board[xi][xj-1] == ' ') {
      if (xi == rows-1 || (xi != rows-1 && board[xi+1][xj-1] != ' ')) {
        board[xi][xj-1] = player;
        printf("Computer Move %d\n", xj);
        return;
      }
    }
  }
  if (threat(rows, cols, board, xi, xj, 0, -1)) {
    if (xj != cols-1 && !helper(xj+1, cols, notcols) &&
        !helper(xj+1, cols, bnotcols) && board[xi][xj+1] == ' ') {
      if (xi == rows-1 || (xi != rows-1 && board[xi+1][xj+1] != ' ')) {
        board[xi][xj+1] = player;
        printf("Computer Move %d\n", xj+2);
        return;
      }
    }
  }
  if (threat(rows, cols, board, xi, xj, -1, 1)) {
    if (!helper(xj+2, cols, notcols) &&
        !helper(xj+2, cols, bnotcols) && board[xi-1][xj+2] != ' ') {
      board[xi-2][xj+2] = player;
      printf("Computer Move %d\n", xj+2+1);
      return;
    }
  }
  if (threat(rows, cols, board, xi, xj, -1, -1)) {
    if (!helper(xj-2, cols, notcols) &&
        !helper(xj-2, cols, bnotcols) && board[xi-1][xj-2] != ' ') {
      board[xi-2][xj-2] = player;
      printf("Computer Move %d\n", xj-1);
      return;
    }
  }
  int r = rand()%2;
  if (r == 0 && xj != 0 && !helper(xj-1, cols, notcols) &&
      !helper(xj-1, cols, bnotcols) && board[xi][xj-1] == ' ') {
    if (xi == rows-1 || (xi != rows-1 && board[xi+1][xj-1] != ' ')) {
      board[xi][xj-1] = player;
      printf("Computer Move %d\n", xj);
      return;
    }
  }
  if (r == 1 && xj != cols-1 && !helper(xj+1, cols, notcols) &&
      !helper(xj+1, cols, bnotcols) && board[xi][xj+1] == ' ') {
    if (xi == rows-1 || (xi != rows-1 && board[xi+1][xj+1] != ' ')) {
      board[xi][xj+1] = player;
      printf("Computer Move %d\n", xj+2);
      return;
    }
  }
  if (!helper(xj, cols, notcols) && !helper(xj, cols, bnotcols)) {
    if (xi > 0) {
      board[xi-1][xj] = player;
      printf("Computer Move %d\n", xj+1);
      return;
    }
  }
  for (int j = 0; j < cols; j++) {
    if (!helper(j, cols, notcols) && !helper(j, cols, bnotcols)) {
      for (int i = rows-1; i >=0 ; i--) {
        if (board[i][j] == ' ') {
          board[i][j] = player;
          printf("Computer Move %d\n", j+1);
          return;
        }
      }
    }
  }
  for (int k = 0; k < cols; k++) {
    int j = bnotcols[k]-1;
    if (j >= 0) {
      for (int i = rows-1; i >=0 ; i--) {
        if (board[i][j] == ' ') {
          board[i][j] = player;
          printf("Computer Move %d\n", j+1);
          return;
        }
      }
    }
  }
  for (int k = 0; k < cols; k++) {
    int j = notcols[k]-1;
    if (j >= 0) {
      for (int i = rows-1; i >=0 ; i--) {
        if (board[i][j] == ' ') {
          board[i][j] = player;
          printf("Computer Move %d\n", j+1);
          return;
        }
      }
    }
  }
}

/**
   Starting point for the program,
   if the program is run with a -a as the only command line argument,
   the program would automatically choose good move for the O player.
   Otherwise, two players each get to drop markers (X or O),
   into the top of a chosen column in a two­-dimensional game board.
   The marker drops down the column until it rests at the bottom of the column,
   or on top of a marker that's already been placed.
   Gameplay continues until a player manages to get four of their markers in a row,
   or the board fills up with neither player winning,
   or the program reaches the end-of-file on standard input.
   When the program starts up, it would ask for the size of the board,
   measured in rows and columns. The user is expected to enter a pair of integer,
   each between 4 and 20, inclusive. If something else is entered,
   the program would exit with a status of 1.
   After the user enters the board size, the program will display the board,
   and then prompt alternately between the X and the O player for a move.
   As their move, players are expected to enter the number of the column,
   they wish to drop a marker into. If the player enters invalid input,
   or if they enter the number of a column that is already full,
   the program would ignore that input line and prompt again for a valid move.
   After the player selects a valid move, the program will put their marker,
   into the selected column, at the lowest unoccupied space.
   It will display an updated picture of the board,
   and then prompt the other player for a move.
   This continues until one of three things happen, and the program terminates successfully.
   1. The program reaches the end-­of-­file on standard input.
   2. A player gets four in a row.
   3. The board fills up without any player winning.
   @param argc the number of command-line arguments.
   @param argv command-line arguments.
   @return exit status for the program
 */
int main(int argc, char *argv[])
{
  int rows = 0;
  int cols = 0;
  char rinput[MOVE+1] = "";
  char cinput[MOVE+1] = "";
  printf("Rows and cols> ");
  if (scanf("%3s%3s", rinput, cinput) != 2) {
    fprintf(stderr, "Invalid board size\n");
    exit(EXIT_UNSUCCESS);
  }
  if (rinput[MOVE] || cinput[MOVE]) {
    fprintf(stderr, "Invalid board size\n");
    exit(EXIT_UNSUCCESS);
  }
  if (sscanf(rinput, "%d", &rows) != 1 ||
      (strlen(rinput) == MOVE && (rinput[MOVE-1] < '0' || rinput[MOVE-1] > '9'))) {
    fprintf(stderr, "Invalid board size\n");
    exit(EXIT_UNSUCCESS);
  }
  if (sscanf(cinput, "%d", &cols) != 1 ||
      (strlen(cinput) == MOVE && (cinput[MOVE-1] < '0' || cinput[MOVE-1] > '9'))) {
    fprintf(stderr, "Invalid board size\n");
    exit(EXIT_UNSUCCESS);
  }
  if (rows < RUNLEN || rows > RUNLEN+LARGER || cols < RUNLEN || cols > RUNLEN+LARGER ) {
    fprintf(stderr, "Invalid board size\n");
    exit(EXIT_UNSUCCESS);
  }
  char board[rows][cols];
  clearBoard(rows, cols, board);
  printBoard(rows, cols, board);
  int status = gameStatus(rows, cols, board);
  char xplayer = 'X';
  char oplayer = 'O';
  if (argc == 2 && strcmp(argv[1], "-a") == 0) {
    int xi = 0;
    int *ptr_xi = &xi;
    int xj = 0;
    int *ptr_xj = &xj;
    while (!status) {
      makeXMove(xplayer, rows, cols, board, ptr_xi, ptr_xj);
      printBoard(rows, cols, board);
      status = gameStatus(rows, cols, board);
      if (!status) {
        makeAutoMove(oplayer, rows, cols, board, xi, xj);
        printBoard(rows, cols, board);
        status = gameStatus(rows, cols, board);
        if (status == WON) {
          printf("Player %c wins\n", oplayer);
          return EXIT_SUCCESS;
        }
        else if (status == FULL) {
          printf("Stalemate\n");
          return EXIT_SUCCESS;
        }
      }
      else if (status == WON) {
        printf("Player %c wins\n", xplayer);
        return EXIT_SUCCESS;
      }
      else {
        printf("Stalemate\n");
        return EXIT_SUCCESS;
      }
    }
  }
  else {
    while (!status) {
      makeMove(xplayer, rows, cols, board);
      printBoard(rows, cols, board);
      status = gameStatus(rows, cols, board);
      if (!status) {
        makeMove(oplayer, rows, cols, board);
        printBoard(rows, cols, board);
        status = gameStatus(rows, cols, board);
        if (status == WON) {
          printf("Player %c wins\n", oplayer);
          return EXIT_SUCCESS;
        }
        else if (status == FULL) {
          printf("Stalemate\n");
          return EXIT_SUCCESS;
        }
      }
      else if (status == WON) {
        printf("Player %c wins\n", xplayer);
        return EXIT_SUCCESS;
      }
      else {
        printf("Stalemate\n");
        return EXIT_SUCCESS;
      }
    }
  }
}
