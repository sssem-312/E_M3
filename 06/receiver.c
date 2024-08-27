#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "message.h"

int main() {
    key_t key = ftok("chat_queue", 65); // Использование того же ключа
    int msg_id = msgget(key, 0666 | IPC_CREAT); // Получение очереди сообщений

    message_t msg;
    msg.sender_pid = getpid(); // запоминаем свой pid

    while (1) {
        // Получение сообщения от отправителя
        msgrcv(msg_id, &msg, sizeof(msg), 1, 0); // ждем сообщения с типом 1
        printf("\tПолучено: %s", msg.text);

        printf("\tВы: ");
        fgets(msg.text, MAX_TEXT, stdin);
        msg.msg_type = 2; // тип сообщения для отправления

        // Отправка ответа
        msgsnd(msg_id, &msg, sizeof(msg), 0);
        printf("\tСообщение отправлено.\n");

        // Проверка завершения
        if (strcmp(msg.text, "exit\n") == 0) {
            break;
        }
    }

    // Очистка очереди сообщений
    msgctl(msg_id, IPC_RMID, NULL);
    return 0;
}