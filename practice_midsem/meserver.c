#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_BUFFER 1024
#define PORT 3388
#define MAX_BOOKS 100
#define MAX_CLIENTS 2

typedef struct
{
    char title[10];
    char author[10];
    int acno;
    int pages;
    char pub[10];

} book;

book lib[MAX_BOOKS];

int bcount = 0;
int clicount = 0;

void insbook(char *title, char *author, int acno, int pages, char *pub, char *response)
{
    if (bcount >= MAX_BOOKS)
    {
        strcpy(response, "max no of books reached");
        return;
    }
    for (int i = 0; i < bcount; i++)
    {
        if (acno == lib[i].acno)
        {
            strcpy(response, "acno already exists");
            return;
        }
    }
    strcpy(lib[bcount].title, title);
    strcpy(lib[bcount].author, author);
    lib[bcount].acno = acno;
    lib[bcount].pages = pages;
    strcpy(lib[bcount].pub, pub);
    bcount++;
    snprintf(response, MAX_BUFFER, "Book '%s' inserted successfully.", title);
}

void delbook(int acno, char *response)
{

    for (int i = 0; i < bcount; i++)
    {
        if (lib[i].acno == acno)
        {
            for (int j = i; j < bcount - 1; j++)
            {
                lib[j] = lib[j + 1];
            }
            bcount--;
            strcpy(response, "deleted successfully");
            return;
        }
    }
    strcpy(response, "book not found");
}

void disbook(char *response)
{
    strcpy(response, "Book List:\n");
    for (int i = 0; i < bcount; i++)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "\nTitle: %s, Author: %s, AccessionNo: %d, Total Pages: %d, Publisher: %s", lib[i].title, lib[i].author, lib[i].acno, lib[i].pages, lib[i].pub);
        strcat(response, buffer);
    }
}

void searchbook(char *author, char *response)
{
    int found = 0;
    strcpy(response, "Search Results:\n");
    for (int i = 0; i < bcount; i++)
    {
        if (strcmp(lib[i].author, author) == 0)
        {
            found = 1;
            char buffer[MAX_BUFFER];
            snprintf(buffer, MAX_BUFFER, "\nTitle: %s, Author: %s, Acno: %d, Pages: %d, Publisher: %s", lib[i].title, lib[i].author, lib[i].acno, lib[i].pages, lib[i].pub);
            strcat(response, buffer);
        }
    }
    if (!found)
    {
        strcpy(response, "book not found");
    }
}

void handlecli(int client_sock)
{
    char buffer[MAX_BUFFER];
    while (1)
    {
        memset(buffer, 0, MAX_BUFFER);
        int recedbytes = recv(client_sock, buffer, MAX_BUFFER, 0);
        if (recedbytes <= 0)
            break;
        int choice = atoi(strtok(buffer, "|"));
        char response[MAX_BUFFER] = {0};
        switch (choice)
        {
        case 1:
        {
            char *title = strtok(NULL, "|");
            char *author = strtok(NULL, "|");
            char *acstr = strtok(NULL, "|");
            char *pagestr = strtok(NULL, "|");
            char *pub = strtok(NULL, "|");
            if (!title || !author || !acstr || !pagestr || !pub)
            {
                strcpy(response, "Missing data!");
            }
            else
            {
                int acno = atoi(acstr);
                int pages = atoi(pagestr);
                insbook(title, author, acno, pages, pub, response);
            }
            break;
        }
        case 2:
        {
            char *acstr = strtok(NULL, "|");
            if (!acstr)
            {
                strcpy(response, "Missing data!!");
            }
            else
            {
                int acno = atoi(acstr);
                delbook(acno, response);
            }
            break;
        }
        case 3:
        {
            disbook(response);
            break;
        }
        case 4:
        {
            char *author = strtok(NULL, "|");
            if (!author)
            {
                strcpy(response, "missing data!!");
            }
            else
            {
                searchbook(author, response);
            }
            break;
        }
        case 5:
        {
            strcpy(response, "EXIT");
            send(client_sock, response, strlen(response), 0);
            close(client_sock);
            exit(0);
        }
        default:
        {
            strcpy(response, "Invalid option");
        }
        }
        send(client_sock, response, strlen(response), 0);
    }
    close(client_sock);
}

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in server_addr, client_addr;
    actuallen = sizeof(client_addr);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    retval = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (retval < 0)
    {
        perror("Binding failed");
        close(sockfd);
        exit(1);
    }

    retval = listen(sockfd, 3);
    if (retval < 0)
    {
        perror("Lsiten failed");
        close(sockfd);
        exit(1);
    }
    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &actuallen);
        clicount++;
        if (clicount > 2)
        {
            char *errormsg = "Server full";
            send(newsockfd, errormsg, strlen(errormsg), 0);
            printf("New connection terminated");
            close(newsockfd);
            continue;
        }
        char *welmsg = "Welcomoe to library pls select an option\n";
        send(newsockfd, welmsg, strlen(welmsg), 0);
        if (newsockfd < 0)
        {
            perror("Accept failed");
            continue;
        }
        printf("Client Connected\n");
        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            perror("Fork failed");
            close(newsockfd);
        }
        else if (pid == 0)
        {
            close(sockfd);
            handlecli(newsockfd);
            exit(0);
        }
        else
        {
            close(newsockfd);
            clicount--;
        }
    }
    close(sockfd);
    return 0;
}