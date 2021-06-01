#pragma once
#include "UART/UART.hpp"
#include "UART/Message.hpp"

#include <stdio.h>
#include <iostream>
#include <functional>

using namespace std;
using namespace chrono;

#define BUF_SIZE 32
#define RailSystem_IF_PSOC_ID "1106040A00228400"//"040F093201324400" //PSOC USB-ID number as string: KitProg_"this number"-if02. Put "0" if there is none

class RailSystem_IF
{
public:
    RailSystem_IF();
    ~RailSystem_IF();
    void startMoving(int);
    void resetPosition();                               //Start kalibrate
    void stopMoving();
    void UARTDispatcher(unsigned id, Message* msg);
    int CalibrateDone(Message* user);                   //Handler
    void addCallback(function<void()> cb);
    UART* getUARTobj(); //test
private:
    UART UARTobj_;
    function<void()> callbackControllerCalibrateComplete_;
};



