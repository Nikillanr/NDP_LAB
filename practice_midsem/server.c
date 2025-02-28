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
#define MAX_BOOKS 100
#define MAX_CLIENTS 2

typedef struct
{
	char title[50];
	char author[50];
	int accession_number;
	int pages;
	char publisher[50];
} book;

book library[MAX_BOOKS];

int bookCount = 0;

int client_count = 0;

void insert_book(char *title, char *author, int accession_number, int pages, char *publisher, char *response)
{
	if (bookCount >= MAX_BOOKS)
	{
		strcpy(response, "Error: Database full. Cannot insert book.");
		return;
	}
	// Check for duplicate accession number
	for (int i = 0; i < bookCount; i++)
	{
		if (library[i].accession_number == accession_number)
		{
			strcpy(response, "Error: Accession number already exists.");
			return;
		}
	}
	// Insert the book
	strcpy(library[bookCount].title, title);
	strcpy(library[bookCount].author, author);
	library[bookCount].accession_number = accession_number;
	library[bookCount].pages = pages;
	strcpy(library[bookCount].publisher, publisher);
	bookCount++;
	snprintf(response, MAX_BUFFER, "Book '%s' inserted successfully.", title);
}

void delete_book(int accession_number, char *response)
{
	int found = 0;
	for (int i = 0; i < bookCount; i++)
	{
		if (library[i].accession_number == accession_number)
		{
			found = 1;
			for (int j = i; j < bookCount - 1; j++)
			{
				library[j] = library[j + 1];
			}
			bookCount--;
			strcpy(response, "Book deleted successfull!");
			return;
		}
	}
	strcpy(response, "Book not found!");
}

void display_books(char *response)
{
	strcpy(response, "Book List:\n");
	for (int i = 0; i < bookCount; i++)
	{
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "\nTitle: %s, Author: %s, AccessionNo: %d, Total Pages: %d, Publisher: %s", library[i].title, library[i].author, library[i].accession_number, library[i].pages, library[i].publisher);
		strcat(response, buffer);
	}
}

void search_books(char *author, char *response)
{
	strcpy(response, "Search Results:\n");
	int found = 0;
	for (int i = 0; i < bookCount; i++)
	{
		if (strcmp(author, library[i].author) == 0)
		{
			found = 1;
			char buffer[256];
			snprintf(buffer, sizeof(buffer), "\nTitle: %s, Author: %s, AccessionNo: %d, Total Pages: %d, Publisher: %s", library[i].title, library[i].author, library[i].accession_number, library[i].pages, library[i].publisher);
			strcat(response, buffer);
		}
	}
	if (!found)
	{
		strcpy(response, "No books found for this author!");
	}
}

void handle_client(int client_sock)
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
		{ // insert
			char *title = strtok(NULL, "|");
			char *author = strtok(NULL, "|");
			char *acc_str = strtok(NULL, "|");
			char *pages_str = strtok(NULL, "|");
			char *publisher = strtok(NULL, "|");
			if (!title || !author || !acc_str || !pages_str || !publisher)
			{
				strcpy(response, "ERROR! Missing input fields");
			}
			else
			{
				int accession_number = atoi(acc_str);
				int total_pages = atoi(pages_str);
				insert_book(title, author, accession_number, total_pages, publisher, response);
			}
			break;
		}
		case 2:
		{ // delete
			char *acc_str = strtok(NULL, "|");
			if (!acc_str)
			{
				strcpy(response, "ERROR! Missing accession number");
			}
			else
			{
				int acc = atoi(acc_str);
				delete_book(acc, response);
			}
			break;
		}
		case 3:
		{ // display
			display_books(response);
			break;
		}

		case 4:
		{ // search books
			char *author_name = strtok(NULL, "|");
			if (!author_name)
			{
				strcpy(response, "ERROR! Missing Author Name!");
			}
			else
			{
				search_books(author_name, response);
			}
			break;
		}

		case 5:
		{ // exit
			strcpy(response, "EXIT");
			send(client_sock, response, strlen(response), 0);

			close(client_sock);
			exit(0);
		}
		default:
		{
			strcpy(response, "Invalid Command");
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
	int recedbytes, sentbytes;
	struct sockaddr_in server_addr, client_addr;
	actuallen = sizeof(client_addr);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		perror("Socket Creation Failed!");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	retval = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (retval < 0)
	{
		perror("Binding Failed!");
		close(sockfd);
		exit(1);
	}

	retval = listen(sockfd, 2);
	if (retval < 0)
	{
		perror("Listen Failed!");
		close(sockfd);
		exit(1);
	}

	printf("Sever listening on port %d...\n", PORT);

	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &actuallen);

		client_count++;
		if (client_count > 2)
		{
			char *error_msg = "Server is full! Try again later...";
			send(newsockfd, error_msg, strlen(error_msg), 0);
			printf("New Client Connection Refused and Terminated!\n");
			close(newsockfd); // Ensure client is closed immediately
			continue;
		}

		char *welcome_msg = "Welcome to the library! Please select an option...";
		send(newsockfd, welcome_msg, strlen(welcome_msg), 0);

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
			perror("Fork Failed!");
			close(newsockfd);
		}
		else if (pid == 0)
		{
			close(sockfd);
			handle_client(newsockfd);

			exit(0);
		}
		else
		{
			close(newsockfd);

			client_count--;
		}
	}
	close(sockfd);
	return 0;
}
