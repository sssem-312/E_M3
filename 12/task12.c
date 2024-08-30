#include <stdio.h>       
#include <stdlib.h>     
#include <signal.h>     
#include <string.h>    
#include <unistd.h>    
#include <sys/ipc.h>   
#include <sys/shm.h> 
#include <time.h>      
#include <sys/types.h> 
#include <wait.h>  

#define MAX_NUMBERS 10   // Максимальное количество чисел в одном наборе
#define SHM_SIZE sizeof(struct shared_data)  // Размер разделяемой памяти

// Структура для хранения данных в разделяемой памяти
struct shared_data {
    int numbers[MAX_NUMBERS]; // Массив для хранения случайных чисел
    int count;                // Количество чисел в массиве
    int max;                  // Максимальное значение из массива
    int min;                  // Минимальное значение из массива
    int processed_sets;       // Счетчик обработанных наборов
};

struct shared_data *data; 
int shmid;

// Функция для обработки сигнала SIGINT
void handle_sigint(int sig) {
    printf("\nКоличество обработанных наборов: %d\n", data->processed_sets);
    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);

    // Создание разделяемой памяти
    shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Присоединяем разделяемую память
    data = (struct shared_data *)shmat(shmid, NULL, 0);
    if (data == (struct shared_data *)(-1)) {
        perror("shmat");
        exit(1);
    }

    data->processed_sets = 0;
    srand(time(NULL));

    while (1) {
        data->count = rand() % MAX_NUMBERS + 1; 
        for (int i = 0; i < data->count; i++) {
            data->numbers[i] = rand() % 100;
        }

        pid_t pid = fork(); 

        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) { // Дочерний процесс
            // Инициализируем max и min первым элементом массива
            data->max = data->min = data->numbers[0]; 
            // Находим максимальное и минимальное значения
            for (int i = 1; i < data->count; i++) {
                if (data->numbers[i] > data->max) data->max = data->numbers[i];
                if (data->numbers[i] < data->min) data->min = data->numbers[i];
            }
            data->processed_sets++; 
            // Отключаем разделяемую память в дочернем процессе
            shmdt(data);
            exit(0);
        } else { // Родительский процесс
            wait(NULL);
            printf("Найденные значения: max = %d, min = %d\n", data->max, data->min);
        }
        sleep(1); 
    }

    return 0;
}