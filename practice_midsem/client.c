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
	printf("\n========LIBRARY DATABASE=========\n");
	printf("1. Insert a new book\n");
	printf("2. Delete a book\n");
	printf("3. Display Books\n");
	printf("4. Search for a book\n");
	printf("5. Exit\n");
	printf("Enter your choice...\n");
}

int main()
{
	int sockfd, retval;
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
	memset(buffer, 0, MAX_BUFFER);
	recv(sockfd, buffer, MAX_BUFFER, 0);
	if (strncmp(buffer, "Server is full", 14) == 0)
	{
		printf("Server Response: %s\n", buffer);
		close(sockfd);
		exit(1);
	}
	else
	{
		printf("Server Response: %s\n", buffer);
	}

	int choice;
	while (1)
	{
		display_menu();
		scanf("%d", &choice);
		getchar(); // consume a newline

		switch (choice)
		{
		case 1:
		{ // insert
			char title[50], author[50], publisher[50];
			int accession_number, pages;
			printf("Enter title: ");
			fgets(title, 50, stdin);
			title[strcspn(title, "\n")] = '\0';
			printf("Enter author: ");
			fgets(author, 50, stdin);
			author[strcspn(author, "\n")] = '\0';
			printf("Enter accession number: ");
			scanf("%d", &accession_number);
			printf("Enter total pages: ");
			scanf("%d", &pages);
			getchar();
			printf("Enter publisher: ");
			fgets(publisher, 50, stdin);
			publisher[strcspn(publisher, "\n")] = '\0';

			snprintf(buffer, MAX_BUFFER, "1|%s|%s|%d|%d|%s", title, author, accession_number, pages, publisher);
			send(sockfd, buffer, strlen(buffer), 0);
			memset(buffer, 0, MAX_BUFFER);
			recv(sockfd, buffer, MAX_BUFFER, 0);
			printf("Server Response: %s\n", buffer);
			break;
		}
		case 2:
		{ // delete
			int acc;
			printf("Enter accession number of book you want to delete: ");
			scanf("%d", &acc);
			snprintf(buffer, MAX_BUFFER, "2|%d", acc);
			send(sockfd, buffer, strlen(buffer), 0);
			memset(buffer, 0, MAX_BUFFER);
			recv(sockfd, buffer, MAX_BUFFER, 0);
			printf("Server Response: %s\n", buffer);
			break;
		}
		case 3:
		{ // display
			strcpy(buffer, "3");
			send(sockfd, buffer, strlen(buffer), 0);
			memset(buffer, 0, MAX_BUFFER);
			recv(sockfd, buffer, MAX_BUFFER, 0);
			printf("Server Response: %s\n", buffer);
			break;
		}
		case 4:
		{ // search
			char author[50];
			printf("Enter author name to search: ");
			scanf("%s", author);
			snprintf(buffer, MAX_BUFFER, "4|%s", author);
			send(sockfd, buffer, strlen(buffer), 0);
			memset(buffer, 0, MAX_BUFFER);
			recv(sockfd, buffer, MAX_BUFFER, 0);
			printf("Server Response: %s\n", buffer);
			break;
		}
		case 5:
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
