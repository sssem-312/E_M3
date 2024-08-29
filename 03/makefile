CC = gcc
CFLAGS = -Wall -Wextra
SRC = main.c task03.c
OBJ = $(SRC:.c=.o)
TARGET = task03

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c task03.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean