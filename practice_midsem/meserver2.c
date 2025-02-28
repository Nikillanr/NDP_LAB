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

typedef struct
{
    int id;
    char name[10];
    int age;
    char dis[10];
} patient;

patient hosp[MAX_PATIENTS];

int pcount = 0;
int clicount = 0;

void insnew(int id, char *name, int age, char *dis, char *response)
{
    printf("inserting..\n");
    for (int i = 0; i < pcount; i++)
    {
        if (hosp[i].id == id)
        {
            strcpy(response, "Patient already exists");
            return;
        }
    }
    if (pcount >= MAX_PATIENTS)
    {
        strcpy(response, "MAX no of patients reached");
        return;
    }
    hosp[pcount].id = id;
    strcpy(hosp[pcount].name, name);
    hosp[pcount].age = age;
    strcpy(hosp[pcount].dis, dis);
    pcount++;
    snprintf(response, MAX_BUFFER, "patient %s has been inserted", name);
}

void disp(char *response)
{
    strcpy(response, "Patient Records\n");
    for (int i = 0; i < pcount; i++)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "ID: %d,NAME: %s, AGE: %d, DISEASE: %s\n", hosp[i].id, hosp[i].name, hosp[i].age, hosp[i].dis);
        strcat(response, buffer);
    }
}

void findold(char *response)
{
    patient old, young;
    old.age = 0;
    young.age = 100;
    strcpy(response, "Oldest Patient\n");
    for (int i = 0; i < pcount; i++)
    {
        if (old.age < hosp[i].age)
        {
            old = hosp[i];
        }
        if (young.age > hosp[i].age)
        {
            young = hosp[i];
        }
    }
    char buffer1[MAX_BUFFER];
    snprintf(buffer1, MAX_BUFFER, "Oldest patient id %d age %d, Youngest patient id %d age %d\n", old.id, old.age, young.id, young.age);
    strcat(response, buffer1);
}

void mostcom(char *response)
{
    int mostcom = 0;
    char mcd[10];
    strcpy(response, "Most common disease is: \n");
    for (int i = 0; i < pcount; i++)
    {
        int count = 0;
        for (int j = 1; j < pcount; j++)
        {
            if (hosp[i].dis == hosp[j].dis)
            {
                count++;
            }
        }
        if (count > mostcom)
        {
            mostcom = count;
            strcpy(mcd, hosp[i].dis);
        }
    }
    char buffer[MAX_BUFFER];
    snprintf(buffer, MAX_BUFFER, "Most common disease %s no of times %d\n", mcd, mostcom);
    strcat(response, buffer);
}

void modrec(int id, char *name, int age, char *dis, char *response)
{
    for (int i = 0; i < pcount; i++)
    {
        if (hosp[i].id == id)
        {
            strcpy(hosp[i].name, name);
            hosp[i].age = age;
            strcpy(hosp[i].dis, dis);
        }
    }
    strcpy(response, "Modified data");
}

void handlecli(int client_sock)
{
    char buffer[MAX_BUFFER];
    while (1)
    {
        int recedbytes;
        memset(buffer, 0, MAX_BUFFER);
        char response[MAX_BUFFER];
        recedbytes = recv(client_sock, buffer, MAX_BUFFER, 0);
        if (recedbytes < 0)
        {
            break;
        }
        int choice = atoi(strtok(buffer, "|"));
        memset(response, 0, MAX_BUFFER);
        switch (choice)
        {
        case 1:
        {
            int id = atoi(strtok(NULL, "|"));
            char *name = strtok(NULL, "|");
            int age = atoi(strtok(NULL, "|"));
            char *dis = strtok(NULL, "|");
            insnew(id, name, age, dis, response);
            break;
        }
        case 2:
        {
            disp(response);
            break;
        }
        case 3:
        {
            findold(response);
            break;
        }
        case 4:
        {
            mostcom(response);
            break;
        }
        case 5:
        {
            int id = atoi(strtok(NULL, "|"));
            char *name = strtok(NULL, "|");
            int age = atoi(strtok(NULL, "|"));
            char *dis = strtok(NULL, "|");
            modrec(id, name, age, dis, response);
            break;
        }
        case 6:
        {
            strcpy(response, "Exiting");
            send(client_sock, response, strlen(response), 0);
            close(client_sock);
            exit(0);
        }

        default:
        {
            strcpy(response, "Invalid");
            break;
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
    struct sockaddr_in server_addr, client_addr;
    actuallen = sizeof(client_addr);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    retval = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (retval < 0)
    {
        perror("binding failed");
        close(sockfd);
        exit(1);
    }
    retval = listen(sockfd, 10);
    if (retval < 0)
    {
        perror("listen failed");
        close(sockfd);
        exit(1);
    }

    printf("Server listening of port %d\n", PORT);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &actuallen);
        if (newsockfd < 0)
        {
            perror("Accept Failed!");
            continue;
        }

        printf("Client connected!\n");
        pid_t pid;
        pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
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
        }
    }
    close(sockfd);
    return 0;
}