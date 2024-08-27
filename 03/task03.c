#include "task03.h"

// Сохранение контактов в файл в конце
void save_contacts(PhoneBook *pb) {
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Ошибка при открытии файла для записи");
        return;
    }
    write(fd, pb, sizeof(PhoneBook));
    close(fd);
}

// Загрузка контактов из файла в начале
int load_contacts(PhoneBook *pb) {
    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1) {
        // Файл может не существовать при первом запуске
        return 0;
    }
    int read_count = read(fd, pb, sizeof(PhoneBook));
    close(fd);
    return read_count / sizeof(Contact);
}

void SetContact(PhoneBook* a, int i) {
    printf(" Имя: ");
    read(STDIN_FILENO, a->TBL[i].name.firstName, L_NAME);
    a->TBL[i].name.firstName[strcspn(a->TBL[i].name.firstName, "\n")] = 0;

    printf(" Отчество: ");
    read(STDIN_FILENO, a->TBL[i].name.secondName, L_NAME);
    a->TBL[i].name.secondName[strcspn(a->TBL[i].name.secondName, "\n")] = 0;

    printf(" Фамилия: ");
    read(STDIN_FILENO, a->TBL[i].name.lastName, L_NAME);
    a->TBL[i].name.lastName[strcspn(a->TBL[i].name.lastName, "\n")] = 0;

    printf(" Номер телефона: ");
    read(STDIN_FILENO, a->TBL[i].phoneNumberMain, L_NUMBER);
    a->TBL[i].phoneNumberMain[strcspn(a->TBL[i].phoneNumberMain, "\n")] = 0;

    printf(" Хотите ли вы внести дополнительные данные? y/n ");
    char answer[3];
    read(STDIN_FILENO, answer, 3);
    if (answer[0] == 'y') {
        printf(" Дополнительный номер телефона: ");
        read(STDIN_FILENO, a->TBL[i].info.phoneNumberMore, L_NUMBER);
        a->TBL[i].info.phoneNumberMore[strcspn(a->TBL[i].info.phoneNumberMore, "\n")] = 0;

        printf(" Место работы: ");
        read(STDIN_FILENO, a->TBL[i].info.jobPlace, ETC);
        a->TBL[i].info.jobPlace[strcspn(a->TBL[i].info.jobPlace, "\n")] = 0;

        printf(" Должность: ");
        read(STDIN_FILENO, a->TBL[i].info.jobTitle, ETC);
        a->TBL[i].info.jobTitle[strcspn(a->TBL[i].info.jobTitle, "\n")] = 0;

        printf(" Адрес EMail: ");
        read(STDIN_FILENO, a->TBL[i].info.eMailAddressMain, ETC);
        a->TBL[i].info.eMailAddressMain[strcspn(a->TBL[i].info.eMailAddressMain, "\n")] = 0;

        printf(" Доп. адрес EMail: ");
        read(STDIN_FILENO, a->TBL[i].info.eMailAddressMore, ETC);
        a->TBL[i].info.eMailAddressMore[strcspn(a->TBL[i].info.eMailAddressMore, "\n")] = 0;

        printf(" Ссылки на соц.сети: ");
        read(STDIN_FILENO, a->TBL[i].info.linkNetwork, ETC);
        a->TBL[i].info.linkNetwork[strcspn(a->TBL[i].info.linkNetwork, "\n")] = 0;

        a->TBL[i].info.flag = 1;
    } else {
        a->TBL[i].info.flag = 0; // Сброс флага, если нет дополнительных данных
    }
    printf("\n Данные о контакте сохранены!\n");
}

void SetContacts(PhoneBook* a, int n) {
    for (int i = a->n; i < a->n + n; i++) {
        printf(" Контакт %d: \n", i + 1);
        SetContact(a, i);
    }
    a->n += n;
    printf("\n Данные о контактах добавлены! \n\n");
}

void Print(PhoneBook a) {
    for (int i = 0; i < a.n; i++) {
        printf(" * Контакт %d *****\n", i + 1);
        printf(" Имя: %s\n", a.TBL[i].name.firstName);
        printf(" Отчество: %s\n", a.TBL[i].name.secondName);
        printf(" Фамилия: %s\n", a.TBL[i].name.lastName);
        printf(" Номер телефона: %s\n", a.TBL[i].phoneNumberMain);
        if (a.TBL[i].info.flag == 1) {
            printf(" Дополнительный номер телефона: %s\n", a.TBL[i].info.phoneNumberMore);
            printf(" Место работы: %s\n", a.TBL[i].info.jobPlace);
            printf(" Должность: %s\n", a.TBL[i].info.jobTitle);
            printf(" Адрес EMail: %s\n", a.TBL[i].info.eMailAddressMain);
            printf(" Доп. адрес EMail: %s\n", a.TBL[i].info.eMailAddressMore);
            printf(" Ссылки на соц.сети: %s\n", a.TBL[i].info.linkNetwork);
        }
        printf("\n");
    }
}

void Delete(PhoneBook* a, int n) {
    for (int i = n; i < a->n; i++) {
        a->TBL[i] = a->TBL[i + 1];
    }
    (a->n)--;
}

void menu() {
    printf(" ---------------- Меню -----------------\n");
    printf(" [1] Добавить контакт\n");
    printf(" [2] Список контактов\n");
    printf(" [3] Удалить контакт\n");
    printf(" [0] Выход\n");
    printf(" ---------------------------------------\n");
}