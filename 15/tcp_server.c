#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/mman.h>

// Функция для обслуживания подключившихся пользователе
void dostuff(int sock, int shm);

// Функция обработки ошибок
void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// Количество активных пользователей
int nclients = 0;

// Печать количества активных пользователей
void printusers() {
    if (nclients) {
        printf("%d user(s) on-line\n", nclients);
    } else {
        printf("No user on-line!\n");
    }
}

// Функция обработки данных
int myfunc(int a, int b, char c) {
    switch(c) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        default:
            return 0;
    }
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;              // Дескрипторы сокетов
    int portno;                         // Номер порта
    int pid;                            // ID номер потока
    socklen_t clilen;                   // Размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // Структура сокета сервера и клиента

    printf("TCP SERVER DEMO\n");

    // Ошибка в случае если не указан порт
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(EXIT_FAILURE);
    }
    
    // Шаг 1 - создание сокета
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    
    // Шаг 2 - связывание сокета с локальным адресом
    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Сервер принимает подключения на все IP-адреса
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
    // Шаг 3 - ожидание подключений, размер очереди - 5
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    
    // Создание общей ячейки памяти для хранения количества активных пользователей
    int shm = shm_open("/shm", O_CREAT | O_RDWR, 0660);
    if (shm == -1) 
        error("Shared memory open");
    if (ftruncate(shm, sizeof(int)) == -1) 
        error("Shared memory truncate");
    char* addr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    memcpy(addr, &nclients, sizeof(nclients));

    // Шаг 4 - извлекаем сообщение из очереди (цикл извлечения запросов на подключение)
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");
        memcpy(&nclients, addr, sizeof(nclients));
        nclients++;
        memcpy(addr, &nclients, sizeof(nclients));

        // вывод сведений о клиенте
        struct hostent *hst;
        hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s] new connect!\n",
            (hst) ? hst->h_name : "Unknown host",
            inet_ntoa(cli_addr.sin_addr));
        printusers();

        pid = fork();
        if (pid < 0) 
            error("ERROR on fork");
        if (pid == 0) {
            close(sockfd);
            dostuff(newsockfd, shm);
            exit(EXIT_SUCCESS);
        }
        else close(newsockfd);
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
}

void dostuff (int sock, int shm) {
    int bytes_recv;         // Размер принятого сообщения
    int a,b;                // Переменные для myfunc
    char buff[20 * 1024];
    char calcBuff[10];
    char* addr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    #define str1 "Enter 1 parameter\n"
    #define str2 "Enter 2 parameter\n"
    #define str3 "Enter sign\n"

    while (1) {
        bytes_recv = read(sock, &calcBuff[0], sizeof(calcBuff));
        if (bytes_recv < 0) error("ERROR reading from socket");
        if (strcmp(calcBuff, "calc\n")) break;

        // Отправляем клиенту сообщение
        write(sock, str1, sizeof(str1));
        
        // Обработка первого параметра
        bytes_recv = read(sock, &buff[0], sizeof(buff));
        if (bytes_recv < 0) error("ERROR reading from socket");
        if (!strcmp(buff, "quit\n")) break;
        
        a = atoi(buff); // Преобразование первого параметра в int

        // Отправляем клиенту сообщение
        write(sock, str2, sizeof(str2));

        // Обработка второго параметра
        bytes_recv = read(sock, &buff[0], sizeof(buff));
        if (bytes_recv < 0) error("ERROR reading from socket");
        if (!strcmp(buff, "quit\n")) break;

        b = atoi(buff); // Преобразование второго параметра в int

        // Отправляем клиенту сообщение
        write(sock, str3, sizeof(str3));

        // Обработка знака
        bytes_recv = read(sock, &buff[0], sizeof(buff));
        if (bytes_recv < 0) error("ERROR reading from socket");
        if (!strcmp(buff, "quit\n")) break;

        a = myfunc(a, b, buff[0]); // Вызов пользовательской функции
        sprintf(buff, "%d", a); // Преобразование результата в строку
        buff[strlen(buff)] = '\n'; // Добавление к сообщению символа конца строки
;
        write(sock, &buff[0], strlen(buff) + 1);
    }
    memcpy(&nclients, addr, sizeof(nclients));
    nclients--; // Уменьшаем счетчик активных клиентов
    memcpy(addr, &nclients, sizeof(nclients));
    close(sock);
    printf("-disconnect\n");
    printusers();
    return;
}



