#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: ./client <IP_address> <port>\n");
        exit(EXIT_FAILURE);
    }

    int sockfd = -1; // Инициализация дескриптора
    char line[1000]; // Массив для принятой и отсылаемой строки
    struct sockaddr_in servaddr; // Структура для адреса сервера

    // Создаем UDP сокет
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    // Заполняем структуру для адреса сервера
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    int port = atoi(argv[2]);
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    while (1) {
        printf("Введите сообщение: ");
        fgets(line, sizeof(line), stdin);

        // Отправка сообщения на сервер
        sendto(sockfd, line, strlen(line) + 1, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

        // Ожидаем ответа от сервера
        if (recvfrom(sockfd, line, sizeof(line), 0, NULL, NULL) < 0) {
            perror(NULL);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("Ответ от сервера: %s\n", line);
    }

    close(sockfd);
    return 0;
}