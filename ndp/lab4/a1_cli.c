#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

void communicate(int sock)
{
    char buffer[BUFFER_SIZE];
    int choice;

    while (1)
    {
        printf("\n1. Insert Book\n2. Delete Book\n3. Display Books\n4. Search Book\n5. Exit\nEnter choice: ");
        scanf("%d", &choice);
        getchar();

        memset(buffer, 0, BUFFER_SIZE);
        if (choice == 1)
        {
            printf("Enter book details (AccNo;Title;Author;Pages;Publisher): ");
            fgets(buffer + 2, BUFFER_SIZE - 2, stdin);
            buffer[0] = '1';
        }
        else if (choice == 2)
        {
            printf("Enter Accession Number to delete: ");
            scanf("%s", buffer + 2);
            buffer[0] = '2';
        }
        else if (choice == 3)
        {
            strcpy(buffer, "3");
        }
        else if (choice == 4)
        {
            printf("Search by (1) Title or (0) Author? ");
            scanf("%s", buffer + 2);
            getchar();
            printf("Enter title/author: ");
            fgets(buffer + 4, BUFFER_SIZE - 4, stdin);
            buffer[0] = '4';
        }
        else if (choice == 5)
        {
            strcpy(buffer, "5");
            send(sock, buffer, strlen(buffer), 0);
            break;
        }

        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, BUFFER_SIZE, 0);
        printf("%s\n", buffer);
    }
}

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {.sin_family = AF_INET, .sin_port = htons(PORT)};

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    communicate(sock);
    close(sock);
    return 0;
}
