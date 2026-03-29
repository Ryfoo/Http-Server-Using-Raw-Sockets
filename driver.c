#include <stdio.h>
#include <stdlib.h>
#include "server/include/utils.h"
#include "server/include/server.h"


int main(int argc, char* argv[]) {
    if(argc < 3)
    {
        printf("Usage : %s <ip> <port>\n", argv[0]);
        return FAILURE;
    }



    printf("listening at %s:%s\n", argv[1], argv[2]);

    
    if(server_engine(argv[1], argv[2], 1) != SUCCESS) {
        printf("Server crash\n");
        return FAILURE;
    }

    
    
    return SUCCESS;
}
