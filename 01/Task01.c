#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void calculate_areas(int *sides, int count) {
    for (int i = 0; i < count; i++) {
        int area = sides[i] * sides[i];
        printf(" Сторона: %d, Площадь: %d\n", sides[i], area);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, " Запуск программы: %s <length1> <length2> ... <lengthN>\n", argv[0]);
        return 1;
    }

    int count = argc - 1;
    int *sides = (int *)malloc(count * sizeof(int));
    
    // Преобразование аргументов в целые числа
    for (int i = 0; i < count; i++) {
        sides[i] = atoi(argv[i + 1]);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        free(sides);
        return 1;
    }

    if (pid == 0) {
        int half = count / 2;
        printf(" Дочерний:\n");
        calculate_areas(sides + half, count - half);
    } else {
        int half = count / 2;
        printf(" Родительский:\n");
        calculate_areas(sides, half);
        wait(NULL);
    }
    free(sides);
    return 0;
}