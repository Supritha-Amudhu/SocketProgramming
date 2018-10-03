README.md

Description:
This is a client-server communcation program. There is a client program and a server program which communicates among each other by establishing a connection. Whenever a connection has to be established, a new port gets created, binded and then the port would be ready to send or receive messages.

Input files:
1. registered_users.txt - The text file that contains a list of registered users and their password.
2. messages.txt - The text file that contains a list of messages that are shared between users in the application.


C files:
1. Server.c - The program where the code for Server is invoked from.
2. Client.c - The program where the code for Client is invoked from.
3. main.c - The main program that has to be invoked for the client-server program to run.


Steps to option 6 - Register a new user:
(Please do not start with option 0 to connect to server. Start with option 6 to connect to server and register as a new user. Please choose the username as one word, password as one word.)
1. In a terminal window, compile and run Server.c
2. In a new terminal window, compile and run main.c
3. Choose option 6 to register as a new user.
4. Enter IP address and Port number.
5. Enter new username and password.
6. Data gets sent to the server and is then stored in the file registered_users.txt.
7. Select option 0 and enter IP, port and Login with credentials.
8. The registered user now can select from options 0-7 to login and then perform operations.

Steps to option 1 - Get user list:
1. In a terminal window, compile and run server.c.
2. In a new terminal window, compile and run main.c.
3. Select option 0.
4. Enter Username and password. Wait for validation.
5. After validation, choose option 1.

Steps to option 2 - Send a message:
1. In a terminal window, compile and run server.c.
2. In a new terminal window, compile and run main.c.
3. Select option 0.
4. Enter Username and password. Wait for validation.
5. After validation, choose option 2.
6. Type name of a user to send messages to.
7. Enter message and hit enter.

Steps to option 3 - Get my messages:
1. In a terminal window, compile and run server.c.
2. In a new terminal window, compile and run main.c.
3. Select option 0.
4. Enter Username and password. Wait for validation.
5. After validation, choose option 3.
6. View your list of messages.

Steps to option 4 - Initiate a chat with my friend:
1. In a terminal window, compile and run server.c.
2. In a new terminal window, compile and run main.c.
3. Select option 0.
4. Enter Username and password. Wait for validation.
5. After validation, choose option 4.
6. Enter the port number in which you are available.
7. Wait for another client to join the chat. Do not close the window.

Steps to option 5 - Chat with my friend:
1. If server.c is not running already, in a terminal window, compile and run server.c.
2. In a new terminal window, compile and run main.c.
3. Select option 0.
4. Enter Username and password. Wait for validation.
5. After validation, choose option 5.
6. Enter the IP address and port number in which you want to chat with another client.
7. IP and port number validates and you are now ready to chat.

Steps to option 7 - Exit:
1. In a terminal window, compile and run server.c.
2. In a new terminal window, compile and run main.c.
3. Select option 0.
4. Enter Username and password. Wait for validation.
5. After validation, choose option 7.
6. This option can also be chosen anytime else in the program.
