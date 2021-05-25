#pragma once
#include "UART.hpp"
#include "Message.hpp"
#include <stdio.h>

#define BUF_SIZE 32



class Target_IF
{
public:
    Target_IF();
    ~Target_IF();
    void startDetection(int numberOfPlayers);
    void stopDetection();
private:
    UART UARTobj_;
};


int updateScore(Message* user); //Handler in TargetIF_Dispatcher
void TargetIF_Dispatcher(unsigned id, Message* msg);