#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER 1024
#define PORT 3388
#define MAX_PATIENTS 50

typedef struct
{
	int id;
	char name[50];
	int age;
	char disease[50];
} patients;

patients patient[MAX_PATIENTS];
int pCount = 0;

void insert_patient(int id, char *name, int age, char *disease, char *response)
{
	if (pCount >= MAX_PATIENTS)
	{
		strcpy(response, "ERROR! Database full cannot insert details");
		return;
	}

	for (int i = 0; i < pCount; i++)
	{
		if (patient[i].id == id)
		{
			strcpy(response, "ID already exists!");
			return;
		}
	}
	patient[pCount].id = id;
	strcpy(patient[pCount].name, name);
	patient[pCount].age = age;
	strcpy(patient[pCount].disease, disease);
	pCount++;
	snprintf(response, MAX_BUFFER, "Patient %d details entered successfully!", id);
}

void display_patients(char *response)
{
	strcpy(response, "Patient List:\n");
	for (int i = 0; i < pCount; i++)
	{
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "\nID: %d, Name: %s, Age: %d, Disease: %s", patient[i].id, patient[i].name, patient[i].age, patient[i].disease);
		strcat(response, buffer);
	}
}

void handle_client(int newsockfd)
{
	char buffer[MAX_BUFFER];

	while (1)
	{
		int recedbytes;
		memset(buffer, 0, MAX_BUFFER);
		recedbytes = recv(newsockfd, buffer, MAX_BUFFER, 0);

		if (recedbytes < 0)
		{
			break;
		}
		int choice = atoi(strtok(buffer, "|"));
		char response[MAX_BUFFER];
		memset(response, 0, MAX_BUFFER);

		switch (choice)
		{
		case 1:
		{ // insert
			char *cid = strtok(NULL, "|");
			char *name = strtok(NULL, "|");
			char *p_age = strtok(NULL, "|");
			char *disease = strtok(NULL, "|");

			if (!cid || !name || !p_age || !disease)
			{
				strcpy(response, "ERROR! Missing Input Fields!");
			}
			else
			{
				int id = atoi(cid);
				int age = atoi(p_age);
				insert_patient(id, name, age, disease, response);
			}
			break;
		}
		case 2:
		{ // display
			display_patients(response);
			break;
		}
		case 3:
		{ // oldest & youngest

			if (pCount == 0)
			{
				strcpy(response, "No patient records found.");
			}
			else
			{
				int oldest = patient[0].age;
				int youngest = patient[0].age;

				for (int i = 1; i < pCount; i++)
				{
					if (patient[i].age > oldest)
					{
						oldest = patient[i].age;
					}
					if (patient[i].age < youngest)
					{
						youngest = patient[i].age;
					}
				}

				snprintf(response, MAX_BUFFER, "\nOldest Patient Age: %d\nYoungest Patient Age: %d", oldest, youngest);
			}

			break;
		}
		case 4:
		{ // most common disease
			if (pCount == 0)
			{
				strcpy(response, "No patient records found.");
			}
			else
			{
				int most_common = 0;
				char mcd[50] = "";

				for (int i = 0; i < pCount; i++)
				{
					int count = 0;
					for (int j = 0; j < pCount; j++)
					{
						if (strcmp(patient[i].disease, patient[j].disease) == 0)
						{
							count++;
						}
					}
					if (count > most_common)
					{
						most_common = count;
						strcpy(mcd, patient[i].disease);
					}
				}

				snprintf(response, MAX_BUFFER, "Most Common Disease: %s (Occurrences: %d)", mcd, most_common);
			}

			break;
		}
		case 5:
		{ // modify record
			char *cid = strtok(NULL, "|");
			char *ch = strtok(NULL, "|");
			char *mod = strtok(NULL, "|");
			if (!ch || !cid || !mod)
			{
				strcpy(response, "Missing Input Fields!");
			}
			else
			{
				int c = atoi(ch);
				int id = atoi(cid);
				for (int i = 0; i < pCount; i++)
				{
					if (patient[i].id == id)
					{
						if (c == 1)
						{
							strcpy(patient[i].name, mod);
						}
						if (c == 2)
						{
							int age = atoi(mod);
							patient[i].age = age;
						}
						if (c == 3)
						{
							strcpy(patient[i].disease, mod);
						}
					}
				}
			}
			strcpy(response, "Data Modified!");
			break;
		}
		case 6:
		{ // exit
			strcpy(response, "EXIT");
			send(newsockfd, response, strlen(response), 0);

			close(newsockfd);
			exit(0);
		}
		default:
		{
			strcpy(response, "Invalid Command");
			break;
		}
		}
		send(newsockfd, response, strlen(response), 0);
	}
	close(newsockfd);
}

int main()
{
	int sockfd, newsockfd;
	socklen_t actuallen;
	struct sockaddr_in serveraddr, clientaddr;
	int retval;
	actuallen = sizeof(clientaddr);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("Socket Creation Failed!");
		exit(1);
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval < 0)
	{
		perror("Binding Failed!");
		close(sockfd);
		exit(1);
	}

	retval = listen(sockfd, 10);
	if (retval < 0)
	{
		perror("Cannot Listen! Error");
		close(sockfd);
		exit(1);
	}

	printf("Server Listening On PORT %d...\n", PORT);

	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

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
		}
	}
	close(sockfd);
	return 0;
}