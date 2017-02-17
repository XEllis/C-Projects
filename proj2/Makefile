# We're using the default rules for make, but we're using
# these variables to get them to do exactly what we want.
CC = gcc
CFLAGS = -g -Wall -std=c99 -D_GNU_SOURCE
LDLIBS = -lm

# This is a common trick.  All is the first target, so it's the
# default.  We use it to build both of the executables we want.
all: cross connect

cross: cross.o

connect: connect.o board.o

cross.o: cross.c

connect.o: connect.c board.h

board.o: board.c board.h

# Another common trick, a clean rule to remove temporary files, or
# files we could easily rebuild.
clean:
	rm -f cross cross.o
	rm -f connect connect.o
	rm -f board board.o
	rm -f output.txt
	rm -f stderr.txt
