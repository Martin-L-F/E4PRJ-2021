#pragma once
#include "UART.hpp"
#include "Message.hpp"
#include <stdio.h>

#define BUF_SIZE 32
#define Target_IF_PSOC_ID "0C0E040A00228400" //PSOC USB-ID number as string


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