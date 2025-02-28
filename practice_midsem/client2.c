#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3388
#define MAX_BUFFER 1024

void display_menu()
{
	printf("\n=====HOSPITAL DB=====\n");
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
	struct sockaddr_in serveraddr;
	char buffer[MAX_BUFFER];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("Socket Creation Failed!");
		exit(1);
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
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
		{ // insert
			char name[50], disease[50];
			int id, age;
			printf("Enter Patient ID: ");
			scanf("%d", &id);
			getchar();
			printf("Enter Patient Name: ");
			fgets(name, 50, stdin);
			name[strcspn(name, "\n")] = '\0';
			printf("Enter Patient Age: ");
			scanf("%d", &age);
			getchar();
			printf("Enter Patient Disease: ");
			fgets(disease, 50, stdin);
			disease[strcspn(disease, "\n")] = '\0';

			snprintf(buffer, MAX_BUFFER, "1|%d|%s|%d|%s", id, name, age, disease);
			send(sockfd, buffer, strlen(buffer), 0);
			memset(buffer, 0, MAX_BUFFER);
			recv(sockfd, buffer, MAX_BUFFER, 0);
			printf("Server Response: %s\n", buffer);
			break;
		}
		case 2:
		{ // display
			strcpy(buffer, "2");
			send(sockfd, buffer, strlen(buffer), 0);
			memset(buffer, 0, MAX_BUFFER);
			recv(sockfd, buffer, MAX_BUFFER, 0);
			printf("Server Response: %s\n", buffer);
			break;
		}
		case 3:
		{ // old+young
			strcpy(buffer, "3");
			send(sockfd, buffer, strlen(buffer), 0);
			memset(buffer, 0, MAX_BUFFER);
			recv(sockfd, buffer, MAX_BUFFER, 0);
			printf("Server Response: %s\n", buffer);
			break;
		}
		case 4:
		{ // most common disease
			strcpy(buffer, "4");
			send(sockfd, buffer, strlen(buffer), 0);
			memset(buffer, 0, MAX_BUFFER);
			recv(sockfd, buffer, MAX_BUFFER, 0);
			printf("Server Response: %s\n", buffer);
			break;
		}
		case 5:
		{ // modify
			char mod[50];
			int id, ch;
			printf("Enter Patient ID : ");
			scanf("%d", &id);

			printf("What do you want to change?\n1. Name\n2. Age\n3. Disease\n");
			scanf("%d", &ch);
			getchar();

			printf("Enter modification: ");
			fgets(mod, 50, stdin);
			mod[strcspn(mod, "\n")] = '\0';

			snprintf(buffer, MAX_BUFFER, "5|%d|%d|%s", id, ch, mod);
			send(sockfd, buffer, strlen(buffer), 0);
			memset(buffer, 0, MAX_BUFFER);
			recv(sockfd, buffer, MAX_BUFFER, 0);
			printf("Server Response: %s\n", buffer);
			break;
		}
		case 6:
		{ // exit
			strcpy(buffer, "5");
			send(sockfd, buffer, strlen(buffer), 0);
			close(sockfd);
			printf("Exiting...\n");
			exit(0);
		}
		default:
		{
			printf("Invalid choice, try again!\n");
		}
		}
	}
	close(sockfd);
	return 0;
}