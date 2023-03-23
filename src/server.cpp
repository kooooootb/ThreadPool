#include <iostream>

#include "Server.h"

int main(){
    Server server(32);

    do{
        server.processRequest();
    } while(true);

    return 0;
}