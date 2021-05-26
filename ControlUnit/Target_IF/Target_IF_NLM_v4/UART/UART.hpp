#pragma once
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdint>
#include <functional>

#include <string>
using namespace std;

#include "Message.hpp"

#define debug_UART 0

class UART
{
public:
    UART(string = "");
    ~UART();
    void writeMsg(uint8_t *, uint8_t);
    uint8_t readMsg(uint8_t *, uint8_t) const;
    void UART_Dispatcher(unsigned id, Message* msg);
    void addCallback(function<void(Message*)> cb) {
		callbackTargetupdateScore_ = cb;
	}
private:
    int fd_;
    function<void(Message*)> callbackTargetupdateScore_;
};

