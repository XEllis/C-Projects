CFLAGS = -g -Wall -std=c99 -I curse
LDFLAGS = -L curse
LDLIBS = -lncurses -ltinfo

all: sled

tests: test-1 test-2 test-3 test-4

sled: sled.o edit.o document.o

sled.o: edit.h document.h

edit.o: edit.h document.h

document.o: document.h

test-1: test-1.o document.o

test-1.o: document.h

test-2: test-2.o document.o

test-2.o: document.h

test-3: test-3.o edit.o document.o

test-3.o: edit.h document.h

test-4: test-4.o edit.o document.o

test-4.o: edit.h document.h

clean:
	rm -rf *.o
	rm -rf sled test-1 test-2 test-3 test-4
