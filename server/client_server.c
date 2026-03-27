#include <stdio.h>
#include <stdlib.h>
#include "include/utils.h"
#include "include/client.h"
int main(int args, char* argvs[]) {
    load_env("server/.env");
    char* address = getenv("IP");
    char* port = getenv("PORT");
    if(client_engine(address, port) < 0) {
        printf("client disconnected!\n");
        return -1;
    }
    return 0;
}