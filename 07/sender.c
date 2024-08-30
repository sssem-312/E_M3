#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include "message.h"

#define QUEUE_NAME "/chat"

int main() {
    mqd_t mq; // Дескриптор
    struct mq_attr attr;

    // Атрибуты очереди
    attr.mq_flags = 0; // Нет флагов
    attr.mq_maxmsg = 10; // Максимальное количество сообщений в очереди
    attr.mq_msgsize = sizeof(message_t); // Максимальный размер сообщения
    attr.mq_curmsgs = 0; // Текущее количество сообщений в очереди

    // Создание или открытие очереди сообщений
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("Ошибка при открытии очереди");
        exit(EXIT_FAILURE);
    } else {
        printf("Очередь открыта!\n");
    }

    message_t msg;
    msg.sender_pid = getpid(); // Запоминаем свой PID

    while (1) {
        printf("\tВы: ");
        fgets(msg.text, MAX_TEXT, stdin);
        msg.msg_type = 1; // Тип сообщения

        // Проверка завершения
        if (strcmp(msg.text, "exit\n") == 0) {
            mq_send(mq, (const char*)&msg, sizeof(msg), 0); // Отправляем сообщение о выходе
            break;
        }

        // Отправка сообщения
        if (mq_send(mq, (const char*)&msg, sizeof(msg), 0) == -1) {
            perror("Ошибка при отправке сообщения");
        } else {
            printf("\tСообщение отправлено.\n");
        }

        // Получение сообщения от получателя
        if (mq_receive(mq, (char*)&msg, sizeof(msg), NULL) == -1) {
            perror("Ошибка при получении сообщения");
            continue;
        }

        printf("\tПолучено: %s", msg.text);

        if (strcmp(msg.text, "exit\n") == 0) {
            printf("Получено сообщение 'exit'. Завершение программы.\n");
            break;
        }
    }

    // Закрытие очереди
    if (mq_close(mq) == -1) {
        perror("Ошибка при закрытии очереди");
    } else {
        printf("Очередь закрыта!\n");
    }

    // Удаление очереди сообщений
    if (mq_unlink(QUEUE_NAME) == -1) {
        perror("Ошибка при удалении очереди");
    } else {
        printf("Очередь удалена!\n");
    }

    return 0;
}