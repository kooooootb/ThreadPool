#ifndef MAIN_COMMON_H
#define MAIN_COMMON_H

#include <functional>
#include <stdexcept>

using procTime_t = double;
using func_t = uint8_t;
using arg_t = double;

struct Request{
    procTime_t procTime;
    func_t func;
    arg_t a, b, res;

    static constexpr bool enableDelay = true;

    Request(arg_t a_, arg_t b_, func_t func_);
    Request(procTime_t procTime_, func_t func_, arg_t a_, arg_t b_, arg_t res_);

    [[nodiscard]]std::function<arg_t(arg_t, arg_t)> getFunction() const;

    [[nodiscard]]void *serialize() const;
    static Request deserialize(void *ptr);
    static constexpr size_t getLength();

    [[nodiscard]]std::string fullReport() const;
    [[nodiscard]]std::string partialReport() const;
    friend std::ostream &operator<<(std::ostream &ostream, const Request &request);
};

class Message{
private:
    void *message;

public:
    explicit Message(const Request &request);
    Message();

    ~Message();

    [[nodiscard]]void *getMessage() const;
    [[nodiscard]]Request makeRequest() const;
};

static constexpr int SERVER_PORT = 8080;
static constexpr char SERVER_ADDRESS[] = "127.0.0.1";

#endif //MAIN_COMMON_H
