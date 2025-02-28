#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024
#define MAX_CLIENTS 2
#define MAX_PATIENTS 5
#define PORT 3388

void display_menu()
{
    printf("\nHOSPITAL DB\n");
    printf("1. Insert Patient Details\n");
    printf("2. Display Patient Details\n");
    printf("3. Age of oldest and youngest\n");
    printf("4. Most Common Disease\n");
    printf("5. Modify records\n");
    printf("6. Exit\n");
    printf("Enter your choice...\n");
}

int main()
{
    int sockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket Creation Failed!");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (retval < 0)
    {
        perror("Failed to connect to server!");
        close(sockfd);
        exit(1);
    }

    printf("Connected to server!\n");

    int choice;
    while (1)
    {
        display_menu();
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
        {
            char name[10], dis[10];
            int id, age;
            printf("Enter id: \n");
            scanf("%d", &id);
            getchar();
            printf("Enter name\n");
            fgets(name, 10, stdin);
            name[strcspn(name, "\n")] = '\0';
            printf("Enter age: \n");
            scanf("%d", &age);
            getchar();
            printf("Enter dis\n");
            fgets(dis, 10, stdin);
            dis[strcspn(dis, "\n")] = '\0';

            snprintf(buffer, MAX_BUFFER, "1|%d|%s|%d|%s", id, name, age, dis);
            send(sockfd, buffer, MAX_BUFFER, 0);
            memset(buffer, 0, MAX_BUFFER);
            recv(sockfd, buffer, MAX_BUFFER, 0);
            printf("Server Response: %s\n", buffer);
            break;
        }
        case 2:
        {
            strcpy(buffer, "2");
            send(sockfd, buffer, MAX_BUFFER, 0);
            memset(buffer, 0, MAX_BUFFER);
            recv(sockfd, buffer, MAX_BUFFER, 0);
            printf("Server Response: %s\n", buffer);
            break;
        }
        case 3:
        {
            strcpy(buffer, "3");
            send(sockfd, buffer, MAX_BUFFER, 0);
            memset(buffer, 0, MAX_BUFFER);
            recv(sockfd, buffer, MAX_BUFFER, 0);
            printf("Server Response: %s\n", buffer);
            break;
        }
        case 4:
        {
            strcpy(buffer, "4");
            send(sockfd, buffer, MAX_BUFFER, 0);
            memset(buffer, 0, MAX_BUFFER);
            recv(sockfd, buffer, MAX_BUFFER, 0);
            printf("Server Response: %s\n", buffer);
            break;
        }
        case 5:
        {
            char name[10], dis[10];
            int id, age;
            printf("Enter id to modify: \n");
            scanf("%d", &id);
            getchar();
            printf("Enter new name\n");
            fgets(name, 10, stdin);
            name[strcspn(name, "\n")] = '\0';
            printf("Enter new age: \n");
            scanf("%d", &age);
            getchar();
            printf("Enter new dis\n");
            fgets(dis, 10, stdin);
            dis[strcspn(dis, "\n")] = '\0';

            snprintf(buffer, MAX_BUFFER, "5|%d|%s|%d|%s", id, name, age, dis);
            send(sockfd, buffer, MAX_BUFFER, 0);
            memset(buffer, 0, MAX_BUFFER);
            recv(sockfd, buffer, MAX_BUFFER, 0);
            printf("Server Response: %s\n", buffer);
            break;
        }
        case 6:
        {
            strcpy(buffer, "6");
            send(sockfd, buffer, strlen(buffer), 0);
            close(sockfd);
            printf("Exiting...\n");
            exit(0);
        }
        default:
        {
            printf("Invalid\n");
        }
        }
    }
    close(sockfd);
    return 0;
}