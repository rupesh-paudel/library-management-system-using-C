#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
struct books {
    int id;
    char bookName[50];
    char authorName[50];
    char date[12];
} b;

struct student {
    int id;
    char sName[50];
    char sClass[50];
    int sRoll;
    char bookName[50];
    char date[12];
} s;

FILE* fp;

void addBook();
void booksList();
void del();
void issueBook();
void issueList();
void returnBook();

int main() {
    int ch;

    while (1) {
        system("cls");
        printf("<== Library Management System ==>\n");
        printf("1. Add Book\n");
        printf("2. Books List\n");
        printf("3. Remove Book\n");
        printf("4. Issue Book\n");
        printf("5. Issued Book List\n");
        printf("6. Return Book and Calculate Fine\n");
        printf("0. Exit\n\n");
        printf("Enter your choice: ");
        scanf("%d", &ch);

        switch (ch) {
        case 0:
            exit(0);

        case 1:
            addBook();
            break;

        case 2:
            booksList();
            break;

        case 3:
            del();
            break;

        case 4:
            issueBook();
            break;

        case 5:
            issueList();
            break;

        case 6:
            returnBook();
            break;

        default:
            printf("Invalid Choice...\n\n");
        }

        printf("Press Any Key To Continue...");
        getch();
    }

    return 0;
}

void addBook() {
    char myDate[12];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(myDate, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    strcpy(b.date, myDate);

    fp = fopen("books.txt", "ab");

    printf("Enter book id: ");
    scanf("%d", &b.id);

    printf("Enter book name: ");
    fflush(stdin);
    gets(b.bookName);

    printf("Enter author name: ");
    fflush(stdin);
    gets(b.authorName);

    printf("Book Added Successfully\n");

    fwrite(&b, sizeof(b), 1, fp);
    fclose(fp);
}

void booksList() {
    system("cls");
    printf("<== Available Books ==>\n\n");
    printf("%-10s %-30s %-20s %s\n\n", "Book id", "Book Name", "Author", "Date");

    fp = fopen("books.txt", "rb");
    while (fread(&b, sizeof(b), 1, fp) == 1) {
        printf("%-10d %-30s %-20s %s\n", b.id, b.bookName, b.authorName, b.date);
    }

    fclose(fp);
}

void del() {
    int id, f = 0;
    system("cls");
    printf("<== Remove Books ==>\n\n");
    printf("Enter Book id to remove: ");
    scanf("%d", &id);

    FILE* ft;

    fp = fopen("books.txt", "rb");
    ft = fopen("temp.txt", "wb");

    while (fread(&b, sizeof(b), 1, fp) == 1) {
        if (id == b.id) {
            f = 1;
        } else {
            fwrite(&b, sizeof(b), 1, ft);
        }
    }

    if (f == 1) {
        printf("\n\nDeleted Successfully.");
    } else {
        printf("\n\nRecord Not Found !");
    }

    fclose(fp);
    fclose(ft);

    remove("books.txt");
    rename("temp.txt", "books.txt");
}

void issueList() {
    system("cls");
    printf("<== Book Issue List ==>\n\n");

    printf("%-10s %-30s %-20s %-10s %-30s %s\n\n", "S.id", "Name", "Class", "Roll", "Book Name", "Date");

    fp = fopen("issue.txt", "rb");
    while (fread(&s, sizeof(s), 1, fp) == 1) {
        printf("%-10d %-30s %-20s %-10d %-30s %s\n", s.id, s.sName, s.sClass, s.sRoll, s.bookName, s.date);
    }

    fclose(fp);
}


void issueBook() {
    char myDate[12];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(myDate, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    strcpy(s.date, myDate);

    int f = 0;

    system("cls");
    printf("<== Issue Books ==>\n\n");

    printf("Enter Book id to issue: ");
    scanf("%d", &s.id);

    fp = fopen("books.txt", "rb");

    while (fread(&b, sizeof(b), 1, fp) == 1) {
        if (b.id == s.id) {
            s.sRoll = b.id; // Record the book ID as the student's roll
            strcpy(s.bookName, b.bookName);
            f = 1;
            break;
        }
    }

    if (f == 0) {
        printf("No book found with this id\n");
        printf("Please try again...\n\n");
        return;
    }

    printf("Enter Student ID: ");
    scanf("%d", &s.id);

    printf("Enter Student Name: ");
    fflush(stdin);
    gets(s.sName);

    printf("Enter Student Class: ");
    fflush(stdin);
    gets(s.sClass);

    printf("Book Issued Successfully\n\n");

    fp = fopen("issue.txt", "ab");
    fwrite(&s, sizeof(s), 1, fp);
    fclose(fp);
}


void returnBook() {
    int studentId, bookId;
    int found = 0;
    int fine = 0;

    system("cls");
    printf("<== Return Book ==>\n\n");
    printf("Enter Student ID: ");
    scanf("%d", &studentId);
    printf("Enter Book ID: ");
    scanf("%d", &bookId);

    FILE* issueFile = fopen("issue.txt", "rb");
    FILE* tempFile = fopen("temp.txt", "wb");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    while (fread(&s, sizeof(s), 1, issueFile) == 1) {
        if (s.id == studentId && s.sRoll == bookId) {
            found = 1;

            char returnDate[12];
            sprintf(returnDate, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

            int day, month, year;
            sscanf(s.date, "%02d/%02d/%d", &day, &month, &year);
            struct tm dueDate_tm = {0};
            dueDate_tm.tm_mday = day + 15; // Assuming 15 days due period
            dueDate_tm.tm_mon = month - 1;
            dueDate_tm.tm_year = year - 1900;

            time_t dueTime = mktime(&dueDate_tm);
            time_t returnTime = mktime(&tm);

            double diffInSeconds = difftime(returnTime, dueTime);
            int daysOverdue = (int)(diffInSeconds / (60 * 60 * 24));

            if (daysOverdue > 0) {
                fine = daysOverdue * 10; // Rs.10 per day overdue
                printf("Book is overdue by %d days.\n", daysOverdue);
            } else {
                printf("Book returned on time.\n");
            }
        } else {
            fwrite(&s, sizeof(s), 1, tempFile);
        }
    }

    fclose(issueFile);
    fclose(tempFile);

    if (!found) {
        printf("Book with Student ID %d and Book ID %d was not found in the issued books.\n", studentId, bookId);
    } else {
        remove("issue.txt");
        rename("temp.txt", "issue.txt");
        if (fine > 0) {
            printf("Fine to be collected: Rs.%d\n", fine);
        }
    }
}



