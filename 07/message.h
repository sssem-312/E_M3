#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>
#include <stdio.h>

#define MAX_TEXT 128

typedef struct {
    long msg_type;  // тип сообщения (приоритет)
    char text[MAX_TEXT]; // текст сообщения
    pid_t sender_pid; // идентификатор процесса отправителя
} message_t;

#endif // MESSAGE_H