/**
    @file edit.c
    @author Xiaohui Z Ellis (xzheng6)

    This program provides the inheritance hierarchy, Edit,
    representing undo-able edits to a document.
    It also provides a History structure,
    and related functions to maintain a stack of edits,
    that can be undone and a stack of recently undone edits that can be redone.
 */

#include <stdlib.h>
#include "edit.h"

/** Derived class, Insert. */
typedef struct {
  void (*apply)(Edit *, Document *);
  void (*undo)(Edit *, Document *);
  void (*destroy)(Edit *);
  int ch;
  int iRow;
  int iCol;
} Insert;

/** Function used to apply insert to the given document.
    @param edit The edit that should be applied to the given document.
    @param doc Document to insert a character in. */
void applyInsert(Edit *edit, Document *doc)
{
  Insert *insert = (Insert *) edit;
  if (insert->ch != '\n') {
    if (doc->lines[insert->iRow]->len >= doc->lines[insert->iRow]->cap) {
      doc->lines[insert->iRow]->cap *= 2;
      doc->lines[insert->iRow]->text = (char *) realloc(doc->lines[insert->iRow]->text,
                                                        doc->lines[insert->iRow]->cap);
    }
    for (int i = doc->lines[insert->iRow]->len; i > insert->iCol; i--)
      doc->lines[insert->iRow]->text[i] = doc->lines[insert->iRow]->text[i-1];
    doc->lines[insert->iRow]->text[insert->iCol] = insert->ch;
    doc->lines[insert->iRow]->len += 1;
    if (doc->lines[insert->iRow]->len >= doc->lines[insert->iRow]->cap) {
      doc->lines[insert->iRow]->cap *= 2;
      doc->lines[insert->iRow]->text = (char *) realloc(doc->lines[insert->iRow]->text,
                                                        doc->lines[insert->iRow]->cap);
    }
    doc->lines[insert->iRow]->text[doc->lines[insert->iRow]->len] = '\0';
    doc->cCol = insert->iCol + 1;
    doc->cRow = insert->iRow;
  }
  else {
    if (doc->len >= doc->cap) {
      doc->cap *= 2;
      doc->lines = (Line **) realloc(doc->lines, sizeof(Line *) * doc->cap);
    }
    for (int i = doc->len; i > insert->iRow + 1; i--)
      doc->lines[i] = doc->lines[i-1];
    doc->lines[insert->iRow+1] = (Line *) malloc(sizeof(Line));
    doc->lines[insert->iRow+1]->len = 0;
    doc->lines[insert->iRow+1]->cap = CAP;
    doc->lines[insert->iRow+1]->text = (char *) malloc(doc->lines[insert->iRow+1]->cap);
    for (int i = insert->iCol; i < doc->lines[insert->iRow]->len; i++) {
      if (doc->lines[insert->iRow+1]->len >= doc->lines[insert->iRow+1]->cap) {
        doc->lines[insert->iRow+1]->cap *= 2;
        doc->lines[insert->iRow+1]->text = (char *) realloc(doc->lines[insert->iRow+1]->text,
                                                            doc->lines[insert->iRow+1]->cap);
      }
      doc->lines[insert->iRow+1]->text[doc->lines[insert->iRow+1]->len] = doc->lines
                                                                          [insert->iRow]->text[i];
      doc->lines[insert->iRow+1]->len += 1;
    }
    if (doc->lines[insert->iRow+1]->len >= doc->lines[insert->iRow+1]->cap) {
      doc->lines[insert->iRow+1]->cap *= 2;
      doc->lines[insert->iRow+1]->text = (char *) realloc(doc->lines[insert->iRow+1]->text,
                                                          doc->lines[insert->iRow+1]->cap);
    }
    doc->lines[insert->iRow+1]->text[doc->lines[insert->iRow+1]->len] = '\0';
    doc->lines[insert->iRow]->text[insert->iCol] = '\0';
    doc->lines[insert->iRow]->len = insert->iCol;
    doc->cCol = 0;
    doc->cRow = insert->iRow + 1;
    doc->len += 1;
  }
}

/** Function used to undo insert to the given document.
    @param edit The edit that should be undone to the given document.
    @param doc Document to undo the insert of a character in. */
void undoInsert(Edit *edit, Document *doc)
{
  Insert *insert = (Insert *) edit;
  if (insert->ch != '\n') {
    for (int i = insert->iCol; i < doc->lines[insert->iRow]->len; i++)
      doc->lines[insert->iRow]->text[i] = doc->lines[insert->iRow]->text[i+1];
    doc->lines[insert->iRow]->len -= 1;
    doc->cRow = insert->iRow;
    doc->cCol = insert->iCol;
  }
  else {
    for (int i = 0; i < doc->lines[insert->iRow+1]->len; i++) {
      doc->lines[insert->iRow]->text[doc->lines[insert->iRow]->len] = doc->lines
                                                                      [insert->iRow+1]->text[i];
      doc->lines[insert->iRow]->len += 1;
    }
    doc->lines[insert->iRow]->text[doc->lines[insert->iRow]->len] = '\0';
    freeLine(doc->lines[insert->iRow+1]);
    for (int i = insert->iRow + 1; i < doc->len - 1; i++)
      doc->lines[i] = doc->lines[i+1];
    doc->cRow = insert->iRow;
    doc->cCol = insert->iCol;
    doc->len -= 1;
  }
}

/** Function used to free the given insert.
    @param edit The edit that should be freed. */
void destroyInsert(Edit *edit)
{
  Insert *insert = (Insert *) edit;
  free(insert);
}

/** Derived class, Delete. */
typedef struct {
  void (*apply)(Edit *, Document *);
  void (*undo)(Edit *, Document *);
  void (*destroy)(Edit *);
  int ch;
  int dRow;
  int dCol;
  int upc;
} Delete;

/** Function used to apply delete to the given document.
    @param edit The edit that should be applied to the given document.
    @param doc Document to delete a character in. */
void applyDelete(Edit *edit, Document *doc)
{
  Delete *dlt = (Delete *) edit;
  if (dlt->dCol != 0) {
    for (int i = dlt->dCol - 1; i < doc->lines[dlt->dRow]->len; i++)
      doc->lines[dlt->dRow]->text[i] = doc->lines[dlt->dRow]->text[i+1];
    doc->lines[dlt->dRow]->len -= 1;
    doc->cCol = dlt->dCol - 1;
    doc->cRow = dlt->dRow;
  }
  else {
    for (int i = 0; i < doc->lines[dlt->dRow]->len; i++) {
      if (doc->lines[dlt->dRow-1]->len >= doc->lines[dlt->dRow-1]->cap) {
        doc->lines[dlt->dRow-1]->cap *= 2;
        doc->lines[dlt->dRow-1]->text = (char *) realloc(doc->lines[dlt->dRow-1]->text,
                                                            doc->lines[dlt->dRow-1]->cap);
      }
      doc->lines[dlt->dRow-1]->text[doc->lines[dlt->dRow-1]->len] = doc->lines
                                                                          [dlt->dRow]->text[i];
      doc->lines[dlt->dRow-1]->len += 1;
    }
    if (doc->lines[dlt->dRow-1]->len >= doc->lines[dlt->dRow-1]->cap) {
      doc->lines[dlt->dRow-1]->cap *= 2;
      doc->lines[dlt->dRow-1]->text = (char *) realloc(doc->lines[dlt->dRow-1]->text,
                                                       doc->lines[dlt->dRow-1]->cap);
    }
    doc->lines[dlt->dRow-1]->text[doc->lines[dlt->dRow-1]->len] = '\0';
    freeLine(doc->lines[dlt->dRow]);
    for (int i = dlt->dRow; i < doc->len - 1; i++)
      doc->lines[i] = doc->lines[i+1];
    doc->cRow = dlt->dRow - 1;
    doc->cCol = dlt->upc;
    doc->len -= 1;
  }
}

/** Function used to undo delete to the given document.
    @param edit The edit that should be undone to the given document.
    @param doc Document to undo the delete of a character in. */
void undoDelete(Edit *edit, Document *doc)
{
  Delete *dlt = (Delete *) edit;
  if (dlt->ch != '\n') {
    for (int i = doc->lines[dlt->dRow]->len; i > dlt->dCol - 1; i--)
      doc->lines[dlt->dRow]->text[i] = doc->lines[dlt->dRow]->text[i-1];
    doc->lines[dlt->dRow]->text[dlt->dCol - 1] = dlt->ch;
    doc->lines[dlt->dRow]->len += 1;
    doc->lines[dlt->dRow]->text[doc->lines[dlt->dRow]->len] = '\0';
    doc->cRow = dlt->dRow;
    doc->cCol = dlt->dCol;
  }
  else {
    for (int i = doc->len; i > dlt->dRow; i--)
      doc->lines[i] = doc->lines[i-1];
    doc->lines[dlt->dRow] = (Line *) malloc(sizeof(Line));
    doc->lines[dlt->dRow]->len = 0;
    doc->lines[dlt->dRow]->cap = CAP;
    doc->lines[dlt->dRow]->text = (char *) malloc(doc->lines[dlt->dRow]->cap);
    for (int i = dlt->upc; i < doc->lines[dlt->dRow-1]->len; i++) {
      if (doc->lines[dlt->dRow]->len >= doc->lines[dlt->dRow]->cap) {
        doc->lines[dlt->dRow]->cap *= 2;
        doc->lines[dlt->dRow]->text = (char *) realloc(doc->lines[dlt->dRow]->text,
                                                          doc->lines[dlt->dRow]->cap);
      }
      doc->lines[dlt->dRow]->text[doc->lines[dlt->dRow]->len] = doc->lines
                                                                      [dlt->dRow-1]->text[i];
      doc->lines[dlt->dRow]->len += 1;
    }
    if (doc->lines[dlt->dRow]->len >= doc->lines[dlt->dRow]->cap) {
      doc->lines[dlt->dRow]->cap *= 2;
      doc->lines[dlt->dRow]->text = (char *) realloc(doc->lines[dlt->dRow]->text,
                                                        doc->lines[dlt->dRow]->cap);
    }
    doc->lines[dlt->dRow]->text[doc->lines[dlt->dRow]->len] = '\0';
    doc->lines[dlt->dRow-1]->text[dlt->upc] = '\0';
    doc->lines[dlt->dRow-1]->len = dlt->upc;
    doc->cRow = dlt->dRow;
    doc->cCol = 0;
    doc->len += 1;
  }
}

/** Function used to free the given delete.
    @param edit The edit that should be freed. */
void destroyDelete(Edit *edit)
{
  Delete *dlt = (Delete *) edit;
  free(dlt);
}

/** Make an edit operation to insert the given character into the document,
    at the current cursor position. This doesn't actually change the document,
    but it returns an object that can make the change or undo it.
    @param doc Document to insert a character in.
    @param ch Character to insert in the document.
    @return New, edit operation to insert the character. */
Edit *makeInsert(Document *doc, int ch)
{
  Insert *this = (Insert *) malloc(sizeof(Insert));
  this->apply = applyInsert;
  this->undo = undoInsert;
  this->destroy = destroyInsert;
  this->ch = ch;
  this->iRow = doc->cRow;
  this->iCol = doc->cCol;
  return (Edit *) this;
}

/** Make an edit operation to delete the character,
    before the current cursor position in the document.
    @param doc Document to modify with the edit operation.
    @return New, edit operation to delelete a character,
            or NULL if there's nothing before the current cursor position. */
Edit *makeDelete(Document *doc)
{
  if (doc->cRow == 0 && doc->cCol == 0)
    return NULL;
  Delete *this = (Delete *) malloc(sizeof(Delete));
  this->apply = applyDelete;
  this->undo = undoDelete;
  this->destroy = destroyDelete;
  this->dRow = doc->cRow;
  this->dCol = doc->cCol;
  this->upc = doc->lines[doc->cRow-1]->len;
  if (doc->cCol != 0)
    this->ch = doc->lines[doc->cRow]->text[doc->cCol-1];
  else
    this->ch = '\n';
  return (Edit *) this;
}

/** Initialize the given history to have empty undo and redo stack.
    @param hist The history to initialize. */
void initHistory(History *hist)
{
  hist->unc = 0;
  hist->rec = 0;
}

/** Discard and free any edits in the history.
    This can be used at exit time to keep those edits from looking like memory leaks.
    @param hist The history to clear. */
void clearHistory(History *hist)
{
  for (int i = 0; i < hist->unc; i++)
    hist->undoL[i]->destroy(hist->undoL[i]);
  for (int i = 0; i < hist->rec; i++)
    hist->redoL[i]->destroy(hist->redoL[i]);
}

/** Apply the given edit to the given document,
    and add it to history of undoable operations.
    This will discard any recently undone edits on the redo stack of the history.
    @param hist The History this edit should be saved to, for undo support.
    @param doc The document the edit should be applied to.
    @param edit The the edit that should be applied to the given document. */
void applyEdit(History *hist, Document *doc, Edit *edit)
{
  edit->apply(edit, doc);
  if (hist->unc == HIST_SIZE) {
    hist->undoL[0]->destroy(hist->undoL[0]);
    for (int i = 0; i < HIST_SIZE - 1; i++)
      hist->undoL[i] = hist->undoL[i+1];
    hist->undoL[HIST_SIZE-1] = edit;
  }
  else {
    hist->undoL[hist->unc] = edit;
    hist->unc += 1;
  }
  for (int i = 0; i < hist->rec; i++)
    hist->redoL[i]->destroy(hist->redoL[i]);
  hist->rec = 0;
}

/** Undo the edit on the top of the undo stack, and push it to the redo stack.
    Return true if there was an edit to undo.
    @param hist the history containing the undo-able edit.
    @param doc the document we're supposed to undo the command in.
    @return true if there was an edit to undo. */
bool undoEdit(History *hist, Document *doc)
{
  if (hist->unc == 0)
    return false;
  hist->undoL[hist->unc-1]->undo(hist->undoL[hist->unc-1], doc);
  if (hist->rec == HIST_SIZE) {
    hist->redoL[0]->destroy(hist->redoL[0]);
    for (int i = 0; i < HIST_SIZE - 1; i++)
      hist->redoL[i] = hist->redoL[i+1];
    hist->redoL[HIST_SIZE-1] = hist->undoL[hist->unc-1];
  }
  else {
    hist->redoL[hist->rec] = hist->undoL[hist->unc-1];
    hist->rec += 1;
  }
  hist->unc -= 1;
  return true;
}

/** Redo the edit on the top of the redo stack, and push it to the undo stack.
    Return true if there was an edit to redo.
    @param hist the history containing the redo-able edit.
    @param doc the document we're supposed to redo the command in.
    @return true if there was an edit to redo. */
bool redoEdit(History *hist, Document *doc)
{
  if (hist->rec == 0)
    return false;
  hist->redoL[hist->rec-1]->apply(hist->redoL[hist->rec-1], doc);
  if (hist->unc == HIST_SIZE) {
    hist->undoL[0]->destroy(hist->undoL[0]);
    for (int i = 0; i < HIST_SIZE - 1; i++)
      hist->undoL[i] = hist->undoL[i+1];
    hist->undoL[HIST_SIZE-1] = hist->redoL[hist->rec-1];
  }
  else {
    hist->undoL[hist->unc] = hist->redoL[hist->rec-1];
    hist->unc += 1;
  }
  hist->rec -= 1;
  return true;
}
