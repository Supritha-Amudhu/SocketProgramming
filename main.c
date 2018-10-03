#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "DieWithError.c"

#define CONNECT_SERVER 0 
#define USER_LIST 1
#define SEND_MESSAGE 2
#define GET_MESSAGES 3
#define INITIATE_CHAT 4
#define CHAT_WITH_FRIEND 5
#define REGISTER_AS_USER 6
#define EXIT 7
#define STARTING_VALUE 10
#define FILE_PATH "/Users/nishsiva/Desktop/Supritha/Networks/Networks Project 1/registered_user_list.txt"



char global_username[10];

int main() {
	int option = STARTING_VALUE;
	int sock = 0;
	while(option != EXIT){
		option = view_available_commands();
		sock = execute_commands(option, sock);
	}
	return 0;
}

int execute_commands(int option, int sock) {
	// printf("The option you selected is: %d\n\n", option);
	
	switch(option) {
		case CONNECT_SERVER:
			if (sock)
				{
					close(sock);
					printf("Disconnecting from Server!\n");
				}
			sock = connect_to_server();
		break;
		case USER_LIST:
			if (sock)
				{
					get_list_of_users(sock);
				}
			else
				{
					printf("Please connect to the server and try again.\n");
				}
		break;
		case SEND_MESSAGE:
			if (sock)
				{
					send_message(sock);
				}
			else
				{
					printf("Please connect to the server and try again.\n");
				}
		break;
		case GET_MESSAGES:
			if (sock)
				{
					get_messages(sock);
				}
			else
				{
					printf("Please connect to the server and try again.\n");
				}
		break;
		case INITIATE_CHAT:
			if (sock)
				{
					close(sock);
					printf("Disconnecting from Server!\n");
					initiate_chat();
				}
			else
				{
					printf("Please connect to the server and try again.\n");
				}
		break;
		case CHAT_WITH_FRIEND:
			if (sock)
				{
					close(sock);
					printf("Disconnecting from Server!\n");
					chat_with_friend();
				}
			else
				{
					printf("Please connect to the server and try again.\n");
				}
		break;
		case REGISTER_AS_USER:
			if (sock)
				{
					close(sock);
					printf("Disconnecting from Server!\n");
				}
			register_user_on_server();
		break;
		case EXIT:
			printf("Exiting application\n");
		break;
		default:
			printf("Invalid option. Please choose another option from the list: \n");
		break;
	}
	return sock;
}	

int connect_to_server() {
	printf("Please enter the IP address:\n");
	char* ip_address;
	scanf("%s", &ip_address);
	printf("Please enter the port number:\n");
	char* port_number;
	scanf("%s", &port_number);

	int sock; /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort; /* Echo server port */

    char *echoString; /* String to send to echo server */
    char echoBuffer[100]; /* Buffer for echo string */
    unsigned int echoStringLen; /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd; /* Bytes read in single recv() and total bytes read */

	/* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError(" socket () failed") ;


	/* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(&ip_address); /* Server IP address */
    echoServAddr.sin_port = htons(atoi(&port_number)); /* Server port */


	/* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError(" connect () failed");
    else
    	printf("\nConnected to server!\n");
	
	if(validate_user(sock)) {
		printf("User Authenticated!\n");
		return sock;
	}
	else
	{
		printf("Auth Failed! Disconnecting from server.");
		close(sock);
		return 0;
	}
}

int initiate_chat()
{
	int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */

    
	printf("Please enter the port number:\n");
	scanf("%d", &echoServPort);
    

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError( "socket () failed") ;

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort); /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError ( "bind () failed");
    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, 5) < 0)
        DieWithError("listen() failed") ;
    printf("Waiting for Client..\n");

    clntLen = sizeof(echoClntAddr);
    /* Wait for a client to connect */
    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
        DieWithError("accept() failed");
    else
    {
    	/* clntSock is connected to a client! */
        printf("Connected to client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        printf("<Type 'bye' to stop the conversation>\n");
        // printf("%s %d", g_username, strlen(g_username));
        char friend_name[10];
        memset(friend_name, 0, sizeof friend_name);

        // printf("Waiting to receive!\n");
        if (recv(clntSock, friend_name, 10, 0) < 0)
			DieWithError("Server: Unable to recv friend name");
		
		char name[20] = "You\0";
		if (send(clntSock, name, strlen(name), 0) < strlen(name))
			DieWithError("Server: Unable to send user name");

		char message[200];
		memset(message, 0, sizeof message);
		strcpy(message, friend_name);

		while(strcmp(message, "bye") != 0)
		{
			// printf("Inside while in Server!\n");
			char temp;
			memset(message, 0, sizeof message);
			if (recv(clntSock, message, 200, 0) <= 0)
				DieWithError("Unable to recv message");
			
			strcat(message, "\0");
			if (strcmp(message, "bye") == 0)
			{
				break;
			}
			printf("%s: %s\n", "Your Friend", message);
			printf("You: ");
			scanf("%c", &temp);
			memset(message, 0, sizeof message);
			scanf("%[^\n]", message);

			if (send(clntSock, message, strlen(message), 0) != strlen(message))
				DieWithError("Unable to send message.");
		}
		close(clntSock);
    }
}

int chat_with_friend()
{
	printf("Please enter the IP address:\n");
	char* ip_address;
	scanf("%s", &ip_address);
	printf("Please enter the port number:\n");
	char* port_number;
	scanf("%s", &port_number);

	int sock; /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort; /* Echo server port */

    char *echoString; /* String to send to echo server */
    char echoBuffer[100]; /* Buffer for echo string */
    unsigned int echoStringLen; /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd; /* Bytes read in single recv() and total bytes read */

	/* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    	DieWithError(" socket () failed");


	/* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(&ip_address); /* Server IP address */
    echoServAddr.sin_port = htons(atoi(&port_number)); /* Server port */

	// printf("%s %d", g_username, strlen(g_username));

	/* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError(" connect () failed");
    else
    	printf("\nConnected to server!\n");
    	printf("<Type 'bye' to stop the conversation>\n");
    char name[20] = "You\0";
    if (send(sock, name, strlen(name), 0) < strlen(name))
		DieWithError("Client: Unable to send user name");


	char friend_name[10];
	memset(friend_name, 0, sizeof friend_name);
    if (recv(sock, friend_name, 10, 0) < 0)
    	DieWithError("Client: Unable to recv friend name");

	char message[200];
	memset(message, 0, sizeof message);
	strcpy(message, friend_name);

	while(strcmp(message, "bye") != 0)
	{
		// printf("Inside while in Client!\n");
		char temp;
		printf("You: ");
		scanf("%c", &temp);
		memset(message, 0, sizeof message);
		scanf("%[^\n]", message);
		// printf("Message read: %s", message);

		if (send(sock, message, strlen(message), 0) != strlen(message))
			DieWithError("Unable to send message.");
		if (strcmp(message, "bye") == 0)
		{
			break;
		}

		memset(message, 0, sizeof message);
		
		if (recv(sock, message, 200, 0) <= 0)
			DieWithError("Unable to recv message");
		strcat(message, "\0");
		printf("%s: %s\n", "Your Friend", message);
	}

}

int send_message(int sock)
{
	char option[1];
	option[0] = '2';
	
	if (send(sock, option, strlen(option), 0) != strlen(option))
		DieWithError("Unable to send get_message option");

	char* isValid;
	char temp;
	isValid = malloc(2);

	printf("Please enter the recipient name: ");
	char recipient[100];
	scanf("%s", recipient);
	printf("Please enter the message: ");
	scanf("%c", &temp);
	char message[200];
	scanf("%[^\n]", message);
	
	
	char payload[200];
	payload[0] = "\0";
	memset(payload, 0, sizeof payload);
	strcat(payload, recipient);
	strcat(payload, "$$$");
	if (strlen(message) < 1)
	{
		printf("Enter a valid message.");
		return 0;
	}
	strcat(payload, message);
	strcat(payload, "\0");

	if (send(sock, payload, strlen(payload), 0) != strlen(payload))
		DieWithError("Unable to send recipient name");
	
	// sleep(1);
	// if (send(sock, &message, strlen(&message), 0) != strlen(&message))
	// 	DieWithError("Unable to send message");
	
	if (recv(sock, &isValid, 1, 0) <= 0)
		DieWithError("Unable to complete send message");

	if (atoi(isValid) < 0)
	{
		printf("Message Sending Failed\n");
	}
}

int get_messages(int sock) {

	char option[1];
	option[0] = '3';
	char *messages;
	messages = malloc(2000);
	
	if (send(sock, option, strlen(option), 0) != strlen(option))
		DieWithError("Unable to send get_message option");
	// else
	// 	// printf("waiting for messages..\n");

	if (recv(sock, messages, 2000, 0) <= 0)
		DieWithError("Unable to get user messages");

	printf("\nMessages:\n%s\n", messages);
}

int get_list_of_users(int sock) {

	char option[1];
	option[0] = '1';
	char *user_list;
	user_list = malloc(100);
	
	if (send(sock, option, strlen(option), 0) != strlen(option))
		DieWithError("Unable to send get_list_of_users option");
	if (recv(sock, user_list, 50, 0) <= 0)
		DieWithError("Unable to get user list");

	printf("\nList of available users:\n%s\n", user_list);
}

int register_user_on_server() {
	printf("Please enter the IP address:\n");
	char* ip_address;
	scanf("%s", &ip_address);
	printf("Please enter the port number:\n");
	char* port_number;
	scanf("%s", &port_number);

	int sock; /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort; /* Echo server port */

    char *echoString; /* String to send to echo server */
    char echoBuffer[100]; /* Buffer for echo string */
    unsigned int echoStringLen; /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd; /* Bytes read in single recv() and total bytes read */

	/* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError(" socket () failed") ;


	/* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(&ip_address); /* Server IP address */
    echoServAddr.sin_port = htons(atoi(&port_number)); /* Server port */


	/* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError(" connect () failed");
    else
    	printf("\nConnected to server!\n");
	
	if(get_user_details(sock)) {
		printf("User Registered! Connect to Server again to Login!\n");
		close(sock);
		return 0;
	}
	else
	{
		printf("Auth Failed! Disconnecting from server.");
		close(sock);
		return 0;
	}
}

int get_user_details(int sock)
{
	char option[1];
	option[0] = '4';
	
	if (send(sock, option, strlen(option), 0) != strlen(option))
		DieWithError("Unable to send get_list_of_users option");

	char* username;
	char* password;
	char* isValid;
	isValid = malloc(4);
	printf("Please enter a single-word Username to register: \n");
	scanf("%s", &username);
	printf("Please enter a single-word Password to register: \n");
	scanf("%s", &password);

	if (send(sock, &username, strlen(&username), 0) != strlen(&username))
		DieWithError("Unable to send username");
	sleep(2);
	if (send(sock, &password, strlen(&password), 0) != strlen(&password))
		DieWithError("Unable to send password");
	if (recv(sock, &isValid, 1, 0) <= 0)
		DieWithError("Unable to complete authenitcation");
    else
    	return atoi(&isValid);
	return 0;
}


int register_new_user() {
	char* username;
	char* password;
	printf("Enter Username: \n");
	scanf("%s", &username);
	printf("Enter Password: \n");
	scanf("%s", &password);
	printf("The received username is: %s\n", &username);
	printf("The received password is: %s\n", &password);

	int registration_successful = write_user_data_to_file(&username, &password);

	if(registration_successful) {
		printf("User registered successfully\n");
	}
	else {
		printf("Error during registration. Please try again.\n");
	}
	return registration_successful;
}

int write_user_data_to_file(char* username, char* password) {
	FILE *file_writer = fopen(FILE_PATH, "a"); 

	if (file_writer == NULL) {
		printf("Error opening file.\n");
		return 0;	
	}
	fprintf(file_writer, "%s,%s\n", username, password);
	fclose(file_writer);
	return 1;
}

int validate_user(int sock)
{
	char option[1];
	option[0] = '0';
	
	if (send(sock, option, strlen(option), 0) != strlen(option))
		DieWithError("Unable to send get_list_of_users option");

	char* existing_file_contents = malloc(1000);
	char* username;
	char* password;
	char* isValid;
	isValid = malloc(4);
	printf("Enter Username: \n");
	scanf("%s", &username);
	// strncpy(&local_username, &username, strlen(&username));
	printf("Enter Password: \n");
	scanf("%s", &password);

	if (send(sock, &username, strlen(&username), 0) != strlen(&username))
		DieWithError("Unable to send username");
	sleep(2);
	if (send(sock, &password, strlen(&password), 0) != strlen(&password))
		DieWithError("Unable to send password");
	if (recv(sock, &isValid, 1, 0) <= 0)
		DieWithError("Unable to complete authenitcation");
    else
    	return atoi(&isValid);
	return 0;
}

int validate_registered_user(int sock) {
	char* existing_file_contents = malloc(1000);
	char* username;
	char* password;
	printf("Enter Username: \n");
	scanf("%s", &username);
	printf("Enter Password: \n");
	scanf("%s", &password);
	
	FILE *file_reader = fopen(FILE_PATH, "rt"); 
	if (file_reader == NULL) {
		printf("Error opening file.\n");
		return 0;	
	}
	existing_file_contents = fgets(existing_file_contents, 50, file_reader);

	while(existing_file_contents != NULL) {
		char* split_string = strtok(existing_file_contents, ",");
		while(split_string != NULL) {
			if(strcmp(split_string, &username) == 0) {
				split_string = strtok(NULL, ",");
				if(strcmp(split_string, &password) == 0) {
					printf("User %s logged in!\n", username);
					return 1;
				}
			}
			else {
				break;
			}
		}
		existing_file_contents = fgets(existing_file_contents, 50, file_reader);
	}
	fclose(file_reader);
	return 0;
}

int view_available_commands() {
	int option = 0;
	printf("\n\nHello User, welcome to my Client Server communication system :)\n\n");
	printf("---------------------------------------------\n\n");
	printf("0 -- Connect/Re-connect to the Server\n");
	printf("1 -- Get a list of Users\n");
	printf("2 -- Send a message\n");
	printf("3 -- Get my messages\n");
	printf("4 -- Initiate a chat with my friend\n");
	printf("5 -- Chat with my friend\n");
	printf("6 -- Register as a new User\n");
	printf("7 -- Exit\n");
	printf("Please enter your choice:\n");
	scanf("%d", &option);
	return option;
}