#include "../include/client.h"

/*
    Mirrors server_engine() on the client side.
    Instead of bind() + listen() + accept(),
    the client only needs socket() + connect().
*/
success_flag_t client_engine(string ip, string port) {
    struct sockaddr_in addr;
    int fd;

    /*
        Reuse the same address_init() from the server side —
        the struct sockaddr preparation is identical,
        same ip, same port, same family.
    */
    address_init(&addr, ip, port);

    /*
        connection_starter() is the client equivalent of listening_starter().
        calls socket() with IPv4 parameters
        then calls connect() to reach the remote server.
    */
    fd = connection_starter(&addr);
    if (fd < 0) {
        return -1;
    }

    /*
        Heart of the client — symmetric to monitor() on the server.
        sends requests and receives responses
        over the established file descriptor.
    */
    if (exchange(fd, &addr) < 0) {
        printf("Error connecting to the server\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}