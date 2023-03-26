#include <iostream>

#include "Server.h"

int main(){
    int poolSize;
    const char *mes = "Enter pool size: ";
    do {
        std::cout << mes;
        std::string input;
        std::getline(std::cin, input);
        try{
            poolSize = std::stoi(input);
        } catch(std::out_of_range &ex){
            mes = "Too big number, try again: ";
            poolSize = 0;
            continue;
        } catch(std::invalid_argument &ex){
            mes = "Invalid data type, try again: ";
            poolSize = 0;
            continue;
        }

        mes = "Wrong pool size, try again: ";
    }while(poolSize < 1);

    Server server(poolSize);

    do{
        server.processRequest();
    } while(true);

    return 0;
}