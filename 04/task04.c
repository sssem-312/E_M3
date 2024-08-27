#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>

void generate_random_numbers(int count, int write_fd) {
    for (int i = 0; i < count; ++i) {
        int random_number = rand() % 100;
        write(write_fd, &random_number, sizeof(random_number));  // Отправка числа в родительский процесс
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
        printf("%d\n", random_number);
        fprintf(file, "%d\n", random_number);  // Запись числа в файл
    }

    fclose(file);
    close(read_fd);
    printf("Записано\n");
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <count> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int count = atoi(argv[1]);  // Количество случайных чисел
    const char* filename = argv[2];  // Имя файла для записи

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
    } else {  // Родительский процесс
        close(fd[1]);  // Закрываем конец для записи
        write_to_file(filename, fd[0]);
    }

    return EXIT_SUCCESS;
}