#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef PORT
  #define PORT 30000
#endif
#define MAX_BACKLOG 5
#define MAX_CONNECTIONS 12
#define BUF_SIZE 128


struct sockname {
    int sock_fd;
    char *username;
};


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

    if (user_index == MAX_CONNECTIONS) {
        fprintf(stderr, "server: max concurrent connections\n");
        return -1;
    }

    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    users[user_index].sock_fd = client_fd;
    users[user_index].username = NULL;
    return client_fd;
}


/*
 * Read a message from client_index and echo it back to them.
 * Return the fd if it has been closed or 0 otherwise.
 *
 * Task 1: There are now two different types of input that we could
 * receive: a username, or a message. If we have not yet received
 * the username, then we should copy buf to username.  Otherwise, the
 * input will be a message to write to the socket.
 *
 * Task 2: Change the code to broadcast to all connected clients
 */
int read_from(int client_index, struct sockname *users) {
    int fd = users[client_index].sock_fd;
    char buf[BUF_SIZE + 1];

    /* In Lab 10, you focused on handling partial reads. For this lab, you do
     * not need handle partial reads. Because of that, this server program
     * does not check for "\r\n" when it reads from the client.
     */
    int num_read = read(fd, &buf, BUF_SIZE);
    buf[num_read] = '\0';
    if (num_read == 0) {
        users[client_index].sock_fd = -1;
        return fd;
    }
    if (users[client_index].username == NULL){
        char *name = malloc(sizeof(num_read));
        strcpy(name, buf);
        name[num_read - 1] = '\0';
        users[client_index].username = name;
    }else{
        for (int index = 0; index < MAX_CONNECTIONS; index++) {
            if(users[index].sock_fd != -1 &&
               users[index].username != NULL){
               char nbuf[strlen(buf) + strlen(users[client_index].username) + 3];
               char *inter = ": ";
               strcpy(nbuf, users[client_index].username);
               strcat(nbuf, inter);
               strcat(nbuf, buf);
               nbuf[strlen(buf) + strlen(users[client_index].username) + 2] = '\0';
               write(users[index].sock_fd, nbuf, BUF_SIZE);
            }
        }
    }

    return 0;
}


int main(void) {
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
            printf("Accepted connection\n");
        }

        // Next, check the clients.
        // NOTE: The return value of select() could be used to terminate this loop early; however,
        // the maximum number of connections is really small, so it will not have a noticeable impact.
        for (int index = 0; index < MAX_CONNECTIONS; index++) {
            if (users[index].sock_fd > -1 && FD_ISSET(users[index].sock_fd, &listen_fds)) {
                // Note: never reduces max_fd
                int client_closed = read_from(index, users);
                if (client_closed > 0) {
                    FD_CLR(client_closed, &all_fds);
                    free(users[index].username);
                    printf("Client %d disconnected\n", client_closed);
                } else {
                    printf("Echoing message from client %d\n", users[index].sock_fd);
                }
            }
        }
    }

    // Should never get here.
    return 1;
}
