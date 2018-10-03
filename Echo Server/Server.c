#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include "DieWithError.c"
#include "HandleTCPClient.c"

#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define FILE_PATH "registered_user_list.txt"
#define MESSAGES_PATH "messages.txt"

void DieWithError(char *errorMessage); /* Error handling function */
void HandleTCPClient(int clntSocket); /* TCP client handling function */
char* HandleUserValidation(int clntSocket);
void HandleUserAction(int clntSocket);
char validateUser(char username[], char password[]);

void getUserList(int clntSocket);
void sendMessage(int clntSocket, char *username);
void getUserMessages(int clntSocket, char *username);
int register_user(int clntSocket);
void closeSocket(int clntSocket, char *username);
char write_user_data_to_file(char* username, char* password);

int main(int argc, char *argv[])
{
    int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */

    if (argc != 2) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]) ;
        exit(1);
    }

    echoServPort = atoi(argv[1]); /* First arg: local port */

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
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed") ;
    printf("Waiting for Client..\n");
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);
        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");
        /* clntSock is connected to a client! */
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        // char* username = HandleUserValidation(clntSock);
        HandleUserAction(clntSock);
    }
/* NOT REACHED */
}

void closeSocket(int clntSocket, char *username)
{
    printf("Disconnecting %s\n\n", username);
    close(clntSocket);
}

void getUserList(int clntSocket)
{
    char* existing_file_contents = malloc(1000);
    FILE *file_reader = fopen(FILE_PATH, "rt"); 
    if (file_reader == NULL) {
        printf("Error opening file.\n");
    }

    char *user_list;
    user_list = malloc(100);
    existing_file_contents = fgets(existing_file_contents, 50, file_reader);

    while(existing_file_contents != NULL) {
        char* user_name = strtok(existing_file_contents, ",");
        if(user_name != NULL) {
            strcat(user_list, user_name);
            strcat(user_list, "\n");
            printf("%s\n", user_name);
        }
        existing_file_contents = fgets(existing_file_contents, 50, file_reader);
    }
    fclose(file_reader);
    printf("\n%s\n", user_list);

    if (send(clntSocket, user_list, strlen(user_list), 0) <= 0)
        DieWithError("send() sent a different number of bytes than expected");

}

void sendMessage(int clntSocket, char *username)
{
    char *recipient; /* Buffer for echo string */
    char message[200];
    recipient = malloc(100);
    
    char isValid[1];
    int recvMsgSize; /* Size of received message */

    /* Receive recipient from client */
    memset(message, 0, sizeof message);
    if ((recvMsgSize = recv(clntSocket, message, 200, 0)) < 0)
        DieWithError("recv() failed") ;
    else
    {
        strcat(message, "\0");
        FILE *file_writer = fopen(MESSAGES_PATH, "a"); 
        if (file_writer == NULL) {
            printf("Error opening file.\n");
        }
        else
        {
            fprintf(file_writer, "%s\n", message);
        }
        fclose(file_writer);

        char isValid[1];
        isValid[0] = '1';
        if (send(clntSocket, isValid, 1, 0) != 1)
            DieWithError("send() sent a different number of bytes than expected");
    }

}

void getUserMessages(int clntSocket, char *username)
{
    char* existing_file_contents = malloc(1000);
    FILE *file_reader = fopen(MESSAGES_PATH, "rt"); 
    if (file_reader == NULL) {
        printf("Error opening file.\n");
    }

    char *messages_with_count;
    messages_with_count = malloc(1000);

    int message_count = 0;

    char *messages;
    messages = malloc(1000);

    existing_file_contents = fgets(existing_file_contents, 100, file_reader);

    while(existing_file_contents != NULL) {
        char* message_str = strtok(existing_file_contents, "::");
        if(message_str != NULL && strcmp(message_str, username) == 0) {
            message_str = strtok(NULL, "::");
            message_count = message_count + 1;
            strcat(messages, message_str);
            printf("Messages count: %d\n", message_count);
        }
        existing_file_contents = fgets(existing_file_contents, 50, file_reader);
    }

    fclose(file_reader);
    sprintf(messages_with_count, "%s %d %s", "You have", message_count, "message(s).\n");
    strcat(messages_with_count, messages);
    if (send(clntSocket, messages_with_count, strlen(messages_with_count), 0) <= 0)
        DieWithError("send() sent a different number of bytes than expected");
}

void HandleUserAction(int clntSocket)
{
    char *user_option;
    user_option = malloc(2);
    int recvMsgSize;
    char *username;
    printf("waiting for next user option\n");
    while((recvMsgSize = recv(clntSocket, user_option, 2, 0)) > 0)
    {
        printf("received next user option: --%c--\n", user_option[0]);
        int wait_for_request = 1;
        
        if (user_option[0] == '0')
        {
            username = HandleUserValidation(clntSocket);
        }
        else if (user_option[0] == '1')
        {
            getUserList(clntSocket);
        }
        else if (user_option[0] == '2')
        {
            sendMessage(clntSocket, username);
        }
        else if (user_option[0] == '3')
        {
            getUserMessages(clntSocket, username);
        }
        else if (user_option[0] == '4')
        {
            register_user(clntSocket);
            username = malloc(100);
            strcat(username, "New User\0");
        }
        else
        {
           break;
        }
        recvMsgSize = 0;
        free(user_option);
        user_option = malloc(2);
    }
    closeSocket(clntSocket, username);
}

int register_user(int clntSocket)
{
    char *username; /* Buffer for echo string */
    char *password;
    username = malloc(100);
    password = malloc(100);
    char isValid[1];
    int recvMsgSize; /* Size of received message */

    /* Receive username from client */
    if ((recvMsgSize = recv(clntSocket, username, 10, 0)) < 0)
        DieWithError("recv() failed") ;
    else
        printf("Username: %s\n", username);

    /* Receive username from client */
    if ((recvMsgSize = recv(clntSocket, password, 10, 0)) < 0)
        DieWithError("recv() failed") ;
    else
        printf("Password: %s\n", password);

    isValid[0] = write_user_data_to_file(username, password);

    if (send(clntSocket, isValid, 1, 0) != 1)
        DieWithError("send() sent a different number of bytes than expected");

    return 1;
}

char write_user_data_to_file(char *username, char *password) {
    FILE *file_writer = fopen(FILE_PATH, "a"); 
    strcat(username, "\0");
    strcat(password, "\0");
    if (file_writer == NULL) {
        printf("Error opening file.\n");
        return '0';   
    }
    fprintf(file_writer, "%s,%s\n", username, password);
    fclose(file_writer);
    return '1';
}


char* HandleUserValidation(int clntSocket)
{
    char *username; /* Buffer for echo string */
    char *password;
    username = malloc(100);
    password = malloc(100);
    char isValid[1];
    int recvMsgSize; /* Size of received message */

    /* Receive username from client */
    if ((recvMsgSize = recv(clntSocket, username, 10, 0)) < 0)
        DieWithError("recv() failed") ;
    else
        printf("Username: %s\n", username);

    /* Receive username from client */
    if ((recvMsgSize = recv(clntSocket, password, 10, 0)) < 0)
        DieWithError("recv() failed") ;
    else
        printf("Password: %s\n", password);

    isValid[0] = validateUser(username, password);

    if (send(clntSocket, isValid, 1, 0) != 1)
        DieWithError("send() sent a different number of bytes than expected");

    return username;
}

char validateUser(char *username, char *password)
{
    char* existing_file_contents = malloc(1000);
    FILE *file_reader = fopen(FILE_PATH, "rt"); 
    if (file_reader == NULL) {
        printf("Error opening file.\n");
        return '0';   
    }
    existing_file_contents = fgets(existing_file_contents, 100, file_reader);
    while(existing_file_contents != NULL) {
        existing_file_contents[strlen(existing_file_contents)-1]='\0';
        char* split_string = strtok(existing_file_contents, ",");
        while(split_string != NULL) {
            if(strcmp(split_string, username) == 0) {
                split_string = strtok(NULL, ",");
                if(strcmp(split_string, password) == 0) {
                    printf("User %s logged in!\n", username);
                    fclose(file_reader);
                    return '1';
                }
            }
            else {
                break;
            }
        }
        existing_file_contents = fgets(existing_file_contents, 50, file_reader);
    }
    fclose(file_reader);
    return '0';
}
