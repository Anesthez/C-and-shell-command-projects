#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "friends.h"


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef PORT
  #define PORT 56992
#endif
#define MAX_BACKLOG 5
#define MAX_CONNECTIONS 12
#define BUF_SIZE 124
#define INPUT_BUFFER_SIZE 256
#define INPUT_ARG_MAX_NUM 12



struct sockname {
    int sock_fd;
    char *username;
};

int tokenize(int sock_fd, char *cmd, char **cmd_argv);
int find_network_newline(const char *buf, int n);
int partial_read(int fd, char *buf, int size);

/* 
 *Write an intended value to the according fd message with 
 *error checking
 */
void Write(int sock_fd, char *msg) {
    if(write(sock_fd, msg, strlen(msg)) != strlen(msg)){
        perror("write");
        exit(1);
    }
}

/* 
 * Read and process commands
 * Return:  -1 for quit command
 *          0 otherwise
 */

int process_args(int sock_fd, char *name, struct sockname *users, 
                 int cmd_argc, char **cmd_argv, User **user_list_ptr) {
    User *user_list = *user_list_ptr;

    if (cmd_argc <= 0) {
        return 0;
    } else if (strcmp(cmd_argv[0], "quit") == 0 && cmd_argc == 1) {
        return -1;
    } else if (strcmp(cmd_argv[0], "list_users") == 0 && cmd_argc == 1) {
        //send to the user message we are having
        char *buf = list_users(user_list);
        Write(sock_fd, buf);
        free(buf);
    } else if (strcmp(cmd_argv[0], "make_friends") == 0 && cmd_argc == 2) {
        switch (make_friends(name, cmd_argv[1], user_list)) {
            case 1:
                Write(sock_fd, "users are already friends\r\n");
                break;
            case 2:
                Write(sock_fd, "at least one user you entered has the max number of friends\r\n");
                break;
            case 3:
                Write(sock_fd, "you must enter two different users\r\n");
                break;
            case 4:
                Write(sock_fd, "at least one user you entered does not exist\r\n");
                break;
        }
    } else if (strcmp(cmd_argv[0], "post") == 0 && cmd_argc >= 3) {
        // first determine how long a string we need
        int space_needed = 0;
        for (int i = 2; i < cmd_argc; i++) {
            space_needed += strlen(cmd_argv[i]) + 1;
        }

        // allocate the space
        char *contents = malloc(space_needed);
        if (contents == NULL) {
            perror("malloc");
            exit(1);
        }

        // copy in the bits to make a single string
        strcpy(contents, cmd_argv[2]);
        for (int i =3; i < cmd_argc; i++) {
            strcat(contents, " ");
            strcat(contents, cmd_argv[i]);
        }

        User *author = find_user(name, user_list);
        User *target = find_user(cmd_argv[1], user_list);
        switch (make_post(author, target, contents)) {
            case 1:
                Write(sock_fd, "the users are not friends\r\n");
                break;
            case 2:
                Write(sock_fd, "at least one user you entered does not exist\r\n");
                break;
            default:
                for (int index = 0; index < MAX_CONNECTIONS; index++) {
                    if(users[index].username != NULL &&
                       strcmp(users[index].username, cmd_argv[1]) == 0){
                       //notify the user they have new messages
                        Write(users[index].sock_fd, "You have new message!\r\n");
                    }
                }

                break;
        }
    } else if (strcmp(cmd_argv[0], "profile") == 0 && cmd_argc == 2) {
        User *user = find_user(cmd_argv[1], user_list);
        if (strlen(print_user(user)) == 0) {
            Write(sock_fd, "user not found\r\n");
        } else {
            //write to the user the message we have
            char *buf = print_user(user);
            Write(sock_fd, buf);
            free(buf);
        }
    } else {
        Write(sock_fd, "Incorrect syntax\r\n");
    }
    return 0;
}



/*
 * Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
int accept_connection(int fd, struct sockname *users) {
    int user_index = 0;
    while (user_index < MAX_CONNECTIONS && users[user_index].sock_fd != -1) {
        user_index++;
    }
    //accept connection

    if (user_index == MAX_CONNECTIONS) {
        fprintf(stderr, "server: max concurrent connections\n");
        return -1;
    }
    //when connection is maximum

    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }
    //error checking for acception

    users[user_index].sock_fd = client_fd;
    users[user_index].username = NULL;
    //setting up connected but nameless user
    return client_fd;
}

/*
*Method to read from a user, and determine wheter to send welcome message
*or send the message to processs
*
*return non zero values to indicate user exitting user
*/
int read_from(int client_index, struct sockname *users, User **user_list_ptr) {
    int fd = users[client_index].sock_fd;
    char *buf;
    buf = malloc(BUF_SIZE + 1);
    if(buf == NULL){
        perror("malloc");
        exit(1);
    }//error checking for malloc

    /* In Lab 10, you focused on handling partial reads. For this lab, you do
     * not need handle partial reads. Because of that, this server program
     * does not check for "\r\n" when it reads from the client.
     */
    if (users[client_index].username == NULL){
        int num_read = partial_read(users[client_index].sock_fd, buf, BUF_SIZE);
        //partial read the message

        if (num_read == 0) {
            users[client_index].sock_fd = -1;
            free(buf);
            return fd;
        }
        //find that certain user exits
        char *name;

        if(num_read > 31){
            name = malloc(32);
            if(name == NULL){
                perror("malloc");
                exit(1);
            }

            memset(name, '\0', 32);
            strncpy(name, buf, 31);
            name[31] = '\0';
            Write(users[client_index].sock_fd, "name longer than 31, truncate to 31\r\n");
            // name greater than 31
        }else{
            name = malloc(num_read);
            if(name == NULL){
                perror("malloc");
                exit(1);
            }

            memset(name, '\0', num_read);
            strcpy(name, buf);
            name[num_read - 1] = '\0';

            //normal name
        }
        users[client_index].username = name;



        buf[num_read] = '\0';
        switch (create_user(name, user_list_ptr)) {
            case 1:
                Write(users[client_index].sock_fd, "Welcome back!\r\n");
                Write(users[client_index].sock_fd, "Go ahead and write some command\r\n");
                break;
            default:
                Write(users[client_index].sock_fd, "Welcome\r\n");
                Write(users[client_index].sock_fd, "Go ahead and write some command\r\n");
        }

        //welcome message depending on wheter the user is new user

    }else{        
        memset(buf, '\0', BUF_SIZE + 1);
        int num_read = partial_read(fd, buf, BUF_SIZE);

        if (num_read == 0) {
            users[client_index].sock_fd = -1;
            free(buf);
            return fd;
        }// user exit


        char *cmd_argv[INPUT_ARG_MAX_NUM];
        int cmd_argc = tokenize(users[client_index].sock_fd, buf, cmd_argv);
        
        if (cmd_argc > 0 && process_args(users[client_index].sock_fd, users[client_index].username,  users, cmd_argc, cmd_argv, user_list_ptr) == -1) {
            users[client_index].sock_fd = -1;
            free(buf);
            return fd; // can only reach if quit command was entered
        }





    }

    free(buf);
    return 0;
}


/*
 * Tokenize the string stored in cmd.
 * Return the number of tokens, and store the tokens in cmd_argv.
 */
int tokenize(int sock_fd, char *cmd, char **cmd_argv) {
    int cmd_argc = 0;
    char *next_token = strtok(cmd, " \r\n");    
    while (next_token != NULL) {
        if (cmd_argc >= INPUT_ARG_MAX_NUM - 1) {
            Write(sock_fd, "Too many arguments!");
            cmd_argc = 0;
            break;
        }
        cmd_argv[cmd_argc] = next_token;
        cmd_argc++;
        next_token = strtok(NULL, " \r\n");
    }

    return cmd_argc;
}



int main(void) {
    User *user_list = NULL;

    struct sockname users[MAX_CONNECTIONS];
    for (int index = 0; index < MAX_CONNECTIONS; index++) {
        users[index].sock_fd = -1;
        users[index].username = NULL;
    }

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // This sets an option on the socket so that its port can be reused right
    // away. Since you are likely to run, stop, edit, compile and rerun your
    // server fairly quickly, this will mean you can reuse the same port.
    int on = 1;
    int status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
                            (const char *) &on, sizeof(on));
    if (status == -1) {
        perror("setsockopt -- REUSEADDR");
    }

    // This should always be zero. On some systems, it won't error if you
    // forget, but on others, you'll get mysterious errors. So zero it.
    memset(&server.sin_zero, 0, 8);

    // Bind the selected port to the socket.
    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }

    // The client accept - message accept loop. First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);

    while (1) {
        // select updates the fd_set it receives, so we always use a copy and retain the original.
        fd_set listen_fds = all_fds;
        if (select(max_fd + 1, &listen_fds, NULL, NULL, NULL) == -1) {
            perror("server: select");
            exit(1);
        }

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds)) {
            int client_fd = accept_connection(sock_fd, users);
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
            FD_SET(client_fd, &all_fds);
            Write(client_fd, "What's your name?\r\n");
            //send welcom message

            printf("Accepted connection\n");
        }

        // Next, check the clients.
        // NOTE: The return value of select() could be used to terminate this loop early; however,
        // the maximum number of connections is really small, so it will not have a noticeable impact.
        for (int index = 0; index < MAX_CONNECTIONS; index++) {
            if (users[index].sock_fd > -1 && FD_ISSET(users[index].sock_fd, &listen_fds)) {
                // Note: never reduces max_fd
                int client_closed = read_from(index, users, &user_list);
                if (client_closed > 0) {
                    FD_CLR(client_closed, &all_fds);
                    free(users[index].username);
                    //free not needed memory
                    printf("Client %d disconnected\n", client_closed);
                } else {
                    printf("responding message from client %d\n", users[index].sock_fd);
                }
            }
        }
    }

    // Should never get here.
    return 1;
}

/*
 * Search the first n characters of buf for a network newline (\r\n).
 * Return one plus the index of the '\n' of the first network newline,
 * or -1 if no network newline is found. The return value is the index into buf
 * where the current line ends.
 * Definitely do not use strchr or other string functions to search here. (Why not?)
 */
int find_network_newline(const char *buf, int n) {
    char nbuf[n + 1];
    strncpy(nbuf, buf, n);
    nbuf[n] = '\0';
    for(int i = 0; i < n - 1; i++){
        if(nbuf[i] == '\r' &&
           nbuf[i + 1] == '\n'){
            return i + 2;
        }
    }
    return -1;
}

/*
*partial_read when the user may enter partial messages
*return 0 if a fd is disconnected
*
*
*/

int partial_read(int fd, char *buf, int size) {
    memset(buf, '\0', size);
    int inbuf = 0;          
    int room = size;  
    char *after = buf;      
    int nbytes;
    
    while ((nbytes = read(fd, after, room)) > 0) {
        inbuf += nbytes;
        int where;
        if((where = find_network_newline(buf, inbuf)) < 0) {
            buf[where - 2] = '\0';
            return inbuf;
        }
        after = &after[inbuf];
        room = room - inbuf;
    }

    if(nbytes < 0){
        perror("read");
        exit(1);
    }//error checking for read

    return 0;
    
}

