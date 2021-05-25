#pragma once
#include "UART.hpp"
#include <stdio.h>
#define BUF_SIZE 32



class Target_IF
{
public:
    Target_IF();
    ~Target_IF();
    void startDetection(int numberOfPlayers);
    void stopDetection();
    int updateScore(int user);
private:
    UART UARTobj_;
};
