
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>
#include <wait.h>

#define MAX_FILENAME_LENGTH 20

void write_to_file(const char *filename, int number) { // Запись в файл
    FILE *file = fopen(filename, "a+"); // чтение/добавление/создание
    if (file == NULL) {
        perror("Невозможно открыть файл для записи");
        exit(1);
    }

    fprintf(file, "%d\n", number);
    fclose(file);
}

void read_from_file(const char *filename) { // Чтение из файла
    FILE *file = fopen(filename, "a+");
    if (file == NULL) {
        perror("Невозможно открыть файл для чтения");
        exit(1);
    }

    int number;
    printf("Дочерний процесс читает из файла: \n");
    while (fscanf(file, "%d", &number) != EOF) {
        printf("%d\n", number);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) { // Ожидаем 2 аргумента: количество повторов, имя файла
        fprintf(stderr, "Запуск: %s <количество повторов> <имя файла>\n", argv[0]);
        exit(1);
    }

    sem_unlink("/sem_write");
    sem_t* sid_write = sem_open("/sem_write", O_CREAT, 0666, 1);
    if (sid_write == SEM_FAILED) {
        perror("Создание семафора на запись"); //
        exit(EXIT_FAILURE);
    }

    sem_unlink("/sem_read");
    sem_t* sid_read = sem_open("/sem_read", O_CREAT, 0666, 3); // В данном случае устанавливаем 3 для чтения
    if (sid_read == SEM_FAILED) {
        perror("Создание семафора на чтение"); //  
        exit(EXIT_FAILURE);
    }

    int num_repeats = atoi(argv[1]); // Количество повторов
    char filename[MAX_FILENAME_LENGTH]; // Имя файла
    strncpy(filename, argv[2], MAX_FILENAME_LENGTH);
    filename[MAX_FILENAME_LENGTH - 1] = '\0'; // Обеспечить завершение строки

    int pipefd[2]; // Дескрипторы для канала
    if (pipe(pipefd) == -1) {
        perror("Ошибка создания канала");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка при вызове fork");
        exit(1);
    } else if (pid == 0) { // Дочерний процесс
        srand(time(NULL));
        close(pipefd[0]); // Закрыть дескриптор чтения в дочернем процессе

        for (int i = 0; i < num_repeats + 1; i++) {
            if (sem_wait(sid_read) == -1) { // Блокировка файла для чтения
                perror("Блокировка файла");
                exit(EXIT_FAILURE);
            } else printf("Происходит чтение\n");
            read_from_file(filename); // Читаем и выводим данные из файла
            sleep(1);

            // Генерируем новое случайное число и отправляем его, если это не последний повтор
            if (i < num_repeats) {
                int random_number = rand() % 100;
                printf("Дочерний процесс сгенерировал новое число: %d\n", random_number);
                write(pipefd[1], &random_number, sizeof(random_number)); // Отправляем число родительскому процессу
            }

            if (sem_post(sid_read) == -1) { // Разблокировка файла
                perror("Разблокировка файла");
                exit(EXIT_FAILURE);
            } else printf("Файл свободен\n");
            sleep(1);
        }

        close(pipefd[1]); // Закрываем дескриптор записи в дочернем процессе
        exit(0); // Завершаем дочерний процесс
    } 
    
    // Родительский процесс
    close(pipefd[1]); // Закрыть дескриптор записи в родительском процессе
    for (int i = 0; i < num_repeats; i++) { // Ограничение на количество повторов
        int random_number;
        read(pipefd[0], &random_number, sizeof(random_number)); // Читаем случайное число от дочернего процесса
        
        if (sem_wait(sid_write) == -1) { // Блокировка файла для записи
            perror("Блокировка файла");
            exit(EXIT_FAILURE);
        } else printf("Происходит запись\n");
        
        printf("Родительский процесс модифицирует файл...\n");
        write_to_file(filename, random_number); // Записываем это число в файл
        sleep(1);
        
        if (sem_post(sid_write) == -1) { // Разблокировка файла
            perror("Разблокировка файла");
            exit(EXIT_FAILURE);
        } else printf("Файл свободен\n");
        
    }
    
    close(pipefd[0]); // Закрываем дескриптор чтения в родительском процессе
    wait(NULL);
    
    // Закрытие семафоров
    sem_close(sid_write);
    sem_close(sid_read);
    sem_unlink("/sem_write");
    sem_unlink("/sem_read");

    return 0;
}