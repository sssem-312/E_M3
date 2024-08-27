CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGETS = sender receiver
SOURCES = sender.c receiver.c

all: $(TARGETS)

sender: sender.o message.h
	$(CC) $(CFLAGS) -o $@ $^

receiver: receiver.o message.h
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c message.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGETS) *.o

.PHONY: clean