#ifndef _EXCEPTIONS_HPP_
#define _EXCEPTIONS_HPP_

#include <stdexcept>
class ErrMsg : public std::exception {
    private:
    const char* msg;
    public:
    ErrMsg(const char* msg) noexcept : msg(msg) {};
    const char* what() const noexcept override {
        return msg;
    }
};

#endif