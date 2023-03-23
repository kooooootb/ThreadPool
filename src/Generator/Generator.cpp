#include "Generator.h"
#include "Common.h"

Generator::Generator() {
    // open generator socket
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        throw std::runtime_error("failed opening generator socket");
    }

    // make server address
    sockaddr_in serverAddress = {AF_INET, htons(SERVER_PORT)};
    if(inet_pton(AF_INET, SERVER_ADDRESS, &serverAddress.sin_addr) <= 0){
        throw std::runtime_error("failed converting server address");
    }

    // connect to server
    if(connect(fd, (sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
        throw std::runtime_error("failed making connection");
    }
}

Generator::~Generator() {
    close(fd);
}

void Generator::sendRequest(arg_t a, arg_t b, func_t func) const {
    Request request(a, b, func);

    sendRequest(request);
}

void Generator::sendRequest(const Request &request) const {
    Message message(request);
    send(fd, message.getMessage(), Request::getLength(), 0);
}

arg_t Generator::receiveResult() const {
    return receiveRequest().res;
}

Request Generator::receiveRequest() const {
    Message message;
    read(fd, message.getMessage(), Request::getLength());
    return message.makeRequest();
}
