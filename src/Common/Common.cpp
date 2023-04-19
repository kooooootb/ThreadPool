#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>

#include "Common.h"

Request::Request(arg_t a_, arg_t b_, func_t func_) : a(a_) , b(b_) , func(func_) {}

Request::Request(procTime_t procTime_, func_t func_, arg_t a_, arg_t b_, arg_t res_) :
    procTime(procTime_) , func(func_) , a(a_) , b(b_) , res(res_) {}

std::function<arg_t(arg_t, arg_t)> Request::getFunction() const {
    switch(func){
        case '+': {
            return [](arg_t a_, arg_t b_) -> arg_t {
                if(enableDelay) std::this_thread::sleep_for(std::chrono::milliseconds(500));
                return a_ + b_;
            };
        }
        case '-': {
            return [](arg_t a_, arg_t b_) -> arg_t {
                if(enableDelay) std::this_thread::sleep_for(std::chrono::milliseconds(300));
                return a_ - b_;
            };
        }
        case '*': {
            return [](arg_t a_, arg_t b_) -> arg_t {
                if(enableDelay) std::this_thread::sleep_for(std::chrono::milliseconds(700));
                return a_ * b_;
            };
        }
        case '/': {
            return [](arg_t a_, arg_t b_) -> arg_t {
                if(enableDelay) std::this_thread::sleep_for(std::chrono::milliseconds(900));
                return a_ / b_;
            };
        }
        default: {
            std::stringstream resStream;
            resStream << "Invalid function type: " << func << std::endl;
            std::clog << resStream.str();

            return [](arg_t, arg_t) -> arg_t { return 0; };
        }
    }
}

void *Request::serialize() const {
    void *result = (procTime_t *) malloc(getLength());
    void *temp = result;

    *((procTime_t *) temp) = procTime;
    temp = (void *) ((procTime_t *) temp + 1);

    *((func_t *) temp) = func;
    temp = (void *) ((func_t *) temp + 1);

    *((arg_t *) temp) = a;
    temp = (void *) ((arg_t *) temp + 1);

    *((arg_t *) temp) = b;
    temp = (void *) ((arg_t *) temp + 1);

    *((arg_t *) temp) = res;

    return result;
}

Request Request::deserialize(void *ptr) {
    procTime_t procTime_ = *((procTime_t*)ptr);
    func_t func_ = *((func_t*)((procTime_t*)ptr + 1));
    arg_t a_ = *((arg_t*)((func_t*)((procTime_t*)ptr + 1) + 1));
    arg_t b_ = *((arg_t*)((func_t*)((procTime_t*)ptr + 1) + 1) + 1);
    arg_t res_ = *((arg_t*)((func_t*)((procTime_t*)ptr + 1) + 1) + 2);

    return { procTime_, func_, a_, b_, res_ };
}

constexpr size_t Request::getLength() {
    return sizeof(procTime) + sizeof(func) + sizeof(a) + sizeof(b) + sizeof(res);
}

std::ostream &operator<<(std::ostream &ostream, const Request &request) {
    std::stringstream resStream;
    resStream << "[Request: processing time: " << request.procTime << "; arguments: " << request.a << ", " <<
        request.b << "; result: " << request.res << ", function: " << request.func << "]" << std::endl;
    return ostream << resStream.str();
}

std::string Request::fullReport() const {
    std::stringstream resStream;
    resStream << "[Request: processing time: " << procTime << "; arguments: " << a << ", " <<
              b << "; result: " << res << "; function: " << func << "]" << std::endl;
    return resStream.str();
}

std::string Request::partialReport() const {
    std::stringstream resStream;
    resStream << "[Request: arguments: " << a << ", " <<
              b << "; function: " << func << "]" << std::endl;
    return resStream.str();
}

Message::Message(const Request &request) {
    message = request.serialize();
}

Message::Message() {
    message = (void *) malloc(Request::getLength());
}

Message::~Message() {
    free(message);
}

void *Message::getMessage() const {
    return message;
}

Request Message::makeRequest() const {
    return Request::deserialize(message);
}
