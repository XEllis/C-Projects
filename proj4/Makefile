# We're using the default rules for make, but we're using
# these variables to get them to do exactly what we want.
CC = gcc
CFLAGS = -g -Wall -std=c99 -D_GNU_SOURCE
LDLIBS = -lm

# This is a common trick.  All is the first target, so it's the
# default.  We use it to build both of the executables we want.
all: squeeze esqueeze unsqueeze

squeeze: squeeze.o bits.o codes.o

esqueeze: squeeze.o bits.o codes.o
	gcc   squeeze.o bits.o codes.o  -lm -o esqueeze

unsqueeze: unsqueeze.o bits.o codes.o

squeeze.o: squeeze.c bits.h

unsqueeze.o: unsqueeze.c bits.h

bits.o: bits.c bits.h

codes.o: codes.c codes.h

# Another common trick, a clean rule to remove temporary files, or
# files we could easily rebuild.
clean:
	rm -f squeeze squeeze.o
	rm -f esqueeze
	rm -f unsqueeze unsqueeze.o
	rm -f bits bits.o
	rm -f codes codes.o
	rm -f compressed.bin
	rm -f output.txt
	rm -f stderr.txt
	rm -f stdout.txt
