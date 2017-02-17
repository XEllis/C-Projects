/**
   @file activity.c
   @author Xiaohui Z Ellis (xzheng6)
   
   This program defines a number of helper functions for the daily program.
 */

#include "activity.h"
#include <stdio.h>
#include <stdlib.h>

/**
   Read input from standard input in the same format as the arguments to the add command,
   but not the add keyword itself. If the input is valid, it will dynamically allocate,
   and initialize an instance of the Activity structure and return a pointer to it.
   If not, it will return NULL.
   @return pointer to the added activity if the input is valid.
 */
Activity *readActivity()
{
  Activity *act = (Activity *) malloc(sizeof(Activity));
  for (int i = 0; i < LEADER+1; i++) {
    (act->Leader)[i] = '\0';
  }
  if (scanf("%d", &act->startTime.hour) != 1) {
    free(act);
    return NULL;
  }
  if (act->startTime.hour < 0 || act->startTime.hour > HOUR) {
    free(act);
    return NULL;
  }
  char str[LEADER+1]="";
  if (scanf("%1s%d", str, &act->startTime.minute) != 2) {
    free(act);
    return NULL;
  }
  if (str[0] != ':' || act->startTime.minute < 0 || act->startTime.minute > MINUTE) {
    free(act);
    return NULL;
  }
  if (scanf("%d", &act->endTime.hour) != 1) {
    free(act);
    return NULL;
  }
  if (act->endTime.hour < 0 || act->endTime.hour > HOUR) {
    free(act);
    return NULL;
  }
  if (scanf("%1s%d", str, &act->endTime.minute) != 2) {
    free(act);
    return NULL;
  }
  if (str[0] != ':' || act->endTime.minute < 0 || act->endTime.minute > MINUTE) {
    free(act);
    return NULL;
  }
  int start = act->startTime.hour*(MINUTE+1)+act->startTime.minute;
  int end = act->endTime.hour*(MINUTE+1)+act->endTime.minute;
  if (end <= start) {
    free(act);
    return NULL;
  }
  if (scanf("%21s", act->Leader) != 1) {
    free(act);
    return NULL;
  }
  if ((act->Leader)[LEADER]) {
    free(act);
    return NULL;
  }
  int capacity = MATCH;
  char *title = (char *) malloc(capacity);
  if (scanf("%1s", title) != 1) {
    free(act);
    free(title);
    return NULL;
  }
  int ch = getchar();
  int num = 1;
  while (ch != '\n') {
    if (num == capacity) {
      capacity *= 2;
      title = (char *) realloc(title, capacity);
    }
    title[num] = ch;
    num++;
    ch = getchar();
  }
  if (num == capacity) {
    capacity *= 2;
    title = (char *) realloc(title, capacity);
  }
  title[num] = '\0';
  act->Title = title;
  return act;
}

/**
   Free the dynamically allocated memory used to store an activity,
   including the structure itself and the title.
   @param act pointer to the activity.
 */
void freeActivity(Activity *act)
{
  free(act->Title);
  free(act);
}
