#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>

#define MAX_TEXT 64

typedef struct {
    long msg_type;  // тип сообщения (приоритет)
    char text[MAX_TEXT]; // текст сообщения
    pid_t sender_pid; // идентификатор процесса отправителя
} message_t;

#endif // MESSAGE_H