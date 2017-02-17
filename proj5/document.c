/**
    @file document.c
    @author Xiaohui Z Ellis (xzheng6)

    This program provides the document and line structures and related functions.
 */

#include <stdlib.h>
#include "document.h"

/** Create a dynamically allocated line containing the next line read from the given file,
    containing the text of the line, but not the newline at the end of the line.
    @param fp File to read the line from.
    @return The new line read fromt he file. */
Line *loadLine(FILE *fp)
{
  int ch = fgetc(fp);
  if (ch == EOF)
    return NULL;
  Line *line = (Line *) malloc(sizeof(Line));
  line->len = 0;
  line->cap = CAP;
  line->text = (char *) malloc(line->cap);
  while (ch != '\n') {
    if (line->len >= line->cap) {
      line->cap *= 2;
      line->text = (char *) realloc(line->text, line->cap);
    }
      line->text[line->len] = ch;
      line->len += 1;
      ch = fgetc(fp);
  }
  if (line->len >= line->cap) {
    line->cap *= 2;
    line->text = (char *) realloc(line->text, line->cap);
  }
  line->text[line->len] = '\0';
  return line;
}

/** Free the memory used to store the given line.
    @param line Line to free. */
void freeLine(Line *line)
{
  free(line->text);
  free(line);
}

/** Dynamically allocate a document and initialize it,
    with the lines of the file with the given name.
    @param filename Name of the file to read into the document.
    @return dynamically allocated document,
    containing a copy of the text in the given file. */
Document *loadDocument(const char *filename)
{
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "%s: No such file or directory\n", filename);
    exit(EXIT_UNSUCCESS);
  }
  Document *doc = (Document *) malloc(sizeof(Document));
  doc->len = 0;
  doc->cap = CAP;
  doc->lines = (Line **) malloc(sizeof(Line *) * doc->cap);
  doc->cRow = 0;
  doc->cCol = 0;
  Line *line = loadLine(fp);
  while (line) {
    if (doc->len >= doc->cap) {
      doc->cap *= 2;
      doc->lines = (Line **) realloc(doc->lines, sizeof(Line *) * doc->cap);
    }
    doc->lines[doc->len] = line;
    doc->len += 1;
    line = loadLine(fp);
  }
  fclose(fp);
  return doc;
}

/** Save the given document to a file with the given name.
    @param doc Document to save.
    @param dir Filename to save it to.
    @return true if successful. */
bool saveDocument(Document *doc, const char *filename)
{
  FILE *fp = fopen(filename, "w");
  if (!fp)
    return false;
  int tmp = doc->len;
  for (int i = 0; i < tmp; i++)
    fprintf(fp, "%s\n", doc->lines[i]->text);
  fclose(fp);
  return true;
}

/** If possible, move the cursor in doc in the indicated direction.
    @param doc Document in which to move the cursor.
    @param dir Direction to move the currsor.
    @return true if the cursor can be moved in the requested direction. */
bool moveCursor(Document *doc, CursorDir dir)
{
  if (dir == CursorUp) {
    if (doc->cRow == 0)
      return false;
    doc->cRow -= 1;
    int tmp = doc->lines[doc->cRow]->len;
    if (doc->cCol > tmp)
      doc->cCol = tmp;
    return true;
  }
  else if (dir == CursorRight) {
    int tmp = doc->lines[doc->cRow]->len;
    if (doc->cCol == tmp)
      return false;
    doc->cCol += 1;
    return true;
  }
  else if (dir == CursorDown) {
    if (doc->cRow == doc->len - 1)
      return false;
    doc->cRow += 1;
    int tmp = doc->lines[doc->cRow]->len;
    if (doc->cCol > tmp)
      doc->cCol = tmp;
    return true;
  }
  else if (dir == CursorLeft) {
    if (doc->cCol == 0)
      return false;
    doc->cCol -= 1;
    return true;
  }
  return false;
}

/** Free all the memory for the given document,
    including freeing all the Line structures it contains.
    @param doc Document to free. */
void freeDocument(Document *doc)
{
  int tmp = doc->len;
  for (int i = 0; i < tmp; i++)
    freeLine(doc->lines[i]);
  free(doc->lines);
  free(doc);
}
