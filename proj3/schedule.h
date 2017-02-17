#include "activity.h"
#include <stdbool.h>

/** Schedule to keep up with all the Activities. */
typedef struct {
  Activity **actArrayPtr;
  int nextID;
  int NUMAct;
} Schedule;

/**
   Dynamically allocate space for a new, empty Schedule instance and return a pointer to it.
   @return pointer to the schedule.
 */
Schedule *createSchedule();

/**
   Free all the dynamically allocated memory used by a schedule,
   including the schedule object itself, its resizable array of Activity pointers,
   and the Activity instances themselves.
   @param sched pointer to the schedule.
 */
void freeSchedule(Schedule *sched);

/**
   Add the given activity to the given schedule, grow the resizable array as needed,
   and return true if successful. Check for scheduling conflicts. It returns false,
   if the given activity overlaps with an activity the leader is already doing.
   @param sched pointer to the schedule.
   @param act pointer to the activity.
   @return true if the given activity is added successfully to the given schedule.
 */
bool addActivity(Schedule *sched, Activity *act);

/**
   Decide what to print and what not to print, in order to perform the schedule command.
   Return true since the schedule command prints all the activities in the schedule.
   @param act pointer to the activity.
   @param arg NULL pointer.
   @return true for all the activities in the schedule.
 */
bool scheduleCMD(Activity *act, void *arg);

/**
   Decide what to print and what not to print, in order to perform the at command.
   Return true if the activity takes place at the given time.
   @param act pointer to the activity.
   @param arg pointer to the time.
   @return true if the activity takes place at the given time.
 */
bool atCMD(Activity *act, void *arg);

/**
   Decide what to print and what not to print, in order to perform the match command.
   Return true if the title of the activity contains the sequence of letters of the given word.
   @param act pointer to the activity.
   @param arg pointer to the word.
   @return true if the title of the activity contains the sequence of letters of the given word.
 */
bool matchCMD(Activity *act, void *arg);

/**
   Decide what to print and what not to print, in order to perform the leader command.
   Return true if the given leader's name matches the leader of the activity.
   @param act pointer to the activity.
   @param arg pointer to the leader's name.
   @return true if the given leader's name matches the leader of the activity.
 */
bool leaderCMD(Activity *act, void *arg);

/**
   Go through the list of activities and print them in the right order.
   @param sched pointer to the schedule.
   @param test pointer to function to decide what to print and what not to print.
   @param arg void pointer to information the test function needs.
 */
void printSchedule(Schedule *sched, bool (*test)(Activity *, void *arg), void *arg);
