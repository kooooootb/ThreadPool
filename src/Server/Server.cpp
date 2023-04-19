#include <chrono>
#include <iostream>

#include "Server.h"

Server::Server(size_t poolSize)  {
    threadPool = new ThreadPool(poolSize);

    configureSocket();

    sendingThread = new std::thread(&Server::sendResults, this);
}

Server::~Server() {
    delete threadPool;

    stopSending = true;
    sendingThread->join();

    delete sendingThread;

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

    // convert message to request
    Message message;
    read(newFd, message.getMessage(), Request::getLength());
    Request request = message.makeRequest();

    // log to console
    std::cout << request.partialReport();

    // save time
    auto addTime = std::chrono::steady_clock::now();

    // get message's task
    auto task = request.getFunction();

    // add task to pool
    task_id_t taskId = threadPool->addTask(task, request.a, request.b);

    // save initial time
    auto initTime = std::chrono::steady_clock::now();

    // save id with request for sending back
    std::unique_lock<std::mutex> lock(runningTasksMtx);
    runningTasks.insert({taskId, {initTime, request, newFd}});
    lock.unlock();
}

void Server::sendResults() {
    while(!stopSending){
        result_q_t resultTask;

        // get some result from pool
        try{
            resultTask = threadPool->popResult(); // will wait here
        } catch(std::runtime_error &ex){
            std::cout << ex.what() << std::endl;
            return;
        }

        task_id_t& taskId = resultTask.first;
        arg_t& result = resultTask.second;

        // get local record on this id
        std::unique_lock<std::mutex> lock(runningTasksMtx);
        auto it = runningTasks.find(taskId);
        storing_t record = it->second;
        runningTasks.erase(it);
        lock.unlock();

        // get request and fill it
        Request &request = record.request;
        // get socket fd
        int socketFd = record.socketFd;
        // result of operation
        request.res = result;
        // processed time
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - record.initTime);
        request.procTime = static_cast<double>(duration.count()) / 1e6;

        // convert to message
        Message message(request);

        // send it
        send(socketFd, message.getMessage(), Request::getLength(), 0);

        // close socket
        close(socketFd);
    }
}
