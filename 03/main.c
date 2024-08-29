#include <stdio.h>
#include <stdlib.h>

#include "task03.h"

int main() {
    setlocale(LC_ALL, "Russian");
    struct PhoneBook a;
    a.n = 0;

    loadFromFile(&a);

    menu();
    int input;
    while (1) {
        printf("\n Что вы хотите сделать? ");
        scanf(" %d", &input);
        printf("\n");
        if (input == 0) {
            // Сохраняем данные в файл перед выходом
            saveToFile(&a);
            break;
        }
        switch (input) {
            case 1: {
                int n;
                printf(" Сколько контактов вы хотите добавить? ");
                scanf(" %d", &n);
                printf("\n");
                SetContacts(&a, n);
                break;
            }
            case 2:
                Print(a);
                break;
            case 3: {
                int m;
                printf(" Какой контакт вы хотите удалить? Выберите номер от 1 до %d ", a.n);
                scanf(" %d", &m);
                printf("\n");
                Delete(&a, m - 1);
                break;
            }
            case 4: {
                int n;
                printf(" Данные по какому контакту вы хотите изменить? Выберите номер от 1 до %d ", a.n);
                scanf(" %d", &n);
                printf("\n");
                SetContact(&a, n - 1);
                break;
            }
            default:
                break;
        }
        menu();
    }
    return 0;
}