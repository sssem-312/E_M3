#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(" Не переданы аргументы.\n");
        return 1;
    }

    int max = atoi(argv[1]);
    for (int i = 2; i < argc; i++) {
        int num = atoi(argv[i]);
        if (num > max) {
            max = num;
        }
    }
    printf(" Максимальное значение: %d\n", max);
    return 0;
}