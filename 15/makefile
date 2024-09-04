TARGET_SERVER = tcp_server
TARGET_CLIENT = tcp_client
CC = gcc
CFLAGS = -Wall -g
SRCS_SERVER = tcp_server.c
SRCS_CLIENT = tcp_client.c

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): $(SRCS_SERVER)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_CLIENT): $(SRCS_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT)