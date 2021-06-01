#pragma once
#include "UART/UART.hpp"
#include "UART/Message.hpp"

#include <stdio.h>
#include <iostream>
#include <functional>

using namespace std;
using namespace chrono;

#define BUF_SIZE 32
#define Target_IF_PSOC_ID "0C0E040A00228400" //PSOC USB-ID number as string: KitProg_"this number"-if02. Put "0" if there is none


class Target_IF
{
public:
    Target_IF();
    ~Target_IF();
    void startDetection(int numberOfPlayers);
    void stopDetection();
    void UARTDispatcher(unsigned id, Message* msg);
    int updateScore(Message* user);                 //Handler
    void addCallback(function<void(int)> cb);
private:
    UART UARTobj_;
    function<void(int)> callbackControllershotDetected_;
};



