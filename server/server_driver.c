#include <stdio.h>
#include <stdlib.h>
#include "include/utils.h"
#include "include/server.h"
int main(int args, char* argvs[]) {
    load_env("server/.env");
    //the main driver resides here!
    char* address = getenv("IP");
    char* port = getenv("PORT");
    if(server_engine(address, port) < 0) {
        printf("Server crash\n");
        return -1;
    }
    return 0;
}