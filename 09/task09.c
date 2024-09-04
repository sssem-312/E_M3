#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_FILENAME_LENGTH 20

void write_to_file(const char *filename, int number) {
    FILE *file = fopen(filename, "a+");
    if (file == NULL) {
        perror("Невозможно открыть файл для записи");
        exit(1);
    }
    fprintf(file, "%d\n", number);
    fclose(file);
}

void read_from_file(const char *filename) {
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

// Функция для установки значений семафоров
void sem_set_value(int semid, int val) {
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;

    arg.val = val;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("Ошибка установки значения семафора");
        exit(EXIT_FAILURE);
    }
}

// Функция для выполнения операций с семафорами
void sem_operation(int semid, int op) {
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = op; 
    sop.sem_flg = 0;
    if (semop(semid, &sop, 1) == -1) {
        perror("Ошибка операции с семафором");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Запуск: %s <количество повторов> <имя файла>\n", argv[0]);
        exit(1);
    }

    key_t key_write = ftok("file", 'W'); // создаем уникальный ключ
    key_t key_read = ftok("file", 'R');

    int semid_write = semget(key_write, 1, IPC_CREAT | 0666);
    int semid_read = semget(key_read, 1, IPC_CREAT | 0666);

    sem_set_value(semid_write, 1); // инициализуем семафор на запись значением 1
    sem_set_value(semid_read, 3);   // инициализуем семафор на чтение значением 3

    int num_repeats = atoi(argv[1]);
    char filename[MAX_FILENAME_LENGTH];
    strncpy(filename, argv[2], MAX_FILENAME_LENGTH);
    filename[MAX_FILENAME_LENGTH - 1] = '\0'; 

    int pipefd[2];
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
        close(pipefd[0]); 

        for (int i = 0; i < num_repeats + 1; i++) {
            sem_operation(semid_read, -1); // Блокировка семафора для чтения
            printf("Происходит чтение\n");
            read_from_file(filename);
            sleep(1);

            if (i < num_repeats) {
                int random_number = rand() % 100;
                printf("Дочерний процесс сгенерировал новое число: %d\n", random_number);
                write(pipefd[1], &random_number, sizeof(random_number)); 
            }

            sem_operation(semid_read, 1); // Разблокировка семафора для чтения
            printf("Файл свободен\n");
            sleep(1);
        }

        close(pipefd[1]);
        exit(0);
    } 
    
    // Родительский процесс
    close(pipefd[1]);
    for (int i = 0; i < num_repeats; i++) {
        int random_number;
        read(pipefd[0], &random_number, sizeof(random_number));
        
        sem_operation(semid_write, -1); // Блокировка семафора для записи
        printf("Происходит запись\n");
        
        printf("Родительский процесс модифицирует файл...\n");
        write_to_file(filename, random_number);
        sleep(1);
        
        sem_operation(semid_write, 1); // Разблокировка семафора для записи
        printf("Файл свободен\n");
    }
    
    close(pipefd[0]); 
    wait(NULL);

    // Удаление семафоров
    semctl(semid_write, 0, IPC_RMID);
    semctl(semid_read, 0, IPC_RMID);

    return 0;
}