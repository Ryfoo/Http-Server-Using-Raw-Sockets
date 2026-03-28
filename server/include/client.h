/*
    client.h
    this is the main driver function that launches a client,
    it connects directly to a specified file descriptor.
    for any types confusion seek -> server/include/common.h
    
*/


#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#include "../include/socket.h"
#include "../include/common.h"

/*
    Initializes a connection to a remote server.
    Symmetric to server_engine() but connects instead of listens.
*/
success_flag_t client_engine(
                                char* ip, 
                                char* port,
                                char* method,
                                char* uri, 
                                header_t headers[HEADERS_LEN],
                                size_t headers_count,
                                char* body
                            );

#endif