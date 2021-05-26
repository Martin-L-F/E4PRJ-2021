#pragma once
#include "UART/UART.hpp"
#include "UART/Message.hpp"
#include <stdio.h>
#include <functional>

#define BUF_SIZE 32
#define Target_IF_PSOC_ID "0C0E040A00228400" //PSOC USB-ID number as string


class Target_IF
{
public:
    Target_IF();
    ~Target_IF();
    void startDetection(int numberOfPlayers);
    void stopDetection();
    int updateScore(Message* user); //Handler in TargetIF_Dispatcher
    UART* getUARTobj();
    void addCallback(function<void(int)> cb) {
		callbackControllershotDetected_ = cb;
	}
private:
    UART UARTobj_;
    function<void(int)> callbackControllershotDetected_;
};



