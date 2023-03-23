#ifndef THREADPOOL_SERVER_H
#define THREADPOOL_SERVER_H

#include <netinet/in.h>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>

#include "ThreadPool.h"

class Server {
private:
    ThreadPool *threadPool;
    int fd;
    sockaddr_in addrIn;

    void configureSocket();
public:
    explicit Server(size_t poolSize);

    ~Server();

    void processRequest();
};


#endif //THREADPOOL_SERVER_H
