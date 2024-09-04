#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    printf("TCP DEMO CLIENT\n");

    printf("Type \"calc\" to perform calculations or \"exit\" to exit.\n");
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Извлечение порта
    portno = atoi(argv[2]);
    if (portno <= 0) {
        fprintf(stderr, "ERROR: Invalid port number.\n");
        exit(1);
    }
    // Шаг 1 - создание сокета
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0) error("ERROR opening socket");
    // Извлечение хоста
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }
    // Заполнение структуры serv_addr
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
    server->h_length);
    // Установка порта
    serv_addr.sin_port = htons(portno);

    // Шаг 2 - установка соединения
    if (connect(my_sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // Шаг 3 - чтение и передача сообщений
    while (1) {
        char buff[1024];
        // Читаем пользовательский ввод с клавиатуры
        printf("C=>S: ");
        fgets(&buff[0], sizeof(buff) - 1, stdin);
        // передаем строку клиента серверу
        send(my_sock, &buff[0], strlen(&buff[0]), 0);
        if (!strcmp(&buff[0], "quit\n")) {
            // Корректный выход
            printf("Exit...\n");
            close(my_sock);
            exit(EXIT_SUCCESS);
        }
        // получаем сообщение от сервера
        n = recv(my_sock, &buff[0], sizeof(buff) - 1, 0);
        if (n == 0) break;
        printf("S=>C: %s", buff);
    }
    close(my_sock);
    exit(EXIT_SUCCESS);
}

