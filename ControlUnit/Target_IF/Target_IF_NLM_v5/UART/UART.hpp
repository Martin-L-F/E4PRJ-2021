#pragma once
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdint>
#include <functional>
#include <thread>

#include <string>
using namespace std;
using namespace chrono;

#include "Message.hpp"
#include "MsgQueue.hpp"
#include "Rx_data.hpp"

#define debug_UART 1

class UART
{
public:
    UART(string = "");
    ~UART();
    void writeMsg(uint8_t *, uint8_t);
    uint8_t readMsg(uint8_t *, uint8_t) const;
    void addCallback(function<void(unsigned, Message*)> cb);

    void funcUARTReceive();
    void funcUARTDispatcher();
    thread* getThreadUARTREceive(); //test
private:
    int fd_;
    function<void(unsigned, Message*)> callbackDispatcher_;
    thread ThreadUARTReceive;
    thread ThreadUARTDispatcher;
    MsgQueue msgQueueObj;
    string PSoC_ID_; //Test
};