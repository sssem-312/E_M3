#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <fcntl.h>   // Для open
#include <unistd.h>  // Для read и write
#include <sys/stat.h> // Для mode_t
  
#define L_NAME 10 
#define L_NUMBER 15 
#define ETC 20
#define MAX_CONTACTS 100
 
typedef struct 
{ 
 char firstName[L_NAME]; 
 char secondName[L_NAME]; 
 char lastName[L_NAME]; 
} Name; 
 
typedef struct 
{ 
 int flag; 
 char phoneNumberMore[L_NUMBER]; 
 char jobPlace[ETC]; 
 char jobTitle[ETC]; 
 char eMailAddressMain[ETC]; 
 char eMailAddressMore[ETC]; 
 char linkNetwork[ETC]; 
} Info; 
 
struct Contact 
{ 
 int id; 
 Name name; 
 char phoneNumberMain[L_NUMBER]; 
 Info info; 
}; 
 
struct PhoneBook 
{ 
    struct Contact TBL [MAX_CONTACTS]; 
    int n; 
}; 

void saveToFile(struct PhoneBook* a);
void loadFromFile(struct PhoneBook* a);
void SetContact(struct PhoneBook* a, int i);
void SetContacts(struct PhoneBook* a, int n);
void Print(struct PhoneBook a);
void Delete(struct PhoneBook* a, int n);
void menu();