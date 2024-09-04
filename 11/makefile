TARGET = task11
CC = gcc
CFLAGS = -Wall -Wextra -pedantic
SRCS = task11.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	
.PHONY: all clean