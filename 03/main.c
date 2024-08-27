#include "task03.h"

int main() {
    PhoneBook pb;
    pb.n = load_contacts(&pb);

    int choice;
    do {
        menu();
        printf("Выберите опцию: ");
        char input[3];
        read(STDIN_FILENO, input, 3);
        choice = atoi(input);

        switch (choice) {
            case 1: {
                int n;
                printf("Сколько контактов вы хотите добавить? ");
                read(STDIN_FILENO, input, 3);
                n = atoi(input);
                SetContacts(&pb, n);
                save_contacts(&pb);
                break;
            }
            case 2:
                Print(pb);
                break;
            case 3: {
                int n;
                printf("Введите номер контакта для удаления: ");
                read(STDIN_FILENO, input, 3);
                n = atoi(input) - 1; // Чтобы соответствовать индексу
                if (n >= 0 && n < pb.n) {
                    Delete(&pb, n);
                    save_contacts(&pb);
                } else {
                    printf("Неверный номер контакта!\n");
                }
                break;
            }
            case 0:
                save_contacts(&pb);
                printf("Выход...\n");
                break;
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
                break;
        }
    } while (choice != 0);

    return 0;
}