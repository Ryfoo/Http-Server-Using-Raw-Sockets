#include "../include/client.h"

/*
    Mirrors server_engine() on the client side.
    Instead of bind() -> listen() -> accept(),
    the client only needs socket() -> connect().
*/
success_flag_t client_engine(char* ip, char* port) {
    /*
        Note that sockaddr_in data type is heavily used in that code
        if you can't figure out what type of data it carries,
        it's advised to use the manual
    */
    struct sockaddr_in addr;
    int fd;

    /*
        use address_init() from socket.h
    */
    address_init(&addr, ip, port);

    /*
        connection_starter() is the client equivalent of listening_starter().
        calls socket() with IPv4 parameters
        then calls connect() to reach the remote server.
    */
    fd = connection_starter(&addr);
    if (fd < 0) {
        return FAILURE;
    }

    /*
        Heart of the client — symmetric to monitor() on the server.
        sends requests and receives responses
        over the established file descriptor.
    */
    if (exchange(fd, &addr) < 0) {
        printf("Error sending data to the server\n");
        close(fd);
        return FAILURE;
    }

    close(fd);
    return SUCCESS;
}