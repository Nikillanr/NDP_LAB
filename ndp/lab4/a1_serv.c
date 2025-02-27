#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BOOKS 100
#define BUFFER_SIZE 1024
#define PORT 8888

typedef struct
{
    int accession_number;
    char title[100];
    char author[100];
    int total_pages;
    char publisher[100];
} Book;

Book books[MAX_BOOKS];
int book_count = 0;

void insert_book(char *data)
{
    if (book_count >= MAX_BOOKS)
    {
        printf("Database full. Cannot insert more books.\n");
        return;
    }

    Book new_book;
    sscanf(data, "%d;%99[^;];%99[^;];%d;%99[^\n]", &new_book.accession_number, new_book.title, new_book.author, &new_book.total_pages, new_book.publisher);

    books[book_count++] = new_book;
    printf("Book inserted: %s by %s\n", new_book.title, new_book.author);
}

void delete_book(int accession_number, int client_sock)
{
    int found = 0;
    for (int i = 0; i < book_count; i++)
    {
        if (books[i].accession_number == accession_number)
        {
            found = 1;
            for (int j = i; j < book_count - 1; j++)
                books[j] = books[j + 1];

            book_count--;
            char message[] = "Book deleted successfully\n";
            send(client_sock, message, strlen(message), 0);
            return;
        }
    }

    if (!found)
    {
        char message[] = "Book not found\n";
        send(client_sock, message, strlen(message), 0);
    }
}

void display_books(int client_sock)
{
    char buffer[BUFFER_SIZE] = "";
    if (book_count == 0)
    {
        strcpy(buffer, "No books available\n");
    }
    else
    {
        for (int i = 0; i < book_count; i++)
        {
            char book_info[256];
            snprintf(book_info, sizeof(book_info), "Accession No: %d, Title: %s, Author: %s, Pages: %d, Publisher: %s\n",
                     books[i].accession_number, books[i].title, books[i].author, books[i].total_pages, books[i].publisher);
            strcat(buffer, book_info);
        }
    }
    send(client_sock, buffer, strlen(buffer), 0);
}

void search_books(char *query, int client_sock, int by_title)
{
    char buffer[BUFFER_SIZE] = "";
    int found = 0;

    for (int i = 0; i < book_count; i++)
    {
        if ((by_title && strcasecmp(books[i].title, query) == 0) ||
            (!by_title && strcasecmp(books[i].author, query) == 0))
        {
            found = 1;
            char book_info[256];
            snprintf(book_info, sizeof(book_info), "Accession No: %d, Title: %s, Author: %s, Pages: %d, Publisher: %s\n",
                     books[i].accession_number, books[i].title, books[i].author, books[i].total_pages, books[i].publisher);
            strcat(buffer, book_info);
        }
    }

    if (!found)
        strcpy(buffer, "No matching books found\n");

    send(client_sock, buffer, strlen(buffer), 0);
}

void handle_client(int client_sock)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            printf("Client disconnected\n");
            close(client_sock);
            return;
        }

        int option;
        sscanf(buffer, "%d", &option);

        if (option == 1)
        {
            insert_book(buffer + 2);
            send(client_sock, "Book inserted successfully\n", 27, 0);
        }
        else if (option == 2)
        {
            int acc_num;
            sscanf(buffer + 2, "%d", &acc_num);
            delete_book(acc_num, client_sock);
        }
        else if (option == 3)
        {
            display_books(client_sock);
        }
        else if (option == 4)
        {
            int search_by_title;
            char query[100];
            sscanf(buffer + 2, "%d;%99[^\n]", &search_by_title, query);
            search_books(query, client_sock, search_by_title);
        }
        else if (option == 5)
        {
            send(client_sock, "Goodbye!\n", 9, 0);
            break;
        }
    }

    close(client_sock);
}

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    listen(server_sock, 5);
    printf("Server started, waiting for connections...\n");

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)))
    {
        printf("Client connected\n");
        handle_client(client_sock);
    }

    close(server_sock);
    return 0;
}
