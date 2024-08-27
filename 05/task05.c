#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

volatile sig_atomic_t file_accessible = true; // Флаг для отслеживания доступа к файлу

void sigusr1_handler(int signum) {
    file_accessible = false; // Блокировка доступа к файлу при получении сигнала SIGUSR1
}

void sigusr2_handler(int signum) {
    file_accessible = true; // Разрешение доступа к файлу при получении сигнала SIGUSR2
}

void generate_random_numbers(int count, int write_fd) {
    for (int i = 0; i < count; ++i) {
        int random_number = rand() % 100;
        write(write_fd, &random_number, sizeof(random_number));  // Отправка числа в родительский процесс
        sleep(1); // Задержка для имитации времени генерации
    }
    close(write_fd);
}

void write_to_file(const char* filename, int read_fd) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int random_number;
    while (read(read_fd, &random_number, sizeof(random_number)) > 0) {
        printf("Случайное число: %d (записано в файл)\n", random_number);
        fprintf(file, "%d\n", random_number);  // Запись числа в файл
        fflush(file); // Гарантия, что данные записаны в файл немедленно
    }

    fclose(file);
    close(read_fd);
    printf("Записано\n");
}

void read_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int number;
    while (fscanf(file, "%d", &number) != EOF) {
        printf("Дочерний процесс читает из файла: %d\n", number);
        sleep(1); // Задержка, чтобы не нагружать систему
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <count> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int count = atoi(argv[1]);  // Количество случайных чисел
    const char* filename = argv[2];  // Имя файла для записи

    // Установка обработчиков сигналов
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);

    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {  // Дочерний процесс
        close(fd[0]);  // Закрываем конец для чтения
        srand(time(NULL));
        generate_random_numbers(count, fd[1]);

        // После завершения генерации чисел дочерний процесс читает файл
        while (!file_accessible) {
            // Ожидание, пока файл не станет доступным
            sleep(1);
        }

        read_from_file(filename);
    } else {  // Родительский процесс
        close(fd[1]);  // Закрыть конец для записи
        write_to_file(filename, fd[0]);

        // Блокировка доступа к файлу для дочернего процесса перед изменением
        kill(pid, SIGUSR1);
        // Запись данных или изменение файла
        sleep(2); // Имитация времени изменения файла
        // Завершив изменение, разрешен доступ
        kill(pid, SIGUSR2);
    }

    return EXIT_SUCCESS;
}