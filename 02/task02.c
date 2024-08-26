#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 

#define MAX_INPUT_SIZE 64
#define MAX_ARGS 10

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];
    
    while (1) {
        printf(">> ");
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        input[strcspn(input, "\n")] = 0; 
        int argc = 0;
        char *token = strtok(input, " ");
        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;

        if (fork() == 0) {
            execvp(args[0], args);
            perror("Ошибка при выполнении программы");
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
        }
    }
    return 0;
}