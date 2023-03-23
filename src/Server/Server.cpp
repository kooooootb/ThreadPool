#include <chrono>
#include <iostream>

#include "Server.h"

Server::Server(size_t poolSize) {
    threadPool = new ThreadPool(poolSize);

    configureSocket();
}

Server::~Server() {
    delete threadPool;

    shutdown(fd, SHUT_RDWR);
}

void Server::configureSocket() {
    // opening socket
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        throw std::runtime_error("failed opening server socket");
    }

    // setting socket's options
    int option = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option))){
        throw std::runtime_error("failed setting options");
    }

    // setting address
    addrIn = {AF_INET, htons(SERVER_PORT), INADDR_ANY};
    if(bind(fd, (sockaddr *) &addrIn, sizeof(addrIn)) < 0){
        throw std::runtime_error("failed setting address");
    }

    // open listener
    if(listen(fd, 16) < 0){
        throw std::runtime_error("failed listen");
    }
}

void Server::processRequest(){
    // accept message
    int newFd;
    int addrInLength = sizeof(addrIn);
    if((newFd = accept(fd, (sockaddr *) &addrIn, (socklen_t *)&addrInLength)) < 0){
        throw std::runtime_error("failed accepting");
    }

    // generate request
    Message message;
    read(newFd, message.getMessage(), Request::getLength());
    Request request = message.makeRequest();

    // log to console
    std::cout << request.partialReport();

    // save time
    auto addTime = std::chrono::steady_clock::now();

    // generate task
    auto task = [addTime, newFd, request](arg_t a, arg_t b) mutable -> void{
        // get function
        auto func = request.getFunction();

        // get result
        request.res = func(a, b);

        // calculate processing time
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - addTime);
        request.procTime = static_cast<double>(duration.count()) / 1e6;

        // send response
        Message message(request);
        send(newFd, message.getMessage(), Request::getLength(), 0);

        // close socket
        close(newFd);
    };

    // add task to pool
    threadPool->addTask(task, request.a, request.b);
}
