#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024
#define PORT 3388

void dispmenu()
{
    printf("Library Database\n");
    printf("1. Insert a Book\n");
    printf("2. Delete a Book\n");
    printf("3. Display all books\n");
    printf("4. Search based on author\n");
    printf("5. Exit\n");
    printf("Enter your choice\n");
}

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (retval < 0)
    {
        perror("failed to connect ");
        close(sockfd);
        exit(1);
    }
    printf("Connected to server\n");
    memset(buffer, 0, MAX_BUFFER);
    recv(sockfd, buffer, MAX_BUFFER, 0);
    if ((strcmp(buffer, "Server full")) == 0)
    {
        printf("Server Response: %s", buffer);
        close(sockfd);
        exit(1);
    }
    else
    {
        printf("Server Response: %s", buffer);
    }

    int choice;
    while (1)
    {
        dispmenu();
        scanf("%d", &choice);
        getchar();
        switch (choice)
        {
        case 1:
        {
            char title[10], author[10], pub[10];
            int acno, pages;
            printf("Enter title: ");
            fgets(title, 10, stdin);
            title[strcspn(title, "\n")] = '\0';
            printf("Enter author: ");
            fgets(author, 10, stdin);
            author[strcspn(author, "\n")] = '\0';
            printf("Enter acno: ");
            scanf("%d", &acno);
            printf("Enter pages: ");
            scanf("%d", &pages);
            getchar();
            printf("Enter pub: ");
            fgets(pub, 10, stdin);
            pub[strcspn(pub, "\n")] = '\0';

            snprintf(buffer, MAX_BUFFER, "1|%s|%s|%d|%d|%s", title, author, acno, pages, pub);
            send(sockfd, buffer, MAX_BUFFER, 0);
            memset(buffer, 0, MAX_BUFFER);
            recv(sockfd, buffer, MAX_BUFFER, 0);
            printf("Server Response: %s", buffer);
            break;
        }
        case 2:
        {
            int acno;
            printf("Enter acno: ");
            scanf("%d", &acno);
            snprintf(buffer, MAX_BUFFER, "2|%d", acno);
            send(sockfd, buffer, MAX_BUFFER, 0);
            memset(buffer, 0, MAX_BUFFER);
            recv(sockfd, buffer, MAX_BUFFER, 0);
            printf("Server Response: %s", buffer);
            break;
        }
        case 3:
        {
            strcpy(buffer, "3");
            send(sockfd, buffer, MAX_BUFFER, 0);
            memset(buffer, 0, MAX_BUFFER);
            recv(sockfd, buffer, MAX_BUFFER, 0);
            printf("Server Response: %s", buffer);
            break;
        }
        case 4:
        {
            char author[10];
            printf("Enter author name: ");
            fgets(buffer, 10, stdin);
            author[strcspn(author, "\n")] = '\0';
            snprintf(buffer, MAX_BUFFER, "4|%s", author);
            send(sockfd, buffer, MAX_BUFFER, 0);
            memset(buffer, 0, MAX_BUFFER);
            recv(sockfd, buffer, MAX_BUFFER, 0);
            printf("Server Response: %s", buffer);
            break;
        }
        case 5:
        {
            strcpy(buffer, "5");
            send(sockfd, buffer, MAX_BUFFER, 0);
            close(sockfd);
            printf("exiting..\n");
            exit(0);
        }
        default:
        {
            printf("Invalid choice\n");
        }
        }
    }
    close(sockfd);
    return 0;
}