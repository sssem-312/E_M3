CC = gcc
CFLAGS = -Wall -Wextra
EXEC = task02 sum mult max
SRC = task02.c sum.c mult.c max.c

all: $(EXEC)

task02: task02.c
	$(CC) $(CFLAGS) -o task02 task02.c

sum: sum.c
	$(CC) $(CFLAGS) -o sum sum.c

mult: mult.c
	$(CC) $(CFLAGS) -o mult mult.c

max: max.c
	$(CC) $(CFLAGS) -o max max.c

clean:
	rm -f $(EXEC)

.PHONY: all clean