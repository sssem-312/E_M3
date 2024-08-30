#include <stdio.h>       
#include <stdlib.h>     
#include <signal.h>     
#include <string.h>    
#include <unistd.h>    
#include <sys/mman.h>   // Для POSIX shared memory
#include <fcntl.h>      // Для использования O_* флагов
#include <time.h>      
#include <sys/types.h> 
#include <wait.h>      

#define MAX_NUMBERS 10   // Максимальное количество чисел в одном наборе
#define SHM_NAME "/my_shared_memory" // Имя разделяемой памяти

// Структура для хранения данных в разделяемой памяти
struct shared_data {
    int numbers[MAX_NUMBERS]; // Массив для хранения случайных чисел
    int count;                // Количество чисел в массиве
    int max;                  // Максимальное значение из массива
    int min;                  // Минимальное значение из массива
    int processed_sets;       // Счетчик обработанных наборов
};

struct shared_data *data; 

// Функция для обработки сигнала SIGINT
void handle_sigint(int sig) {
    printf("\nКоличество обработанных наборов: %d\n", data->processed_sets);
    shm_unlink(SHM_NAME); // Удаляем разделяемую память
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);

    // Создание разделяемой памяти
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        exit(1);
    }

    // Устанавливаем размер разделяемой памяти
    ftruncate(shm_fd, sizeof(struct shared_data));

    // Присоединяем разделяемую память
    data = (struct shared_data *)mmap(0, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
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
            munmap(data, sizeof(struct shared_data)); // Отключаем разделяемую память
            exit(0);
        } else { // Родительский процесс
            wait(NULL);
            printf("Найденные значения: max = %d, min = %d\n", data->max, data->min);
        }
        sleep(1); 
    }
    shm_unlink(SHM_NAME);
    return 0;
}