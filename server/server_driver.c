#include <stdio.h>
#include <stdlib.h>
#include "include/utils.h"
#include "include/server.h"


int main(int argc, char* argv[]) {
    if(argc < 2)
    {
        printf("Usage : %s <running : 0 - for false, 1 - for true>\n", argv[0]);
        return FAILURE;
    }


    load_env(".env");
    //the main driver resides here!
    char* address = getenv("IP");
    char* port = getenv("PORT");
    printf("listening at %s:%s\n", address, port);

    
    if(server_engine("127.0.0.1", "8080", atoi(argv[1])) != SUCCESS) {
        printf("Server crash\n");
        return FAILURE;
    }

    
    
    return SUCCESS;
}
