/**
   @file daily.c
   @author Xiaohui Z Ellis (xzheng6)
   
   This program can store, update and report on a daily schedule of activities,
   each with a start time and end time. There will be interactive commands to add,
   and remove activities from the schedule, and query activities based on different criteria.
 */
#include "schedule.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
   This function helps program support one additional command, summary. It prints out table,
   summarizing the scheduled activities, with each row summarizing what's going on,
   in a particular half-hour block, and each column reporting on,
   what a particular activity leader is doing.
   @param sched pointer to the schedule.
 */
void printSummary(Schedule *sched)
{
  int count = 0;
  char **ptr = (char **) malloc(sched->NUMAct*sizeof(char *));
  int len = 0;
  Time st;
  Time et;
  for (int i = 0; i < sched->NUMAct; i++) {
    Activity *actPtr = (sched->actArrayPtr)[i];
    if (i == 0) {
      st = actPtr->startTime;
      et = actPtr->endTime;
      len = strlen(actPtr->Leader);
      ptr[0] = actPtr->Leader;
      count++;
    }
    else {
      int endt = et.hour*(MINUTE+1)+et.minute;
      int temp = actPtr->endTime.hour*(MINUTE+1)+actPtr->endTime.minute;
      if (temp > endt) {
        et = actPtr->endTime;
      }
      if (strlen(actPtr->Leader) > len) {
        len = strlen(actPtr->Leader);
      }
      bool flag = false;
      for (int j = 0; j < count; j++) {
        if (strcmp(ptr[j], actPtr->Leader) == 0) {
          flag = true;
        }
      }
      if (flag) {
        continue;
      }
      else {
        int x = 0;
        for (int k = 0; k < count; k++) {
          if (strcmp(actPtr->Leader, ptr[k]) < 0) {
            x=k;
            break;
          }
          x=k+1;
        }
        for (int m = count; m > x; m--) {
          ptr[m] = ptr[m-1];
        }
        ptr[x] = actPtr->Leader;
        count++;
      }
    }
  }
  if (st.minute < (MINUTE+1)/2) {
    st.minute = 0;
  }
  else {
    st.minute = (MINUTE+1)/2;
  }
  if (et.minute > (MINUTE+1)/2) {
    et.minute = (MINUTE+1)/2;
  }
  else {
    et.minute = 0;
  }
  int rows = (et.hour*(MINUTE+1)+et.minute-st.hour*(MINUTE+1)-st.minute)/(MINUTE+1)*2+1;
  int cols = count+2;
  int printTable[rows][cols];
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printTable[i][j] = 0;
    }
  }
  for (int m = 0; m < rows; m++) {
    if (st.minute == 0) {
      printTable[m][0] = st.hour + m/2;
      if (m % 2) {
        printTable[m][1] = (MINUTE+1)/2;
      }
    }
    else {
      printTable[m][0] = st.hour + (m+1)/2;
      if (!(m % 2)) {
        printTable[m][1] = (MINUTE+1)/2;
      }
    }
  }
  for (int n = 0; n < count; n++) {
    for (int i = 0; i < sched->NUMAct; i++) {
      Activity *actPtr = (sched->actArrayPtr)[i];
      if (strcmp(actPtr->Leader, ptr[n]) == 0) {
        Time stemp = actPtr->startTime;
        Time etemp = actPtr->endTime;
        if (stemp.minute < (MINUTE+1)/2) {
          stemp.minute = 0;
        }
        else {
          stemp.minute = (MINUTE+1)/2;
        }
        if (etemp.minute > (MINUTE+1)/2) {
          etemp.minute = (MINUTE+1)/2;
        }
        else if (etemp.minute == 0) {
          etemp.minute = (MINUTE+1)/2;
          etemp.hour = etemp.hour-1;
        }
        else {
          etemp.minute = 0;
        }
        int x = (etemp.hour*(MINUTE+1)+etemp.minute
                 -stemp.hour*(MINUTE+1)-stemp.minute)*2/(MINUTE+1)+1;
        int sr = (stemp.hour*(MINUTE+1)+stemp.minute
                  -st.hour*(MINUTE+1)-st.minute)*2/(MINUTE+1);
        int er = sr+x;
        for (int l = sr; l < er; l++) {
          printTable[l][n+2] = 1;
        }
      }
    }
  }
  printf("\n");
  for (int i = 0; i < rows; i++) {
    printf("%2d:%02d  ", printTable[i][0], printTable[i][1]);
    for (int j = 2; j < cols-1; j++) {
      if (printTable[i][j] == 1) {
        printf("*  ");
      }
      else {
        printf("   ");
      }
    }
    if (printTable[i][cols-1] == 1) {
      printf("*");
    }
    else {
      printf(" ");
    }
    printf("\n");
  }
  for (int l = 0; l < len; l++) {
    printf("       ");
    for (int x = 0; x < count-1; x++) {
      if (ptr[x][l]) {
        printf("%c  ", ptr[x][l]);
      }
      else {
        printf("   ");
      }
    }
    if (ptr[count-1][l]) {
      printf("%c", ptr[count-1][l]);
    }
    else {
      printf(" ");
    }
    printf("\n");
  }
  free(ptr);
}

/**
   The program will repeatedly prompt the user for a command.
   At the prompt, the user can type any of 7 available commands:
   add, remove, schedule, leader, at, match or quit.
   The add command adds a new activity to the schedule. After the add keyword,
   the user needs to enter a starting time for the activity, an ending time,
   the name of the activity leader and a title for the activity leader.
   If successful, this command adds an activity with the given attributes to the schedule.
   The activity will be given the next consecutive ID number,
   starting from one for the first activity.
   The remove command is entered as the keyword remove followed by whitespace,
   then the ID of an activity. On success, the requested activity is removed from the schedule,
   and nothing is printed.
   The schedule command prints a report of all activities in the schedule.
   This command doesn't take any arguments.
   The leader command is entered as the keyword leader followed by whitespace,
   then the name of a person, given as a word of up to 20 characters.
   The name can't contain whitespace.
   In response to this command, the program should print all activities,
   for which the given person is the leader.
   The at command is given as the keyword at, followed by whitespace then a time of day.
   In response to a valid at command, the program prints a list of all activities,
   occurring at the given time.
   The match command is given by the keyword match followed by a word of up to 20 characters.
   In response to this command, the program will print a report of the activities,
   that contain the given word as a substring in their title.
   The given word can't contain spaces.
   The quit command doesn't take any arguments. It terminates the program,
   without prompting for any more commands.
   The program also terminates successfully if it reaches the end-of-file on standard input,
   while it's trying to read the keyword for a command.
   @return exit status for the program.
 */
int main() {
  printf("> ");
  char str[LEADER+1] = "";
  Schedule *sched = createSchedule();
  while (scanf("%21s", str) == 1) {
    if (strcmp(str, "add") == 0) {
      Activity *act = readActivity();
      if (!act) {
        printf("Invalid command\n");
      }
      else if (!addActivity(sched, act)) {
        printf("Schedule conflict\n");
        printf("> ");
        continue;
      }
      else {
        printf("> ");
        continue;
      }
    }
    else if (strcmp(str, "schedule") == 0) {
      int ch = getchar();
      if (ch != '\n') {
        while (ch == ' ') {
          ch = getchar();
        }
        if (ch == '\n') {
          printSchedule(sched, scheduleCMD, NULL);
          printf("> ");
          continue;
        }
        else {
          printf("Invalid command\n");
        }
      }
      else {
        printSchedule(sched, scheduleCMD, NULL);
        printf("> ");
        continue;
      }
    }
    else if (strcmp(str, "remove") == 0) {
      int num = 0;
      int ch;
      if (scanf("%d", &num) == 1) {
        if (num <= 0 || num >= sched->nextID) {
          printf("Invalid command\n");
        }
        else {
          bool flag = false;
          for (int i = 0; i < sched->NUMAct; i++) {
            Activity *actPtr = (sched->actArrayPtr)[i];
            if (num == (actPtr->ID)) {
              flag = true;
            }
          }
          if (flag) {
            if ((ch = getchar()) != '\n') {
              while (ch == ' ') {
                ch = getchar();
              }
              if (ch == '\n') {
                int count = 0;
                for (int i = 0; i < sched->NUMAct; i++) {
                  Activity *actPtr = (sched->actArrayPtr)[i];
                  if (num == (actPtr->ID)) {
                    count = i;
                    break;
                  }
                }
                Activity *tofree = (sched->actArrayPtr)[count];
                Activity **temp = (Activity **) malloc((sched->NUMAct-1)*sizeof(Activity *));
                for (int i = 0; i < count; i++) {
                  Activity *actPtr = (sched->actArrayPtr)[i];
                  temp[i] = actPtr;
                }
                for (int i = count; i < sched->NUMAct-1; i++) {
                  Activity *actPtr = (sched->actArrayPtr)[i+1];
                  temp[i] = actPtr;
                }
                free(sched->actArrayPtr);
                sched->actArrayPtr = temp;
                (sched->NUMAct)--;
                freeActivity(tofree);
                printf("> ");
                continue;
              }
              else {
                printf("Invalid command\n");
              }
            }
            else {
              int count = 0;
              for (int i = 0; i < sched->NUMAct; i++) {
                Activity *actPtr = (sched->actArrayPtr)[i];
                if (num == (actPtr->ID)) {
                  count = i;
                  break;
                }
              }
              Activity *tofree = (sched->actArrayPtr)[count];
              Activity **temp = (Activity **) malloc((sched->NUMAct-1)*sizeof(Activity *));
              for (int i = 0; i < count; i++) {
                Activity *actPtr = (sched->actArrayPtr)[i];
                temp[i] = actPtr;
              }
              for (int i = count; i < sched->NUMAct-1; i++) {
                Activity *actPtr = (sched->actArrayPtr)[i+1];
                temp[i] = actPtr;
              }
              free(sched->actArrayPtr);
              sched->actArrayPtr = temp;
              (sched->NUMAct)--;
              freeActivity(tofree);
              printf("> ");
              continue;
            }
          }
          else {
            printf("Invalid command\n");
          }
        }
      }
      else {
        printf("Invalid command\n");
      }
    }
    else if (strcmp(str, "leader") == 0) {
      char t[LEADER+1] = "";
      int ch;
      if (scanf("%21s", t) == 1) {
        if (t[LEADER]) {
          printf("Invalid command\n");
        }
        else if ((ch = getchar()) != '\n') {
          while (ch == ' ') {
            ch = getchar();
          }
          if (ch == '\n') {
            printSchedule(sched, leaderCMD, t);
            printf("> ");
            continue;
          }
          else {
            printf("Invalid command\n");
          }
        }
        else {
          printSchedule(sched, leaderCMD, t);
          printf("> ");
          continue;
        }
      }
      else {
        printf("Invalid command\n");
      }
    }
    else if (strcmp(str, "at") == 0) {
      Time atTime;
      int ch;
      if (scanf("%d", &atTime.hour) != 1) {
        printf("Invalid command\n");
      }
      else if (atTime.hour < 0 || atTime.hour > HOUR) {
        printf("Invalid command\n");
      }
      else if (scanf("%1s%d", str, &atTime.minute) != 2) {
        printf("Invalid command\n");
      }
      else if (atTime.minute < 0 || atTime.minute > MINUTE) {
        printf("Invalid command\n");
      }
      else if ((ch = getchar()) != '\n') {
          while (ch == ' ') {
            ch = getchar();
          }
          if (ch == '\n') {
            printSchedule(sched, atCMD, &atTime);
            printf("> ");
            continue;
          }
          else {
            printf("Invalid command\n");
          }
      }
      else {
        printSchedule(sched, atCMD, &atTime);
        printf("> ");
        continue;
      }
    }
    else if (strcmp(str, "match") == 0) {
      char t[MATCH+1] = "";
      int ch;
      if (scanf("%21s", t) == 1) {
        if (t[MATCH]) {
          printf("%s", t);
          printf("Invalid command\n");
        }
        else if ((ch = getchar()) != '\n') {
          while (ch == ' ') {
            ch = getchar();
          }
          if (ch == '\n') {
            printSchedule(sched, matchCMD, t);
            printf("> ");
            continue;
          }
          else {
            printf("Invalid command\n");
          }
        }
        else {
          printSchedule(sched, matchCMD, t);
          printf("> ");
          continue;
        }
      }
      else {
        printf("Invalid command\n");
      }
    }
    else if (strcmp(str, "quit") == 0) {
      int ch = getchar();
      if (ch != '\n') {
        while (ch == ' ') {
          ch = getchar();
        }
        if (ch == '\n') {
          freeSchedule(sched);
          return EXIT_SUCCESS;
        }
        else {
          printf("Invalid command\n");
        }
      }
      else {
        freeSchedule(sched);
        return EXIT_SUCCESS;
      }
    }
    else if (strcmp(str, "summary") == 0) {
      int ch = getchar();
      if (ch != '\n') {
        while (ch == ' ') {
          ch = getchar();
        }
        if (ch == '\n') {
          printSummary(sched);
          printf("> ");
          continue;
        }
        else {
          printf("Invalid command\n");
        }
      }
      else {
        printSummary(sched);
        printf("> ");
        continue;
      }
    }
    else {
      printf("Invalid command\n");
    }
    printf("> ");
    int ch = getchar();
    while (ch != '\n') {
      ch = getchar();
    }
  }
  freeSchedule(sched);
  return EXIT_SUCCESS;
}
