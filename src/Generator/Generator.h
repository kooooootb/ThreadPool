#ifndef THREADPOOL_GENERATOR_H
#define THREADPOOL_GENERATOR_H

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#include "Common.h"

class Generator {
private:
    int fd;

public:
    explicit Generator();

    ~Generator();

    void sendRequest(arg_t a, arg_t b, func_t func) const;
    void sendRequest(const Request &request) const;

    [[nodiscard]]arg_t receiveResult() const;
    [[nodiscard]]Request receiveRequest() const;
};


#endif //THREADPOOL_GENERATOR_H
