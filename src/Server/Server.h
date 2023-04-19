#ifndef THREADPOOL_SERVER_H
#define THREADPOOL_SERVER_H

#include <netinet/in.h>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <unordered_map>

#include "ThreadPool.h"

class Server {
private:
    struct Storing{
        decltype(std::chrono::steady_clock::now()) initTime;
        Request request;
        int socketFd;

        Storing(decltype(std::chrono::steady_clock::now()) initTime_, Request request_, int socketFd_) :
            initTime(initTime_),
            request(request_),
            socketFd(socketFd_) {}
    };

    using storing_t = Storing;

    ThreadPool *threadPool; // threads

    int fd; // server's socket
    sockaddr_in addrIn; // somewhat address

    std::unordered_map<task_id_t, storing_t> runningTasks; // should be processed by one help thread
    std::mutex runningTasksMtx;

    std::thread *sendingThread;
    std::atomic<bool> stopSending = false;

    void configureSocket();
    void sendResults();
public:
    explicit Server(size_t poolSize);

    ~Server();

    void processRequest();
};


#endif //THREADPOOL_SERVER_H
