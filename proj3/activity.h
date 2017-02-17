/** The name of the leader cannot be longer than 20. */
#define LEADER 20

/** The word for the match command cannot be longer than 20. */
#define MATCH 20

/** The hour is in the range of 0 and 23. */
#define HOUR 23

/** The minute is in the range of 0 and 59. */
#define MINUTE 59

/** Time for building Activity. */
typedef struct {
  int hour;
  int minute;
} Time;

/** Activity for building Schedule. */
typedef struct {
  Time startTime;
  Time endTime;
  char Leader[LEADER+1];
  int ID;
  char *Title;
} Activity;

/**
   Read input from standard input in the same format as the arguments to the add command,
   but not the add keyword itself. If the input is valid, it will dynamically allocate,
   and initialize an instance of the Activity structure and return a pointer to it.
   If not, it will return NULL.
   @return pointer to the added activity if the input is valid.
 */
Activity *readActivity();

/**
   Free the dynamically allocated memory used to store an activity,
   including the structure itself and the title.
   @param act pointer to the activity.
 */
void freeActivity(Activity *act);
