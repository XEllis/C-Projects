# We're using the default rules for make, but we're using
# these variables to get them to do exactly what we want.
CC = gcc
CFLAGS = -g -Wall -std=c99 -D_GNU_SOURCE
LDLIBS = -lm

daily: daily.o schedule.o activity.o

daily.o: daily.c schedule.h

schedule.o: schedule.c schedule.h

activity.o: activity.c activity.h

# A common trick, a clean rule to remove temporary files, or
# files we could easily rebuild.
clean:
	rm -f daily daily.o
	rm -f schedule schedule.o
	rm -f activity activity.o
	rm -f output.txt
