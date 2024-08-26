#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(" Не переданы аргументы.\n");
        return 1;
    }

    int mult = 1;
    for (int i = 1; i < argc; i++) {
        mult *= atoi(argv[i]);
    }
    printf(" Произведение: %d\n", mult);
    return 0;
}