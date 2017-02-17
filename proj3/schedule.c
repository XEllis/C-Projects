/**
   @file schedule.c
   @author Xiaohui Z Ellis (xzheng6)
   
   This program defines a number of helper functions for the daily program.
 */

#include "schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
   Dynamically allocate space for a new, empty Schedule instance and return a pointer to it.
   @return pointer to the schedule.
 */
Schedule *createSchedule()
{
  Schedule *sched = (Schedule *) malloc(sizeof(Schedule));
  sched->actArrayPtr = NULL;
  sched->nextID = 1;
  sched->NUMAct = 0;
  return sched;
}

/**
   Free all the dynamically allocated memory used by a schedule,
   including the schedule object itself, its resizable array of Activity pointers,
   and the Activity instances themselves.
   @param sched pointer to the schedule.
 */
void freeSchedule(Schedule *sched)
{
  Activity **actPtrPtr = sched->actArrayPtr;
  int length = sched->NUMAct;
  free(sched);
  for (int i = 0; i < length; i++) {
    freeActivity(actPtrPtr[i]);
  }
  free(actPtrPtr);
}

/**
   Add the given activity to the given schedule, grow the resizable array as needed,
   and return true if successful. Check for scheduling conflicts. It returns false,
   if the given activity overlaps with an activity the leader is already doing.
   @param sched pointer to the schedule.
   @param act pointer to the activity.
   @return true if the given activity is added successfully to the given schedule.
 */
bool addActivity(Schedule *sched, Activity *act)
{
  if (sched->nextID == 1) {
    act->ID = sched->nextID;
    Activity **temp = (Activity **) malloc(sizeof(Activity *));
    temp[0] = act;
    sched->actArrayPtr = temp;
    (sched->nextID)++;
    (sched->NUMAct)++;
    return true;
  }
  for (int i = 0; i < sched->NUMAct; i++) {
    Activity *actPtr = (sched->actArrayPtr)[i];
    if (strcmp(actPtr->Leader, act->Leader) == 0) {
      int actStart = act->startTime.hour*(MINUTE+1)+act->startTime.minute;
      int actPtrStart = actPtr->startTime.hour*(MINUTE+1)+actPtr->startTime.minute;
      int actEnd = act->endTime.hour*(MINUTE+1)+act->endTime.minute;
      int actPtrEnd = actPtr->endTime.hour*(MINUTE+1)+actPtr->endTime.minute;
      if ((actStart >= actPtrStart && actStart < actPtrEnd) ||
          (actEnd > actPtrStart && actEnd <= actPtrEnd) ||
          (actStart < actPtrStart && actEnd > actPtrEnd)) {
        freeActivity(act);
        return false;
      }
    }
  }
  act->ID = sched->nextID;
  Activity **temp = (Activity **) malloc((sched->NUMAct+1)*sizeof(Activity *));
  int count = 0;
  int actStart = act->startTime.hour*(MINUTE+1)+act->startTime.minute;
  for (int i = 0; i < sched->NUMAct; i++) {
    Activity *actPtr = (sched->actArrayPtr)[i];
    int actPtrStart = actPtr->startTime.hour*(MINUTE+1)+actPtr->startTime.minute;
    if (actPtrStart < actStart) {
      temp[count] = actPtr;
      count++;
    }
    if (actPtrStart == actStart) {
      if (strcmp(actPtr->Leader, act->Leader) < 0) {
        temp[count] = actPtr;
        count++;
      }
    }
  }
  temp[count] = act;
  count++;
  for (int i = count-1; i < sched->NUMAct; i++) {
    Activity *actPtr = (sched->actArrayPtr)[i];
    temp[count] = actPtr;
    count++;
  }
  free(sched->actArrayPtr);
  sched->actArrayPtr = temp;
  (sched->nextID)++;
  (sched->NUMAct)++;
  return true;
}

/**
   Decide what to print and what not to print, in order to perform the schedule command.
   Return true since the schedule command prints all the activities in the schedule.
   @param act pointer to the activity.
   @param arg NULL pointer.
   @return true for all the activities in the schedule.
 */
bool scheduleCMD(Activity *act, void *arg)
{
  return true;
}

/**
   Decide what to print and what not to print, in order to perform the at command.
   Return true if the activity takes place at the given time.
   @param act pointer to the activity.
   @param arg pointer to the time.
   @return true if the activity takes place at the given time.
 */
bool atCMD(Activity *act, void *arg)
{
  Time *t = (Time *) arg;
  int tTime = t->hour*(MINUTE+1)+t->minute;
  int actStart = act->startTime.hour*(MINUTE+1)+act->startTime.minute;
  int actEnd = act->endTime.hour*(MINUTE+1)+act->endTime.minute;
  if (tTime >= actStart && tTime < actEnd) {
   return true;
  }
  return false;
}

/**
   Decide what to print and what not to print, in order to perform the match command.
   Return true if the title of the activity contains the sequence of letters of the given word.
   @param act pointer to the activity.
   @param arg pointer to the word.
   @return true if the title of the activity contains the sequence of letters of the given word.
 */
bool matchCMD(Activity *act, void *arg)
{
  char *t = (char *) arg;
  if (strlen(t) > strlen(act->Title)) {
    return false;
  }
  for (int i = 0; i < strlen(act->Title)-strlen(t)+1; i++) {
    if (tolower((act->Title)[i]) == tolower(t[0])) {
      bool flag = true;
      for (int j = 1; j < strlen(t); j++) {
        if (tolower((act->Title)[i+j]) != tolower(t[j])) {
          flag = false;
        }
      }
      if (flag) {
        return true;
      }
    }
  }
  return false;
}

/**
   Decide what to print and what not to print, in order to perform the leader command.
   Return true if the given leader's name matches the leader of the activity.
   @param act pointer to the activity.
   @param arg pointer to the leader's name.
   @return true if the given leader's name matches the leader of the activity.
 */
bool leaderCMD(Activity *act, void *arg)
{
  char *t = (char *) arg;
  if (strcmp(t, act->Leader) == 0) {
    return true;
  }
  return false;
}

/**
   Go through the list of activities and print them in the right order.
   @param sched pointer to the schedule.
   @param test pointer to function to decide what to print and what not to print.
   @param arg void pointer to information the test function needs.
 */
void printSchedule(Schedule *sched, bool (*test)(Activity *, void *arg), void *arg)
{
  int maxlen = 0;
  for (int i = 0; i < sched->NUMAct; i++) {
    Activity *actPtr = (sched->actArrayPtr)[i];
    if (test(actPtr, arg)) {
      if (strlen(actPtr->Leader) > maxlen) {
        maxlen = strlen(actPtr->Leader);
      }
    }
  }
  printf("\n");
  for (int i = 0; i < sched->NUMAct; i++) {
    Activity *actPtr = (sched->actArrayPtr)[i];
    if (test(actPtr, arg)) {
      printf("%2d:%02d ", actPtr->startTime.hour, actPtr->startTime.minute);
      printf("%2d:%02d ", actPtr->endTime.hour, actPtr->endTime.minute);
      printf("(%03d) ", actPtr->ID);
      printf("%s ", actPtr->Leader);
      for (int i = 0; i < maxlen-strlen(actPtr->Leader) ;i++) {
        printf(" ");
      }
      printf("%s\n", actPtr->Title);
    }
  }
}
