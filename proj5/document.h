#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

#include <stdio.h>
#include <stdbool.h>

/** The default capacity for the text and lines array. */
#define CAP 10

/** Exit code for invalid input. */
#define EXIT_UNSUCCESS 1

/** Representation for a line of text in the document. */
typedef struct {
  /** Length of this line, not counting the null terminator. */
  int len;

  /** Capacity for the text array, we over-allocate.
      This must be at least one more than len so there's room for the null. */
  int cap;

  /** Text of this line, including a null termiantor at the end. */
  char *text;
} Line;

/** Representation for text in the document. */
typedef struct Document {
  /** Number of lines in the document. */
  int len;

  /** Capacity of the lines array.  It's over-allocated. */
  int cap;

  /** Pointer to a Line structure for each line of text in the document. */
  Line **lines;

  /** Row and column position of the cursor. */
  int cRow, cCol;
} Document;

/** Create a dynamically allocated line containing the next line read from the given file,
    containing the text of the line, but not the newline at the end of the line.
    @param fp File to read the line from.
    @return The new line read fromt he file. */
Line *loadLine(FILE *fp);

/** Free the memory used to store the given line.
    @param line Line to free. */
void freeLine(Line *line);

/** Dynamically allocate a document and initialize it,
    with the lines of the file with the given name.
    @param filename Name of the file to read into the document.
    @return dynamically allocated document,
    containing a copy of the text in the given file. */
Document *loadDocument(const char *filename);

/** Save the given document to a file with the given name.
    @param doc Document to save.
    @param dir Filename to save it to.
    @return true if successful. */
bool saveDocument(Document *doc, const char *filename);

/** Enumeration containing the four directions for cursor movement. */
typedef enum {CursorUp, CursorRight, CursorDown, CursorLeft} CursorDir;

/** If possible, move the cursor in doc in the indicated direction.
    @param doc Document in which to move the cursor.
    @param dir Direction to move the currsor.
    @return true if the cursor can be moved in the requested direction. */
bool moveCursor(Document *doc, CursorDir dir);

/** Free all the memory for the given document,
    including freeing all the Line structures it contains.
    @param doc Document to free. */
void freeDocument(Document *doc);

#endif
