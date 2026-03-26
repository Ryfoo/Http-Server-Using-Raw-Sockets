#ifndef CLIENT_H
#define CLIENT_H

#include "../include/server.h"

/*
    Initializes a connection to a remote server.
    Symmetric to server_engine() but connects instead of listens.
*/
success_flag_t client_engine(string ip, string port);

#endif