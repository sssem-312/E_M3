#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CONTACTS 100
#define L_NAME 50
#define L_NUMBER 15
#define ETC 100
#define FILENAME "PhoneBook.dat"

typedef struct {
    char firstName[L_NAME];
    char secondName[L_NAME];
    char lastName[L_NAME];
} Name;

typedef struct {
    bool flag;
    char phoneNumberMore[L_NUMBER];
    char jobPlace[ETC];
    char jobTitle[ETC];
    char eMailAddressMain[ETC];
    char eMailAddressMore[ETC];
    char linkNetwork[ETC];
} Info;

typedef struct Contact {
    int id;
    Name name;
    char phoneNumberMain[L_NUMBER];
    Info info;
} Contact;

typedef struct PhoneBook {
    Contact TBL[MAX_CONTACTS];
    int n;
} PhoneBook;

void Delete(PhoneBook* a, int n);
void Print(PhoneBook a);
void SetContacts(PhoneBook* a, int n);
void SetContact(PhoneBook* a, int i);
int load_contacts(PhoneBook *pb);
void save_contacts(PhoneBook *pb);

void menu();