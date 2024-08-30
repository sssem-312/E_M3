#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX_FILENAME_LENGTH 20
#define PIPE_READ 0
#define PIPE_WRITE 1

int sem_id = -1; // Идентификатор семафора

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

void semaphore_wait(int sem_id) { // Блокировка процесса
    struct sembuf sb = { 0, -1, 0 }; // Декремент
    semop(sem_id, &sb, 1);
}

void semaphore_signal(int sem_id) { // Разблокировка процесса
    struct sembuf sb = { 0, 1, 0 }; // Инкремент
    semop(sem_id, &sb, 1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) { // Ожидаем 2 аргумента: количество повторов, имя файла
        fprintf(stderr, "Запуск: %s <количество повторов> <имя файла>\n", argv[0]);
        exit(1);
    }

    int num_repeats = atoi(argv[1]); // Количество повторов
    char filename[MAX_FILENAME_LENGTH]; // Имя файла
    strncpy(filename, argv[2], MAX_FILENAME_LENGTH);
    filename[MAX_FILENAME_LENGTH - 1] = '\0'; // Обеспечить завершение строки

    // Создаем семафор
    sem_id = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (sem_id == -1) {
        perror("Ошибка! Семафор не создан");
        exit(1);
    }

    // Инициализируем семафор значением 1 (разрешаем доступ)
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("Ошибка! Семафор не проинициализирован");
        exit(1);
    }

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
        close(pipefd[PIPE_READ]); // Закрыть дескриптор чтения в дочернем процессе

        for (int i = 0; i < num_repeats + 1; i++) {
            semaphore_wait(sem_id); // Блокируем доступ к файлу для других
            read_from_file(filename); // Читаем и выводим данные из файла
            sleep(1);

            // Генерируем новое случайное число и отправляем его, если это не последний повтор
            if (i < num_repeats) {
                int random_number = rand() % 100;
                printf("Дочерний процесс сгенерировал новое число: %d\n", random_number);
                write(pipefd[PIPE_WRITE], &random_number, sizeof(random_number)); // Отправляем число родительскому процессу
            }

            semaphore_signal(sem_id); // Освобождаем доступ к файлу
            sleep(1);
        }

        close(pipefd[PIPE_WRITE]); // Закрываем дескриптор записи в дочернем процессе
        exit(0); // Завершаем дочерний процесс
    } else { // Родительский процесс
        close(pipefd[PIPE_WRITE]); // Закрыть дескриптор записи в родительском процессе

        for (int i = 0; i < num_repeats; i++) { // Ограничение на количество повторов
            int random_number;
            read(pipefd[PIPE_READ], &random_number, sizeof(random_number)); // Читаем случайное число от дочернего процесса
            semaphore_wait(sem_id); // Запрашиваем доступ к файлу
            printf("Родительский процесс модифицирует файл...\n");
            write_to_file(filename, random_number); // Записываем это число в файл
            sleep(1);
            semaphore_signal(sem_id); // Освобождаем доступ к файлу
        }

        close(pipefd[PIPE_READ]); // Закрываем дескриптор чтения в родительском процессе
        wait(NULL); // Ждем завершения дочернего процесса
    }

    semctl(sem_id, 0, IPC_RMID); // Удаляем семафор

    return 0;
}
