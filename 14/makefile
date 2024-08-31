TARGET_SERVER = udp_server
TARGET_CLIENT = udp_client
CC = gcc
CFLAGS = -Wall -g
SRCS_SERVER = udp_server.c
SRCS_CLIENT = udp_client.c

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): $(SRCS_SERVER)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_CLIENT): $(SRCS_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT)
