#include "../include/socket.h"


void address_init(struct sockaddr_in* addr, char* ip, char* port) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(port);
}

socket_fd_t listening_starter(struct sockaddr_in* addr)
{
    socket_fd_t fd;

    fd = socket(AF_INET,SOCK_STREAM, 0);
    if (fd < 0) {
        printf("Error establishing a file descriptor\n");
        return -1;
    }
    if(bind(fd, (struct sockaddr*) addr, sizeof(*addr)) < 0) {
        printf("Error binding (giving the local fd) the address : %s\n", addr->sin_addr);
        close(fd);
        return -1;
    }
    if (listen(fd, QUEUE_LIMIT) < 0) {
        printf("Error commecing the server\n");
        close(fd);
        return -1;
    }
    
    return fd;
}


success_flag_t monitor(socket_fd_t fd, struct sockaddr_in* addr)
{
    while(CONNECTION) {
        socklen_t len = sizeof(struct sockaddr_in);
        socket_fd_t shared_fd = accept(fd, (struct sockaddr *) addr, &len);
        if(shared_fd < 0) {
            perror("Error establishing a common file descriptor\n");
            continue;
        }
        printf("The connection has been established\n");
        char* buffer = malloc(BUFF_SIZE);
        if(!buffer){
            perror("Error allocating memory\n");
            close(shared_fd);
            continue;
        }
        memset(buffer, 0, BUFF_SIZE);
        buffer = strcpy(buffer, "hello mate, welcome to the team\n");
        msg_len_t sent = send(shared_fd, buffer, BUFF_SIZE, 0);
        if (sent < 0) {
            perror("Error sending data over the common file descriptor\n");
            free(buffer);
            close(shared_fd);
            continue;
        }
        memset(buffer, 0, BUFF_SIZE);
        msg_len_t received = recv(shared_fd, buffer, BUFF_SIZE, 0);
        if (sent < 0) {
            perror("Error sending data over the common file descriptor\n");
            free(buffer);
            close(shared_fd);
            continue;
        }
        success_flag_t s = handle_http_request(buffer);
        if(s < 0) {
            perror("Error handling the request\n");
            free(buffer);
            close(shared_fd);
            return -1;
        }
        free(buffer);
        close(shared_fd);
    }
    return 0;
}



/*
    Mirrors listening_starter() but for the client.
    No bind(), no listen() — just socket() + connect().
*/
socket_fd_t connection_starter(struct sockaddr_in *addr) {
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("Error creating socket\n");
        return -1;
    }

    if (connect(fd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        printf("Error connecting to server\n");
        close(fd);
        return -1;
    }

    return fd;
}

/*
    Mirrors monitor() on the server side.
    The client side of the active communication loop.
    sends a request, waits for a response.
*/
success_flag_t exchange(int fd, struct sockaddr_in *addr) {
    char buffer[1024];
    ssize_t bytes_received;

    /*
        Send a request to the server.
        serialize() to build the HTTP message first.
    */
    const char *request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if (send(fd, request, strlen(request), 0) < 0) {
        printf("Error sending request\n");
        return -1;
    }

    /*
        Wait for the server response.
        parse() to deserialize the incoming message.
    */
    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        printf("Error receiving response\n");
        return -1;
    }

    printf("Server response:\n%s\n", buffer);
    return 0;
}