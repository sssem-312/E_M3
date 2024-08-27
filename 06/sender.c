#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "message.h"

int main() {
    key_t key = ftok("chat_queue", 65); // Создание уникального ключа для очереди
    int msg_id = msgget(key, 0666 | IPC_CREAT); // Создание очереди сообщений

    message_t msg;
    msg.sender_pid = getpid(); // Запоминаем свой pid

    while (1) {
        printf("\tВы: ");
        fgets(msg.text, MAX_TEXT, stdin);
        msg.msg_type = 1; // тип сообщения

        // Отправка сообщения
        msgsnd(msg_id, &msg, sizeof(msg), 0);
        printf("\tСообщение отправлено.\n");

        // Получение сообщения от получателя
        msgrcv(msg_id, &msg, sizeof(msg), 2, 0); // ждем сообщения с типом 2
        printf("\tПолучено: %s", msg.text);

        // Проверка завершения
        if (strcmp(msg.text, "exit\n") == 0) {
            break;
        }
    }

    // Очистка очереди сообщений
    msgctl(msg_id, IPC_RMID, NULL);
    return 0;
}